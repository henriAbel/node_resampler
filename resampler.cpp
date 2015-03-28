#include <node.h>
#include <nan.h>
#include <math.h> 
#include "./resampler.h"
#include "./resampleWorker.cpp"
#include <iostream>

using v8::Function;
using v8::Local;
using v8::Value;
using v8::Object;

using namespace v8;

Persistent<Function> Resampler::constructor;

Resampler::Resampler(sample_t sampleData) : 
	sampleData(sampleData) {};

Resampler::~Resampler() {
	swr_free(&sampleData.swr_ctx);
};

void Resampler::Init(Handle<Object> exports) {
	NanScope();
	// Prepare constructor template
	Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
	tpl->SetClassName(NanNew("Resampler"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	NODE_SET_PROTOTYPE_METHOD(tpl, "resample", Resample);

	NanAssignPersistent(constructor, tpl->GetFunction());
	exports->Set(NanNew("Resampler"), tpl->GetFunction());
}

NAN_METHOD(Resampler::New) {
	NanScope();
	if (args.IsConstructCall()) {
		// Invoked as constructor: `new MyObject(...)`
		sample_t sampleData;
		sampleData.sourceRate = args[0]->IsUndefined() ? 44100 : args[0]->NumberValue();
		sampleData.targetRate = args[1]->IsUndefined() ? 22050 : args[1]->NumberValue();
		bool stereo = args[2]->IsUndefined() ? NanTrue()->BooleanValue() : args[2]->BooleanValue();
		sampleData.swr_ctx = swr_alloc();
		if (!sampleData.swr_ctx) {
			fprintf(stderr, "Could not allocate resampler context\n");
			exit(1);
		}

		sampleData.ch_layout = stereo ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;
		// TODO make configurable
		sampleData.src_sample_fmt = AV_SAMPLE_FMT_S16;
		sampleData.dst_sample_fmt = AV_SAMPLE_FMT_S16;

		av_opt_set_int(sampleData.swr_ctx, "in_channel_layout",    sampleData.ch_layout, 0);
		av_opt_set_int(sampleData.swr_ctx, "in_sample_rate",       sampleData.sourceRate, 0);
		av_opt_set_sample_fmt(sampleData.swr_ctx, "in_sample_fmt", sampleData.src_sample_fmt, 0);

		av_opt_set_int(sampleData.swr_ctx, "out_channel_layout",    sampleData.ch_layout, 0);
		av_opt_set_int(sampleData.swr_ctx, "out_sample_rate",       sampleData.targetRate, 0);
		av_opt_set_sample_fmt(sampleData.swr_ctx, "out_sample_fmt", sampleData.dst_sample_fmt, 0);
		swr_init(sampleData.swr_ctx);		
		
		Resampler* obj = new Resampler(sampleData);
		obj->Wrap(args.This());
		NanReturnValue(args.This());
	} else {
		// Invoked as plain function `MyObject(...)`, turn into construct call.
		Local<Function> cons = NanNew<Function>(constructor);
		NanReturnValue(cons->NewInstance());
	}
}

NAN_METHOD(Resampler::Resample) {
	NanScope();

	Resampler* obj = ObjectWrap::Unwrap<Resampler>(args.Holder());
  		
	Local<Object> bufferObj	= args[0]->ToObject();
	char* bufferData = node::Buffer::Data(bufferObj);
	size_t bufferLength = node::Buffer::Length(bufferObj);
	NanCallback *callback = new NanCallback(args[1].As<Function>());
	NanAsyncQueueWorker(new ResampleWorker(callback, bufferData, bufferLength, obj->sampleData));
	NanReturnUndefined();
}


