#include <node.h>
#include <nan.h>
#include "./sampleStruct.h"

class Resampler : public Nan::ObjectWrap {
	public:
		static void Init(v8::Handle<v8::Object> exports);

	private:
		Resampler(sample_t sampleData);
		~Resampler();

		static NAN_METHOD(New);
		static NAN_METHOD(Resample);
		static NAN_METHOD(Configure);
		static Nan::Persistent<v8::Function> constructor;

		sample_t sampleData;
};

void configure(sample_t* sampleData, const Nan::FunctionCallbackInfo<v8::Value>& info);
