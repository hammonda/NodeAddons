#include <stdint.h>
#include <stddef.h>

#include <node.h>
#include <uv.h>
#include <v8.h>

// Container holding the function call context. This is passed along
// the libuv event loop so each stage knows what its doing.
typedef struct {
  uint32_t start; // call argument
  int32_t size; // call argument
  uint32_t *results; // return values
  v8::Persistent<v8::Function> cb_fn; // callback function
} baton_t;

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
  baton_t* baton = static_cast<baton_t*>(req->data);
  // Calculate the primes.
  if (baton->size < 1) {
    baton->results = NULL;
  } else {
    baton->results = new uint32_t[baton->size];
    for (int32_t i = 0; i < baton->size; ++i) {
      baton->results[i] = nthPrime(baton->start + i);
    }
  }
}

// Called by the libuv event loop when the Work function is done. This allows
// the code to clean up any memory allocated.
void Finish(uv_work_t *req, int32_t status) {
  v8::HandleScope scope;

  baton_t* baton = static_cast<baton_t*>(req->data);

  // Create the arguments used to call the callback function.
  v8::Local<v8::Object> returnObj = v8::Object::New();
  v8::Local<v8::Array> results;
  if (baton->size < 0) {
    results = v8::Array::New(0);
  } else {
    results = v8::Array::New(baton->size);
    for (int32_t i = 0; i < baton->size; ++i) {
      results->Set(i, v8::Integer::New(baton->results[i]));
    }
  }
  returnObj->Set(v8::String::New("start"), v8::Integer::New(baton->start));
  returnObj->Set(v8::String::New("size"), v8::Integer::New(baton->size));
  returnObj->Set(v8::String::New("primes"), results);

  // Invoke the registered callback function.
  v8::TryCatch try_catch;
  v8::Local<v8::Value> argv[1] = { returnObj };
  baton->cb_fn->Call(v8::Context::GetCurrent()->Global(), 1, argv);

  // Clean up.
  baton->cb_fn.Dispose();
  baton->cb_fn.Clear();

  if (baton->size > 0) {
    delete baton->results;
  }
  delete baton;
  delete req;

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
}

// Called when the JS function nthPrime is initially called from the JS context.
v8::Handle<v8::Value> AsynCall (const v8::Arguments &args) {
  v8::HandleScope scope;

  // Check there are 2 arguments.
  if (args.Length() < 3) {
    v8::ThrowException(
      v8::Exception::TypeError(v8::String::New("Requires 3 Arguments")));
    return scope.Close(v8::Undefined());
  }

  // Check argument types are correct.
  if (!args[0]->IsUint32() || !args[1]->IsUint32() || !args[2]->IsFunction()) {
    v8::ThrowException(v8::Exception::TypeError(v8::String::New
      ("Requires [Uint32, Uint32, Function]")));
    return scope.Close(v8::Undefined());
  }

  // Remember the arguments.
  baton_t* baton = new baton_t;
  baton->start = args[0]->Uint32Value();
  baton->size = 1 + static_cast<int32_t>(args[1]->Uint32Value()) - baton->start;
  baton->cb_fn =
    v8::Persistent<v8::Function>::New(v8::Local<v8::Function>::Cast(args[2]));

  // Put the C++ work function in the queue to be calculated by the libeio
  // thread pool.
  uv_work_t* req = new uv_work_t;
  req->data = static_cast<void*>(baton);
  uv_queue_work(uv_default_loop(), req, Work, Finish);

  return scope.Close(v8::Undefined());
}

// Called when require('function_addon_1') is called in JS. Registers the nthPrime JS
// function and links it with the AyncCall C++ function.
static void Init (v8::Handle<v8::Object> target) {
  target->Set(v8::String::NewSymbol("getPrimes"),
    v8::FunctionTemplate::New(AsynCall)->GetFunction());
}

// Register the initialization function.
NODE_MODULE(function_addon_1, Init)
