#define BUILDING_NODE_EXTENSION
#include <node.h>

#include "NthPrime.h"

void InitAll(v8::Handle<v8::Object> target) {
  NthPrime::Init(target);
}

NODE_MODULE(async_addon_2, InitAll)
