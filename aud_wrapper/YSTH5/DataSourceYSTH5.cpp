
#include "DataSourceYSTH5.h"

DataSourceYSTH5::DataSourceYSTH5(OpenStructT * data) {

	log("DataSourceYSTH5 contructor\n");

	card = 0;
	device = 0;
	pcm = NULL;
	bad_pcm.fd = -1;

	playBack = NULL;
	
	memset(rawMicBuf,0,sizeof(rawMicBuf));
	memset(rawResBuf,0,sizeof(rawResBuf));

	pOpen = data;
}

DataSourceYSTH5::~DataSourceYSTH5() {

	log("DataSourceYSTH5 destructor\n");
}

int DataSourceYSTH5::initDataSource() {
		
	playBack = (T_CPLAYBACK)pOpen->CallBack;

	if(NULL != playBack) {
		playBack(NULL,0,H5_START_RECEIVE_PCM_DATA);
	}
	else {
		return -1;
	}

/**********************************set hardware*************************************/
	
	card = H5_CARD;
    device = H5_DEVICE;

    config.channels = H5_CHANNELS;
    config.rate = H5_RATE;
	config.period_size = H5_PERIOD_SIZE;
	config.period_count = H5_PERIOD_COUNT;
    config.format = H5_FORMAT;
    config.start_threshold = H5_START_THRESHOLD;
    config.stop_threshold = H5_STOP_THRESHOLD;
    config.silence_threshold = H5_SILENCE_THRESHOLD;

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

int DataSourceYSTH5::closeDataSource() {

	int ret = 0;
	
	if(NULL != pcm) {
		pcm_close(pcm);
	}

	if(NULL != playBack) {
		playBack(NULL,0,H5_STOP_RECEIVE_PCM_DATA);
	}

	return ret;

}

int DataSourceYSTH5::read(char * pMic, unsigned int micDataCount, char * pRes, unsigned int resDataCount) {

	int ret = 0;

	ret = pcm_read_ex(pcm, (void *)rawMicBuf, micDataCount);

	if (0 == ret) {
		deinterleaveMicData(rawMicBuf,(short *)pMic,H5_CAP_SIZE_48K);

		if(NULL != playBack) {
			playBack((char *)rawResBuf,resDataCount * 2,H5_RECEIVE_PCM_DATA); //get hdmi data
			deinterleaveResData(rawResBuf, (short *)pRes, H5_CAP_SIZE_48K);
		}
	}

	return ret;
}

int DataSourceYSTH5::deinterleaveMicData(short * pInData, short * pOutMic, int sampleCnt) {

	int ret = 0;
    int index = 0;
    short *pOut = pOutMic;
    short *pIn = pInData;

    do {
        if((NULL == pIn) || (NULL == pOutMic) || (sampleCnt <= 0) ) {
            ret = -1;
            break;
        }

		for (index = 0; index < sampleCnt; index ++) {
			*(pOut + index)  = *pIn++;
			*(pOut + sampleCnt + index)  = *pIn++;
			*(pOut + sampleCnt*2 + index)  = *pIn++;
            *(pOut + sampleCnt*3 + index)	= *pIn++;
        }

    } while (0);
		
    return ret;
}

int DataSourceYSTH5::deinterleaveResData(short * pInData, short * pOutSpk, int sampleCnt) {

	int ret = 0;
	int index = 0;
	short *pIn = pInData;
	short *pOut = pOutSpk;

	do {
		if ((NULL == pIn) || (NULL == pOut) || (sampleCnt <= 0))
		{
			ret = -1;
			break;
		}

		for (index = 0; index < sampleCnt; index++) {
			*(pOut + index) = (*pIn + *(pIn+1)) / 2;
			pIn = pIn + 2;
		}
		
	} while(0);

	return ret;
}
