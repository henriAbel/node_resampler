#include <stdio.h>
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

Nan::Persistent<Function> Resampler::constructor;

void configure(sample_t* sampleData, const Nan::FunctionCallbackInfo<v8::Value>& info) {
	sampleData->sourceRate = info[0]->IsUndefined() ? 44100 : info[0]->NumberValue();
	sampleData->targetRate = info[1]->IsUndefined() ? 22050 : info[1]->NumberValue();
	bool stereo = info[2]->IsUndefined() ? Nan::True()->BooleanValue() : info[2]->BooleanValue();
	sampleData->ch_layout = stereo ? AV_CH_LAYOUT_STEREO : AV_CH_LAYOUT_MONO;
	av_opt_set_int(sampleData->swr_ctx, "in_sample_rate",      sampleData->sourceRate, 0);
	av_opt_set_int(sampleData->swr_ctx, "out_sample_rate",     sampleData->targetRate, 0);
	av_opt_set_int(sampleData->swr_ctx, "out_channel_layout",  sampleData->ch_layout, 0);
	av_opt_set_int(sampleData->swr_ctx, "in_channel_layout",   sampleData->ch_layout, 0);
}

Resampler::Resampler(sample_t sampleData) :
	sampleData(sampleData) {};

Resampler::~Resampler() {
	swr_free(&sampleData.swr_ctx);
};

void Resampler::Init(Handle<Object> exports) {
	Nan::HandleScope scope;
	// Prepare constructor template
	Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("Resampler").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(2);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "resample", Resample);
	Nan::SetPrototypeMethod(tpl, "configure", Configure);

	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Resampler").ToLocalChecked(), tpl->GetFunction());
}

NAN_METHOD(Resampler::New) {
	Nan::HandleScope scope;
	if (info.IsConstructCall()) {
		// Invoked as constructor: `new MyObject(...)`
		sample_t sampleData;
		sampleData.swr_ctx = swr_alloc();
		configure(&sampleData, info);
		if (!sampleData.swr_ctx) {
			fprintf(stderr, "Could not allocate resampler context\n");
			exit(1);
		}

		// TODO make configurable
		sampleData.src_sample_fmt = AV_SAMPLE_FMT_S16;
		sampleData.dst_sample_fmt = AV_SAMPLE_FMT_S16;

		av_opt_set_sample_fmt(sampleData.swr_ctx, "in_sample_fmt", sampleData.src_sample_fmt, 0);
		av_opt_set_sample_fmt(sampleData.swr_ctx, "out_sample_fmt", sampleData.dst_sample_fmt, 0);
		swr_init(sampleData.swr_ctx);

		Resampler* obj = new Resampler(sampleData);
		obj->Wrap(info.This());
		info.GetReturnValue().Set(info.This());
	} else {
		// Invoked as plain function `MyObject(...)`, turn into construct call.
		Local<Function> cons = Nan::New<Function>(constructor);
		info.GetReturnValue().Set(cons->NewInstance());
	}
}

NAN_METHOD(Resampler::Resample) {
	Nan::HandleScope scope;
	Resampler* obj = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());

	Local<Object> bufferObj	= info[0]->ToObject();
	char* bufferData = node::Buffer::Data(bufferObj);
	size_t bufferLength = node::Buffer::Length(bufferObj);
	Nan::Callback* callback = new Nan::Callback(info[1].As<Function>());
	Nan::AsyncQueueWorker(new ResampleWorker(callback, bufferData, bufferLength, &obj->sampleData));
	return;
}

NAN_METHOD(Resampler::Configure) {
	Nan::HandleScope scope;
	Resampler* obj = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
	configure(&obj->sampleData, info);
	swr_init(obj->sampleData.swr_ctx);
	return;
}
