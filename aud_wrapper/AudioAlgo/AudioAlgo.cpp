#include "AudioAlgo.h"
#include "AudioBase.h"

AudioAlgo::AudioAlgo() {

	log("AudioAlgo construct start\n");
	
	micNumber = 0;
	resNumber = 0;
	apmLarrayFactor = 0.0f;
	apmCarrayFactory = 0.0f;

	micBuf = NULL;
	resBuf = NULL;
	wakeUpBuf = NULL;
	reconBuf = NULL;
	
	dataSource = NULL;
	algoDump = new AlgoDump();;

	algoReady = false;
	apmHandle = NULL;		
	algoThread = NULL;
	algoReadyCx = NULL;

	log("AudioAlgo construct end\n");
}

AudioAlgo::~AudioAlgo() {
	log("AudioAlgo deconstruct\n");
}

bool algoHandlerEvent(void * ptr) {

	bool bRet = true;

	AudioAlgo * algoObj = (AudioAlgo *)ptr;
	
	if(NULL == algoObj) {
		usleep(100);
		return bRet;
	}

	DataSource * dataSourceObj = (DataSource *)algoObj->dataSource;

	if(NULL == dataSourceObj) {
		usleep(100);
		return bRet;
	}
	
	if(false == algoObj->isAlgoReady()) {
		usleep(100);
		return bRet;
	}

	AlgoDump * dumpObj = algoObj->getAlgoDump();
	if(NULL == dumpObj) {
		usleep(100);
		return bRet;
	}
	int result = 0;
	unsigned int micDataLen = algoObj->micNumber * ALGO_CAP_SIZE_16K * sizeof(short);
	unsigned int resDataLen = algoObj->resNumber * ALGO_CAP_SIZE_16K * sizeof(short);
	
	do{
		memset(algoObj->micBuf, 0, micDataLen);
		memset(algoObj->resBuf, 0, resDataLen);

		result = dataSourceObj->getMicData((char *)algoObj->micBuf, micDataLen);
		if(0 != result) {
			usleep(100);
			break;
		}

		result = dataSourceObj->getResData((char *)algoObj->resBuf, resDataLen);

		dumpObj->writeMicAndRes((char *)algoObj->micBuf, micDataLen, (char *)algoObj->resBuf, resDataLen,AlgoDump::before);

		mapm_pushref(algoObj->apmHandle, (char *)algoObj->resBuf, resDataLen);
	    mapm_process(algoObj->apmHandle, (char *)algoObj->micBuf, algoObj->wakeUpBuf, algoObj->reconBuf, AUDDEV_ONE_FRAME_SIZE_1CH);
	
		dumpObj->writeMicAndRes((char *)algoObj->micBuf, micDataLen, (char *)algoObj->resBuf, resDataLen,AlgoDump::after);
		dumpObj->writeWakeAndRec((char *)algoObj->wakeUpBuf, AUDDEV_ONE_FRAME_SIZE_1CH, (char *)algoObj->reconBuf,AUDDEV_ONE_FRAME_SIZE_1CH);
		
		algoObj->interleaveAudioData(algoObj->reconBuf, algoObj->wakeUpBuf, algoObj->outBuf, ALGO_CAP_SIZE_16K);
		
		result = writeMediaRingBuf(&algoObj->ringBufAlgo, (char *)algoObj->outBuf, AUDDEV_ONE_FRAME_SIZE_2CH);
		
		if(result != 0) {
			log("==================writeMediaRingBuf failed======================");
		}
		
	}while(0);
	
	return bRet;
}


int AudioAlgo::init() {

	int ret = 0;

	do {		
		ret = initMediaRingBuf(&ringBufAlgo, algoPerBufSize * algoBufNumber, algoPerBufSize);		
		if(0 != ret) {	
			log("initMediaRingBuf ringBufAlog failed\n");	
			ret = -1;
			break;
		}

		micBuf = new short[micNumber * ALGO_CAP_SIZE_16K];
		resBuf = new short[resNumber * ALGO_CAP_SIZE_16K];
		wakeUpBuf = new short[ALGO_CAP_SIZE_16K];
		reconBuf = new short[ALGO_CAP_SIZE_16K];
		outBuf = new short[ALGO_CAP_SIZE_16K * 2];

		algoDump->open();
		
		//alog thread init
		algoReady = false;
		algoReadyCx = CriticalSectionPosix::Create();
		algoThread = ThreadPosix::Create(algoHandlerEvent, this, kRealtimePriority, "alog Thread");
		
		//alog init
		apmHandle = mapm_init(APM_RATE_16K, micNumber, apmLarrayFactor, apmCarrayFactory, resNumber);

		if(NULL == apmHandle) {
			return ERROR_AUDIO;
		}

		mapm_setparam(apmHandle, APM_PROCESS_ALL_ENABLE);
		mapm_reset(apmHandle);
		
		#if ENABLE_LOCATION
			mapm_set_flag(apmHandle, LOCATION);
		#endif

	}while(0);

	return ret;
}

