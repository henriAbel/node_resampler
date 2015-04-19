#include <node.h>
#include <nan.h>
#include "./sampleStruct.h"

class ResampleWorker : public NanAsyncWorker {
	public:
		ResampleWorker(NanCallback* callback, char* src_data, size_t len, sample_t* sampleData);
		~ResampleWorker();
		void Execute();
		void HandleOKCallback();

	private:
		char* src_data;
		size_t len;
		uint8_t* dst_data;
		sample_t* sampleData;
		int dst_bufsize;		
};