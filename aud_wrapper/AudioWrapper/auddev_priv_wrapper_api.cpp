
#include "auddev_priv_wrapper_api.h"
#include "AudioWrapper.h"

static AudioWrapper * g_audioWrapper = NULL;

int aud_wrapper_open(OpenStructT * pOpen) {
	
	log("aud_wrapper_open\n");
	if(NULL == g_audioWrapper) {

		g_audioWrapper = new AudioWrapper();

		return g_audioWrapper->audin_wrapper_open(pOpen);
	}

	return ERROR_AUDIO;
}

int aud_wrapper_read(ReadStructT * pRead) {

	if(NULL != g_audioWrapper) {
		
		int ret = g_audioWrapper->audin_wrapper_read(pRead);

		return ret;
	}	

	return ERROR_AUDIO;
}

int aud_wrapper_close(CloseStructT * pClose) {

	log("aud_wrapper_close\n");
	
	if(NULL != g_audioWrapper) {

		g_audioWrapper->audin_wrapper_close(pClose);
		delete g_audioWrapper;

		g_audioWrapper = NULL;

		return SUCCESS_AUDIO;
	}
	
	return ERROR_AUDIO;
}

