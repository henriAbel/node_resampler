#include <nan.h>
#include <node.h>
#include "./resampleWorker.h"

using v8::Local;
using v8::Value;
using Nan::HandleScope;
using Nan::Callback;


ResampleWorker::ResampleWorker(Callback* callback, char* src_data, size_t len, sample_t* sampleData)
	: AsyncWorker(callback), src_data(src_data), len(len), sampleData(sampleData) {}
ResampleWorker::~ResampleWorker() {
	//av_free(dst_data);
	dst_data = NULL;
}

void ResampleWorker::Execute() {
	int src_nb_channels, dst_nb_channels, dst_linesize,
		src_nb_samples, dst_nb_samples, ret;

	src_nb_samples = static_cast<int>(ceil(static_cast<int>(len) / (double)4));
		src_nb_channels = av_get_channel_layout_nb_channels(sampleData->ch_layout);
	// TODO
	// Mono -> Stereo if needed
	dst_nb_channels = av_get_channel_layout_nb_channels(sampleData->ch_layout);

	dst_nb_samples = av_rescale_rnd(swr_get_delay(sampleData->swr_ctx, sampleData->sourceRate) +
							src_nb_samples, sampleData->targetRate, sampleData->sourceRate, AV_ROUND_UP);

	ret = av_samples_alloc(&dst_data, NULL, src_nb_channels, dst_nb_samples,
						sampleData->src_sample_fmt, 0);

	if (ret < 0) {
		fprintf(stderr, "Cannot allocate dst data\n");
	}

	ret = swr_convert(sampleData->swr_ctx, &dst_data, dst_nb_samples, (const uint8_t **)&src_data, src_nb_samples);

	if (ret < 0) {
		fprintf(stderr, "Error while converting\n");
	}

	dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
											 ret, sampleData->dst_sample_fmt, 1);
}

void ResampleWorker::HandleOKCallback() {
	HandleScope scope;

	Local<Value> argv[] = {
		Nan::NewBuffer((char*)dst_data, static_cast<int>(dst_bufsize)).ToLocalChecked()
	};

	callback->Call(1, argv);
}
