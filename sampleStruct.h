extern "C" {
#include "libswresample/swresample.h"
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}

#ifndef sample_struct
#define sample_struct
struct sample_t {
	int64_t ch_layout;
	int sourceRate, targetRate;
	enum AVSampleFormat src_sample_fmt, dst_sample_fmt;
	struct SwrContext *swr_ctx;
};

#endif