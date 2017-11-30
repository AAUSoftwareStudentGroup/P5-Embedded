#ifndef __TEST__
#define __TEST__

#if defined TEST && defined DEBUG
	#define ENCOG_MODEL_FILE_PATH "/test"
#endif

enum ReceiverMode {
	READING_DATA_POINT,
	READY
};

void test();

#endif