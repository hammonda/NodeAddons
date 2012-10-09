#ifndef NTHPRIME_H_
#define NTHPRIME_H_

#include <stdint.h>

#include <v8.h>
#include <uv.h>
#include <node.h>

class NthPrime : public node::ObjectWrap {
public:
  static void Init(v8::Handle<v8::Object> target);

  uint32_t getNthPrime (uint32_t n);

private:
  NthPrime ();
  virtual ~NthPrime ();

  static v8::Handle<v8::Value> New(const v8::Arguments& args);

  typedef struct {
    NthPrime *__class;
    uint32_t n;
    uint32_t nthPrime;
    int64_t  thread_id;
    v8::Persistent<v8::Function> cb_fn;
  } getNthPrime_t;

  static v8::Handle<v8::Value> getNthPrime_JS (const v8::Arguments &args);
  static void getNthPrime_start (uv_work_t *req);
  static void getNthPrime_finish (uv_work_t *req);

};

#endif /* NTHPRIME_H_ */
