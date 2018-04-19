#include "AudioYSTH5.h"

REGISTER_CLASS(AudioYSTH5);

AudioYSTH5::AudioYSTH5() : className("AudioYSTH5") {

	log("AudioYSTH5 constructor\n");
}

AudioYSTH5::~AudioYSTH5() {

	log("AudioYSTH5 destructor\n");
}

int AudioYSTH5::openDevice(OpenStructT * pOpen) {

	log("AudioYSTH5::open start\n");

	int ret = 0;

	OpenStructT * openData = pOpen;

	if(NULL == openData) {
		return ERROR_AUDIO;
	}

	property_set("sys.mic.enable","0");
		
	setMicNumber(H5_MIC_NUMBER);
	setResNumber(H5_RES_NUMBER);
	setApmLarrayFactor(H5_APM_LARRAY_FACRORY);
	setApmCarrayFactory(H5_APM_CARRAY_FACTORY);

	setalgoPerBufSize(H5_ALOG_PER_BUF_SIZE);
	setalgoBufNumber(H5_ALOG_BUF_NUMBER);
	
	/************************datasource operate*************************************/

	DataSourceYSTH5 *dataSource = new DataSourceYSTH5(openData);

	dataSource->setMicNumber(H5_MIC_NUMBER);
	dataSource->setResNumber(H5_RES_NUMBER);
	
	dataSource->setMicSampleFre(H5_SAMPLE_FRE);
	dataSource->setResSampleFre(H5_SAMPLE_FRE);
	
	dataSource->setMicPerBufSize(H5_MIC_PER_BUF_SZIE);
	dataSource->setResPerBufSize(H5_RES_PER_BUF_SIZE);

	dataSource->setMicBufNumber(H5_MIC_BUF_NUMBER);
	dataSource->setResBufNumber(H5_RES_BUF_NUMBER);

	registerDataSource(dataSource);
		
	/********************************************************************************/

	log("AudioYSTH5::open end\n");
	return SUCCESS_AUDIO;

}

int AudioYSTH5::closeDevice(CloseStructT * pClose __unused) {

	log("AudioYSTH5::close\n");
	
	sleep(1);
	property_set("sys.mic.enable","1");
	
	return 0;
}

