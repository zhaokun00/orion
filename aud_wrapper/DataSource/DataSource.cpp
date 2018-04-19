#include "DataSource.h"

DataSource::DataSource() {

	pMicDataBuf = NULL;
	pResDataBuf = NULL;
	pMicTmpDataBuf = NULL;
	pResTmpDataBuf = NULL;

	micSampleFre = 0;	
	resSampleFre = 0;
	micResampleFre = SALMPLE_FEQ_ALGO;
	resResampleFre = SALMPLE_FEQ_ALGO;

	micDataBufLen = 0;
	resDataBufLen = 0;
	micTmpDataBufLen = 0;
	resTmpDataBufLen = 0;
			
	micPerBufSize = 0;
	resPerBufSize = 0;
			
	micBufNumber = 0;
	resBufNumber = 0;

	pResampleMic = NULL;
	pResampleRes = NULL;

	capReady = false;
	capThread = NULL;
	capReadyCx = NULL;
	
}

DataSource::~DataSource() {
	log("DataSource destructor\n");
}

int DataSource::start() {

	int ret = 0;
	
	setCapReady(true);
	
	Rtip_UWord32 capId = 0;
	capThread->Start(capId);

	return ret;
}

int DataSource::close() {

	int ret = 0;

	if(NULL == capReadyCx) {
		setCapReady(false);
	}
	
	if(NULL != capThread) {
		capThread->Stop();
	}
	
	usleep(1000);
	
	if(NULL != capThread) {	
		delete capThread;
		capThread = NULL;
	}

	if(NULL != capReadyCx) {
		delete capReadyCx;
		capReadyCx = NULL;
	}
	
	releaseMediaRingBuf(&ringBufMic);
	releaseMediaRingBuf(&ringBufRes);
	
	if(NULL != pResampleMic) {
		resample_exit(pResampleMic);
		pResampleMic = NULL;
	}

	if(NULL != pResampleRes) {
		resample_exit(pResampleRes);
		pResampleRes = NULL;
	}
	
	if(NULL != pMicDataBuf) {
		delete pMicDataBuf;
		pMicDataBuf = NULL;
	}

	if(NULL != pResDataBuf) {
		delete pResDataBuf;
		pResDataBuf = NULL;
	}

	if(NULL != pMicTmpDataBuf) {
		delete pMicTmpDataBuf;
		pMicTmpDataBuf = NULL;
	}

	if(NULL != pResTmpDataBuf) {
		delete pResTmpDataBuf;
		pResTmpDataBuf = NULL;
	}

	closeDataSource();

	return ret;

}

int DataSource::getMicData(char * pData, unsigned int count) {

	int ret = 0;

	do{
		if(NULL == pData) {
			ret = -1;
			break;
		}
		ret = readMediaRingBuf(&ringBufMic, pData, count);		
	} while(0);

	return ret;	
}

int DataSource::getResData(char * pData, unsigned int count) {

	int ret = 0;

	do{
		if(NULL == pData) {
			ret = -1;
			break;
		}
		ret = readMediaRingBuf(&ringBufRes, pData, count);		
	} while(0);

	return ret;	
}

void DataSource::setMicNumber(unsigned int number) {
	
	micNumber = number;
}

void DataSource::setResNumber(unsigned int number) {

	resNumber = number;
}

