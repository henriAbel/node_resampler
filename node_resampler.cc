#include <node.h>
#include <nan.h>
#include <v8.h>
#include "resampler.cpp"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;

void init(Handle<Object> exports) {
  Resampler::Init(exports);
}

NODE_MODULE(node_resampler, init)