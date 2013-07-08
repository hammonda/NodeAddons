#include <pthread.h>
#include <stdlib.h>

#include "NthPrime.h"

NthPrime::NthPrime () {
}

NthPrime::~NthPrime () {
}

void NthPrime::Init(v8::Handle<v8::Object> target) {
  // Prepare constructor function template.
  v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(NthPrime::New);
  tpl->SetClassName(v8::String::NewSymbol("NthPrime"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  // Add class methods to constructor function prototype
  tpl->PrototypeTemplate()->Set(v8::String::NewSymbol("getNthPrime"),
        v8::FunctionTemplate::New(NthPrime::getNthPrime_JS)->GetFunction());

  // Add the constructor to the module exports.
  target->Set(v8::String::NewSymbol("NthPrime"),
      v8::Persistent<v8::Function>::New(tpl->GetFunction()));
}

v8::Handle<v8::Value> NthPrime::New(const v8::Arguments& args) {
  v8::HandleScope scope;

  // Create a new NthPrime class and associate it with a JS Object.
  NthPrime* obj = new NthPrime();
  obj->Wrap(args.This());

  return args.This();
}

uint32_t NthPrime::getNthPrime (uint32_t n) {
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

v8::Handle<v8::Value> NthPrime::getNthPrime_JS (const v8::Arguments &args) {
  v8::HandleScope scope;

  // Check there are 2 arguments.
  if (args.Length() < 2) {
    v8::ThrowException(v8::Exception::TypeError(
        v8::String::New("Requires 2 Arguments")));
    return scope.Close(v8::Undefined());
  }

  // Check argument types are correct.
  if (!args[0]->IsUint32() || !args[1]->IsFunction()) {
    v8::ThrowException(v8::Exception::TypeError(
        v8::String::New("Requires [Uint32, Function]")));
    return scope.Close(v8::Undefined());
  }

  // Remember the arguments.
  getNthPrime_t* baton = (getNthPrime_t*) malloc(sizeof(getNthPrime_t));
  baton->__class = ObjectWrap::Unwrap<NthPrime>(args.This());
  baton->n = args[0]->Uint32Value();
  baton->cb_fn = v8::Persistent<v8::Function>::New(
      v8::Local<v8::Function>::Cast(args[1]));

  // Put the C++ work function in the queue to be calculated by the libeio
  // thread pool.
  uv_work_t* req = (uv_work_t*) malloc(sizeof(uv_work_t));
  req->data = baton;
  uv_queue_work(uv_default_loop(), req, getNthPrime_start, getNthPrime_finish);

  return scope.Close(v8::True());
}

void NthPrime::getNthPrime_start (uv_work_t *req) {
  getNthPrime_t* baton = (getNthPrime_t*) req->data;
  // Call then class method.
  int32_t n = baton->n;
  if (n < 0) {
    baton->nthPrime = 0;
  } else {
    baton->nthPrime = (baton->__class)->getNthPrime(n);
  }
  baton->thread_id = pthread_self();
}

void NthPrime::getNthPrime_finish (uv_work_t *req, int32_t status) {
  getNthPrime_t* baton = (getNthPrime_t*)req->data;
  v8::HandleScope scope;

  // Create the arguments used to call the callback function.
  v8::Local<v8::Value> argv[3];
  argv[0] = v8::Local<v8::Value>::New(v8::Uint32::New(baton->n));
  argv[1] = v8::Local<v8::Value>::New(v8::Uint32::New(baton->nthPrime));
  argv[2] = v8::Local<v8::Value>::New(v8::Number::New(baton->thread_id));

  // Invoke the registered callback function.
  v8::TryCatch try_catch;
  baton->cb_fn->Call(v8::Context::GetCurrent()->Global(), 3, argv);

  // Clean up.
  baton->cb_fn.Dispose();
  baton->cb_fn.Clear();

  free(baton);
  free(req);

  if (try_catch.HasCaught()) {
    node::FatalException(try_catch);
  }
}