bool readHandlerEvent(void *ptr) {

	bool bRet = true;
	int ret = 0;
	
	DataSource * obj = (DataSource *)ptr;

	if(NULL == obj) {	
		usleep(100);
		return bRet;
	}

	do {
		if(obj->isCapReady()) {
			
			memset(obj->pMicDataBuf,0,obj->micDataBufLen * sizeof(short));
			memset(obj->pResDataBuf,0,obj->resDataBufLen * sizeof(short));
			memset(obj->pMicTmpDataBuf,0,obj->micTmpDataBufLen * sizeof(short));
			memset(obj->pResTmpDataBuf,0,obj->resTmpDataBufLen * sizeof(short));

			if (0 == obj->read((char *)obj->pMicDataBuf, obj->micDataBufLen * BYTES, (char *)obj->pResDataBuf, obj->resDataBufLen * BYTES)) {
				
				if(obj->micSampleFre != obj->micResampleFre) {
					ret = resample_process(obj->pResampleMic, obj->pMicDataBuf, obj->pMicTmpDataBuf, obj->micDataBufLen);
					if(ret != 0) {
						log("resample_process mic data failed\n");
						break;
					}
					ret = writeMediaRingBuf(&obj->ringBufMic, (char *)obj->pMicTmpDataBuf, obj->micTmpDataBufLen * BYTES);
				}
				else {
					ret = writeMediaRingBuf(&obj->ringBufMic, (char *)obj->pMicDataBuf, obj->resDataBufLen * BYTES);
				}
				if(ret < 0) {
					log("writeMediaRingBuf ringBufMic failed");
				}

				if(obj->resSampleFre != obj->resResampleFre) {
					ret = resample_process(obj->pResampleRes, obj->pResDataBuf, obj->pResTmpDataBuf, obj->resDataBufLen);					
					if(ret != 0) {
						log("resample_process res failed\n");
						break;
					}
					ret = writeMediaRingBuf(&obj->ringBufRes, (char *)obj->pResTmpDataBuf, obj->resTmpDataBufLen * BYTES);
				}
				else {
					ret = writeMediaRingBuf(&obj->ringBufRes, (char *)obj->pResDataBuf, obj->resTmpDataBufLen);
				}				
				if(ret != 0) {
					log("writeMediaRingBuf ringBufRes failed\n");
				}				
			}
		}
		
	}while(0);
		
	return bRet;
}


int DataSource::init() {

	int ret = 0;

	do {
		ret = initMediaRingBuf(&ringBufMic, micPerBufSize * micBufNumber, micPerBufSize);			
		if(0 != ret) {
			log("initMediaRingBuf ringBufMic failed\n");	
			ret = -1;
			break;
		}
	
		ret = initMediaRingBuf(&ringBufRes, resPerBufSize * resBufNumber, resPerBufSize);		
		if(0 != ret) {	
			log("initMediaRingBuf ringBufSpk failed\n");	
			ret = -1;
			break;
		}

		micDataBufLen = micSampleFre / 1000 * SAMPLE_TIME * micNumber;
		resDataBufLen = resSampleFre / 1000 * SAMPLE_TIME * resNumber;
		micTmpDataBufLen = micResampleFre / 1000 * SAMPLE_TIME * micNumber;
		resTmpDataBufLen = resResampleFre / 1000 * SAMPLE_TIME * resNumber;

		
		pMicDataBuf = new short[micDataBufLen];
		pResDataBuf = new short[resDataBufLen];
		pMicTmpDataBuf = new short[micTmpDataBufLen];
		pResTmpDataBuf = new short[resTmpDataBufLen];

		memset(pMicDataBuf,0,micDataBufLen * sizeof(short));
		memset(pResDataBuf,0,resDataBufLen * sizeof(short));
		memset(pMicTmpDataBuf,0,micTmpDataBufLen * sizeof(short));
		memset(pResTmpDataBuf,0,resTmpDataBufLen * sizeof(short));
		
		if(micSampleFre != micResampleFre) {
			pResampleMic = resample_init(micNumber,micSampleFre,micResampleFre,micSampleFre / 1000 * SAMPLE_TIME);
		}

		if(resSampleFre != resResampleFre) {
			pResampleRes = resample_init(resNumber,resSampleFre,resResampleFre,resSampleFre / 1000 * SAMPLE_TIME);
		}
		
		capReady = false;
		capReadyCx = CriticalSectionPosix::Create();
		capThread = ThreadPosix::Create(readHandlerEvent, this, kRealtimePriority, "wrapper_capThread");

		initDataSource();

	}while(0);
	
	return ret;
	
}

bool DataSource::isCapReady() {

	CriticalSectionScoped critScoped(capReadyCx);

	return capReady;
}

void DataSource::setCapReady(bool val) {

	CriticalSectionScoped critScoped(capReadyCx);
	capReady = val;
}

void DataSource::setMicSampleFre(unsigned int fre) {

	micSampleFre = fre;
}

void DataSource::setResSampleFre(unsigned int fre) {

	resSampleFre = fre;
}

void DataSource::setMicResampleFre(unsigned int fre) {

	micResampleFre = fre;
}

void DataSource::setResResampleFre(unsigned int fre) {

	resResampleFre = fre; 

}

void DataSource::setMicPerBufSize(unsigned int size) {

	micPerBufSize = size;
}

void DataSource::setResPerBufSize(unsigned int size) {

	resPerBufSize = size;
}

void DataSource::setMicBufNumber(unsigned int number) {

	micBufNumber = number;
}

void DataSource::setResBufNumber(unsigned int number) {

	resBufNumber = number;
}

