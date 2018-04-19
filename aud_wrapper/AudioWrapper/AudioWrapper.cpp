#include "AudioWrapper.h"
#include "AudioYSTH5.h"

AudioWrapper::AudioWrapper() {

	log("AudioWrapper constructor\n");

	audioBase = NULL;
}

AudioWrapper::~AudioWrapper() {

	log("AudioWrapper destructor \n");
}

int AudioWrapper::audin_wrapper_open(OpenStructT * pOpen __unused) {
	
	log("AudioWrapper::audin_wrapper_open\n");

	int ret = 0;

	do {
		audioBase = static_cast<AudioBase *>(ObjectFactory::CreateObject(PLATFORM));

		if(NULL == audioBase) {
			ret = -1;
			break;
		}

		ret = audioBase->open(pOpen);
	} while(0);
	

	return ret;
}

int AudioWrapper::audin_wrapper_read(ReadStructT * pRead __unused) {

	int ret = 0;
	
	do {
		if(NULL == audioBase) {
			ret = -1;
			break;
		}
		ret = audioBase->read(pRead);
	} while(0);

	return ret;
}

int AudioWrapper::audin_wrapper_close(CloseStructT * pClose __unused) {

	log("AudioWrapper::audin_wrapper_close\n");

	int ret = 0;

	do {
		if(NULL == audioBase) {
			ret = -1;
			break;
		}

		ret = audioBase->close(pClose);

		delete audioBase;
		audioBase = NULL;
	} while(0);
	
	return ret; 
}


