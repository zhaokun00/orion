#ifndef AUDDEV_PRIV_WRAPPER_API_H____
#define AUDDEV_PRIV_WRAPPER_API_H____

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct OpenStruct {

	void * pcm;
	unsigned int card;
	unsigned int device;
	unsigned int flags;
	void * config;
	int * CallBack;
	
} OpenStructT;

typedef struct ReadStruct {

	void *data;
	unsigned int count;
	
} ReadStructT;

typedef struct CloseStruct {
	
	
} CloseStructT;

int aud_wrapper_open(OpenStructT * pOpen);
int aud_wrapper_read(ReadStructT * pRread);
int aud_wrapper_close(CloseStructT * pClose);

#if defined(__cplusplus)
}
#endif

#endif