int AudioAlgo::start() {

	int ret = 0;	
	Rtip_UWord32 alogId = 0;

	do {

		if(NULL != algoReadyCx) {
			setAlgoReady(true);
		}	

		if(NULL == algoThread) {
			ret = -ERROR_AUDIO;
			break;
		}
		
		algoThread->Start(alogId);
	} while(0);
	
	return ret;
}

int AudioAlgo::close() {

	int ret = 0;

	
	setAlgoReady(false);

	if(NULL != algoThread) {
		algoThread->Stop();
	}

	usleep(100);

	if(NULL != algoThread) {	
		delete algoThread;
		algoThread = NULL;
	}

	if(NULL != algoReadyCx) {
		delete algoReadyCx;
		algoReadyCx = NULL;
	}

	releaseMediaRingBuf(&ringBufAlgo);
	
	if(NULL != apmHandle) {
		mapm_exit(apmHandle);
		apmHandle = NULL;
	}
	
	if(NULL != micBuf) {
		delete []micBuf;
		micBuf = NULL;
	}

	if(NULL != resBuf) {
		delete []resBuf;
		resBuf = NULL;
	}

	if(NULL != wakeUpBuf) {
		delete []wakeUpBuf;
		wakeUpBuf = NULL;
	}

	if(NULL != reconBuf) {
		delete []reconBuf;
		reconBuf = NULL;
	}

	if(NULL != outBuf) {
		delete []outBuf;
		outBuf = NULL;
	}

	if(NULL != algoDump) {
		algoDump->close();
		delete algoDump;
		algoDump = NULL;
	}
		
	return ret;
}

int AudioAlgo::getAlgoData(char * pData, unsigned int count) {

	int ret = 0;

	do {
		if(NULL == pData) {
			ret = -1;
			break;
		}
		ret = readMediaRingBuf(&ringBufAlgo, pData, count);
	}while(0);

	return ret;
}

void * AudioAlgo::getAlgoHandle() {

	return apmHandle;
}

AlgoDump * AudioAlgo::getAlgoDump() {

	return algoDump;
}

int AudioAlgo::registerDataSource(DataSource * source) {

	int ret = 0;

	dataSource = source;
	return ret;
}

bool AudioAlgo::isAlgoReady() {

	CriticalSectionScoped critScoped(algoReadyCx);

	return algoReady;
}

void AudioAlgo::setMicNumber(unsigned int number) {

	micNumber = number;
	
	if(NULL != algoDump) {
		algoDump->setMicFileNumber(number);
	}
	
}

void AudioAlgo::setResNumber(unsigned int number) {

	resNumber = number;

	if(NULL != algoDump) {
		algoDump->setResFileNumebr(number);
	}
}

void AudioAlgo::setApmLarrayFactor(float factory) {

	apmLarrayFactor = factory;
}

void AudioAlgo::setApmCarrayFactory(float factory) {

	apmCarrayFactory = factory;
}

void AudioAlgo::setAlgoReady(bool val) {

	CriticalSectionScoped critScoped(algoReadyCx);
	algoReady = val;
}

void AudioAlgo::setalgoBufNumber(unsigned int number) {

	algoBufNumber = number;
}

void AudioAlgo::setalgoPerBufSize(unsigned int size) {

	algoPerBufSize = size;
}

int AudioAlgo::mapm_compute(struct objWakeupTimeInfo* wakeupTimeInfo, float *wakupLoca_buff) {

	int ret = 0;

	do {
		if(NULL == apmHandle) {
			ret = -1;
			break;
		}

		ret = mapm_compute_wakLoca(apmHandle,wakeupTimeInfo,wakupLoca_buff);
	} while(0);	

	return ret;
}


int AudioAlgo::interleaveAudioData(short * pIn01, short * pIn02, short * pOut, int sampleCnt) {

	int ret;
	int index;
	short *pL = NULL;
	short *pR= NULL;
	short *pO = NULL;

	do{
		if((pIn01 == NULL) || (pIn02 == NULL) || (pOut == NULL)) {
			ret = -1;
			break;
		}

		pL = pIn01;
		pR = pIn02;
		pO = pOut;

		for (index = 0; index < sampleCnt; index++) {
			*pO ++ = *pL ++;
			*pO ++ = *pR ++;
		}
	} while (0);

	return ret;

}

