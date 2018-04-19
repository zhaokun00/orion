#include "AudioCMRK29.h"

REGISTER_CLASS(AudioCMRK29);

AudioCMRK29::AudioCMRK29() : className("AudioCMRK29") {

	log("AudioCMRK29 constructor\n");
}

AudioCMRK29::~AudioCMRK29() {

	log("AudioCMRK29 destructor\n");
}

int AudioCMRK29::openDevice(OpenStructT * pOpen) {

	log("AudioCMRK29::open start\n");

	int ret = 0;

	OpenStructT * openData = pOpen;

	if(NULL == openData) {
		return ERROR_AUDIO;
	}
		
	setMicNumber(CMRK29_MIC_NUMBER);
	setResNumber(CMRK29_RES_NUMBER);
	setApmLarrayFactor(CMRK29_APM_LARRAY_FACRORY);
	setApmCarrayFactory(CMRK29_APM_CARRAY_FACTORY);

	setalgoPerBufSize(CMRK29_ALOG_PER_BUF_SIZE);
	setalgoBufNumber(CMRK29_ALOG_BUF_NUMBER);
	
	/************************datasource operate*************************************/

	DataSourceCMRK29 *dataSource = new DataSourceCMRK29(openData);

	dataSource->setMicNumber(CMRK29_MIC_NUMBER);
	dataSource->setResNumber(CMRK29_RES_NUMBER);
	
	dataSource->setMicSampleFre(CMRK29_SAMPLE_FRE);
	dataSource->setResSampleFre(CMRK29_SAMPLE_FRE);
	
	dataSource->setMicPerBufSize(CMRK29_MIC_PER_BUF_SZIE);
	dataSource->setResPerBufSize(CMRK29_RES_PER_BUF_SIZE);

	dataSource->setMicBufNumber(CMRK29_MIC_BUF_NUMBER);
	dataSource->setResBufNumber(CMRK29_RES_BUF_NUMBER);

	registerDataSource(dataSource);
		
	/********************************************************************************/

	log("AudioCMRK29::open end\n");
	return SUCCESS_AUDIO;

}

int AudioCMRK29::closeDevice(CloseStructT * pClose __unused) {

	log("AudioCMRK29::close\n");
	
	
	return 0;
}

