#include "AudioBase.h"

//bool AudioBase::dumpFileFlag = false;

AudioBase::AudioBase() {

	log("AudioBase constructor\n");

	dataSourceObj = NULL;
	
	audioAlgoObj = new AudioAlgo();

	audioIpcObj = new AudioIpc(audioAlgoObj);
}

AudioBase::~AudioBase() {
	log("AudioBase destructor\n");
}

int AudioBase::open(OpenStructT * pOpen __unused) {

	int ret = 0;
	log("AudioBase::open\n");

	do {

		openDevice(pOpen);
		
		if(NULL == audioAlgoObj) {
			ret = -1;
			break;
		}
		
		if(NULL == dataSourceObj) {
			ret = -1;
			break;
		}

		if(NULL == audioIpcObj) {
			ret = -1;
			break;		
		}

		audioAlgoObj->init();
		dataSourceObj->init();
		audioIpcObj->init();

		dataSourceObj->start();
		audioAlgoObj->start();	
		audioIpcObj->start();
		
	} while(0);

	
	return ret;
}

int AudioBase::read(ReadStructT * pRead) {

	int ret = 0;
	int sleepCount = 0;
	
	ReadStructT * pData = (ReadStructT *)pRead;


	if((NULL == pData) || (NULL == audioAlgoObj)) {
		ret = -1;
		return ret;
	}
	
	while (1) {

	   ret = audioAlgoObj->getAlgoData((char *)pData->data, pData->count);
	   
	   if(0 == ret) {
	       break;
	   }

	   if (sleepCount >= 10) {
            memset((char *)pData->data, 0xAA, pData->count);
			log("reset data to 0xAA\n");
            break;
        }

        if (ret == -ERR_NOTENOUGHDATA) {
            sleepCount ++;
			usleep(10000);
        }
  
	}

	return ret;
}


int AudioBase::close(CloseStructT * pClose __unused) {

	int ret = 0;

	do {
		
		if(NULL != audioIpcObj) {
			log("close audioIpcObj");
			audioIpcObj->release();
		}
			
		if(NULL != audioAlgoObj) {
			log("close audioAlgoObj");
			audioAlgoObj->close();
		}

		if(NULL != dataSourceObj) {
			log("close dataSourceObj");
			dataSourceObj->close();
		}
				
		if(NULL != audioIpcObj) {
			delete audioIpcObj;
			audioIpcObj = NULL;
		}
		
		if(NULL != audioAlgoObj) {
			log("delete audioAlgoObj");
			delete audioAlgoObj;
			audioAlgoObj = NULL;
		}

		if(NULL != dataSourceObj) {
			log("delete audioAlgoObj");
			delete dataSourceObj;
			dataSourceObj = NULL;
		}

		ret = closeDevice(pClose);
		
	} while(0);
	
	return ret;
}

int AudioBase::registerDataSource(DataSource * source) {

	int ret = 0;
	
	if((NULL == source) || (NULL == audioAlgoObj)) {
		ret = -1;
		return ret;
	}

	dataSourceObj = source;
	
	audioAlgoObj->registerDataSource(source);
	
	return ret;
}

void AudioBase::setMicNumber(unsigned int number) {

	if(NULL != audioAlgoObj) {
		audioAlgoObj->setMicNumber(number);
	}
}

void AudioBase::setResNumber(unsigned int number) {

	if(NULL != audioAlgoObj) {
		audioAlgoObj->setResNumber(number);
	}
}

void AudioBase::setApmLarrayFactor(float factory) {

	if(NULL != audioAlgoObj) {
		audioAlgoObj->setApmLarrayFactor(factory);
	}

}

void AudioBase::setApmCarrayFactory(float factory) {

	if(NULL != audioAlgoObj) {
		audioAlgoObj->setApmCarrayFactory(factory);
	}
}

void AudioBase::setalgoPerBufSize(unsigned int size) {

	if(NULL != audioAlgoObj) {
		audioAlgoObj->setalgoPerBufSize(size);
	}

}

void AudioBase::setalgoBufNumber(unsigned int number) {

	if(NULL != audioAlgoObj) {
		audioAlgoObj->setalgoBufNumber(number);
	}

}

