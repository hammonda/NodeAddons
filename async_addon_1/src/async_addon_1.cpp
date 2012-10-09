#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <node.h>
#include <pthread.h>
#include <uv.h>
#include <v8.h>

using namespace node;
using namespace v8;

static pthread_t node_thread;

// Container holding the function call context. This is passed along
// the libuv event loop so each stage knows what its doing.
typedef struct {
  uint32_t n; // call argument
  uint32_t nthPrime; // return value
  int64_t  thread_id; // thread id of calculating thread
  Persistent<Function> cb_fn; // callback function
} async_call_t;

// Simple C++ function to calculate the nth prime number. Ultimately this is
// the function that does the work.
uint32_t nthPrime (uint32_t n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 2;
  } else {
    n--;
    uint32_t i, p = 3;
    while (n > 0) {
      i = 3;
      while (true) {
        if (i * i > p) {
          n--;
          break;
        }
        if (p % i == 0) {
          break;
        }
        i += 2;
      }
      p += 2;
    }
    return p - 2;
  }
}

// Called by the libuv event loop when its ready to do the work. In this case,
// call the nthPrime C++ function.
void Work(uv_work_t *req) {
  async_call_t* async_call = (async_call_t*)req->data;

  // Calculate the n'th prime.
  int32_t n = async_call->n;
  if (n < 0) {
    async_call->nthPrime = 0;
  } else {
    async_call->nthPrime = nthPrime(n);
  }
  async_call->thread_id = pthread_self();

  assert(async_call->thread_id != node_thread);
}

// Called by the libuv event loop when the Work function is done. This allows
// the code to clean up any memory allocated.
void Finish(uv_work_t *req) {
  assert(pthread_self() == node_thread);

  async_call_t* async_call = (async_call_t*)req->data;

  // Create the arguments used to call the callback function.
  Local<Value> argv[3];
  argv[0] = Local<Value>::New(Uint32::New(async_call->n));
  argv[1] = Local<Value>::New(Uint32::New(async_call->nthPrime));
  argv[2] = Local<Value>::New(Number::New(async_call->thread_id));

  // Invoke the registered callback function.
  TryCatch try_catch;
  async_call->cb_fn->Call(Context::GetCurrent()->Global(), 3, argv);

  // Clean up.
  async_call->cb_fn.Dispose();
  async_call->cb_fn.Clear();

  free(async_call);
  free(req);

  if (try_catch.HasCaught()) {
    FatalException(try_catch);
  }
}

// Called when the JS function nthPrime is initially called from the JS context.
Handle<Value> AsynCall (const Arguments &args) {
  assert(pthread_self() == node_thread);

  HandleScope scope;

  // Check there are 2 arguments.
  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Requires 2 Arguments")));
    return scope.Close(Undefined());
  }

  // Check argument types are correct.
  if (!args[0]->IsUint32() || !args[1]->IsFunction()) {
    ThrowException(Exception::TypeError(String::New("Requires [Uint32, Function]")));
    return scope.Close(Undefined());
  }

  // Remember the arguments.
  async_call_t* async_call = (async_call_t*)malloc(sizeof(async_call_t));
  async_call->n = args[0]->Uint32Value();
  async_call->cb_fn = Persistent<Function>::New(Local<Function>::Cast(args[1]));

  // Put the C++ work function in the queue to be calculated by the libeio
  // thread pool.
  uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
  req->data = async_call;
  uv_queue_work(uv_default_loop(), req, Work, Finish);

  return scope.Close(True());
}

// Called when require('async_addon') is called in JS. Registers the nthPrime JS
// function and links it with the AyncCall C++ function.
static void Init (Handle<Object> target) {
  node_thread = pthread_self();

  target->Set(String::NewSymbol("nthPrime"),
    FunctionTemplate::New(AsynCall)->GetFunction());
}

// Register the initialization function.
NODE_MODULE(async_addon_1, Init)
