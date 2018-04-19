
#include "DataSourceCMRK29.h"

DataSourceCMRK29::DataSourceCMRK29(OpenStructT * data) {

	log("DataSourceCMRK29 contructor\n");

	card = 0;
	device = 0;
	pcm = NULL;
	outPcm = NULL;
	bad_pcm.fd = -1;
	
	memset(rawBuf,0,sizeof(rawBuf));

	pOpen = data;
}

DataSourceCMRK29::~DataSourceCMRK29() {

	log("DataSourceCMRK29 destructor\n");
}

int DataSourceCMRK29::initDataSource() {
		
/**********************************set hardware*************************************/

	card = CMRK29_CARD;
    device = CMRK29_DEVICE;

    config.channels = CMRK29_CHANNELS;
    config.rate = CMRK29_RATE;
	config.period_size = CMRK29_PERIOD_SIZE;
	config.period_count = CMRK29_PERIOD_COUNT;
    config.format = CMRK29_FORMAT;
    config.start_threshold = CMRK29_START_THRESHOLD;
    config.stop_threshold = CMRK29_STOP_THRESHOLD;
    config.silence_threshold = CMRK29_SILENCE_THRESHOLD;

	pcm = pcm_open(card,device,PCM_IN,&config);

	if(pcm->fd == -1) {
     	pcm_close(pcm);
		pOpen->pcm = &bad_pcm;
 		return -1;
    }

	outPcm = (struct pcm *)calloc(1, sizeof(struct pcm));
	
	outPcm->config = *(struct pcm_config *)(pOpen->config);
	outPcm->buffer_size = ((struct pcm_config *)(pOpen->config))->period_count * ((struct pcm_config *)(pOpen->config))->period_size;
	outPcm->boundary = outPcm->buffer_size;
	outPcm->flags = pOpen->flags;
	outPcm->underruns = 0;
	outPcm->fd = pcm->fd;
	
	pOpen->pcm = (void *)outPcm;

/**********************************************************************************/
	
	return SUCCESS_AUDIO;
	
}

int DataSourceCMRK29::closeDataSource() {

	int ret = 0;
	
	if(NULL != pcm) {
		pcm_close(pcm);
	}

	if(NULL != outPcm) {
		outPcm->prepared = 0;
	    outPcm->running = 0;
	    outPcm->buffer_size = 0;
	    outPcm->fd = -1;
	    free(outPcm);
	}

	return ret;

}

int DataSourceCMRK29::read(char * pMic, unsigned int micDataCount, char * pRes, unsigned int resDataCount) {

	int ret = 0;

	ret = pcm_read(pcm, (void *)rawBuf, sizeof(rawBuf));

	if (0 == ret) {
		deinterleaveAudioData(rawBuf,(short *)pMic,(short *)pRes,CMRK29_CAP_SIZE_48K);
	}

	return ret;
}

int DataSourceCMRK29::deinterleaveAudioData(short *pIn, short *pOutMic, short *pOutSpk, int sampleCnt) {

    int ret = 0; 
    int i;
    short *pTmpOut = NULL;
    short *pTmpSpk = NULL;
    short *pTmpIn = NULL;
    int spkLeft = 0;
    int spkRight = 0;

    do {
    	if ((NULL == pIn) || (NULL == pOutMic) || (NULL == pOutSpk) || (sampleCnt <= 0) ) {
    		ret = -1;
    		break;
    	}
		
    	pTmpOut = pOutMic;
    	pTmpSpk = pOutSpk;
    	pTmpIn = pIn;

    	for (i = 0;i < sampleCnt;i++) {
	    	/*
				 5(5)      0(0)


				4(4)               1(1)


	   			3(3)       2(2)
	    		*/
	    
			*(pTmpOut + sampleCnt * 4 + i)  = *pTmpIn++;
			*(pTmpOut + sampleCnt * 5 + i)  = *pTmpIn++;
			*(pTmpOut + sampleCnt * 2 + i )  = *pTmpIn++;
			*(pTmpOut + sampleCnt * 3 + i)  = *pTmpIn++;
			*(pTmpOut + i)  = *pTmpIn++;
			*(pTmpOut + sampleCnt + i)  = *pTmpIn++;

	   		spkLeft = *pTmpIn++;
	    	spkRight = *pTmpIn++;
			*pTmpSpk++  = (short)((spkLeft + spkRight) >> 1);

    	}
    } while (0);                                                                                                                                                                       
    return ret;
}

