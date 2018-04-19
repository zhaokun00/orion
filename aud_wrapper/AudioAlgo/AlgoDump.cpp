#include "AlgoDump.h"

bool AlgoDump::enableBeforeDump = false;
bool AlgoDump::enableDump = false;

AlgoDump::AlgoDump() {
	log("AlgoDump construct\n");

	ppFile = NULL;
	micFileNumber = 0;
	resFileNumber = 0;
	sumFileNumber = 0;
						
	enableDump = isDumpFile();
//	enableDump = true;
	enableBeforeDump = isDumpBeforeFile();
//	enableBeforeDump = true;

}

AlgoDump::~AlgoDump() {
	log("AlgoDump deconstruct\n");
}

bool AlgoDump::isDumpFile() {

	int ret = 0;
	bool result = true;
	
	struct stat buf;

	ret = stat(DUMP_PROFILE_NAME_PATH, &buf);

	if(0 != ret) {
		result = false;
	}
	
	return result;
}

bool AlgoDump::isDumpBeforeFile() {

	int ret = 0;
	bool result = true;

	struct stat buf;

	ret = stat(DUMP_BEFORE__PROFILE_NAME_PATH, &buf);

	if(0 != ret) {
		result = false;
	}

	return result;

}

void AlgoDump::saveLastRecFiles() {

	 system("rm -rf /data/local/test/lastSaved");	 	    
	 system("mkdir -p /data/local/test/lastSaved");
	 system("cp /data/local/test/*.pcm /data/local/test/lastSaved");

}

int AlgoDump::open() {

	int ret = 0;

	do {

		saveLastRecFiles();
		registerSignal();
		sumFileNumber = algoFileNumber + resFileNumber + micFileNumber;
		
		if(true == enableBeforeDump) {
			sumFileNumber += micFileNumber;
		}
		
		ppFile = (FILE **)malloc(sizeof(FILE *) * sumFileNumber);

		if(NULL == ppFile) {
			ret = ERROR_AUDIO;
			return ret;
		}

		unsigned int indexFile = 0;
		
		ppFile[0] = fopen(WAKEUP_DUMP_FILE_PATH,"wb");		
		if(NULL == ppFile[0]) {
			ret = ERROR_AUDIO;
			break;
		}
		
		ppFile[1] = fopen(RECON_DUMP_FILE_PATH,"wb");
		if(NULL == ppFile[1]) {
			ret = ERROR_AUDIO;
			break;
		}
		
		indexFile = algoFileNumber;
		
		unsigned index = 0;
		char filePath[256] = {0};
		
		for(index = 0;index < resFileNumber;index++) {
			snprintf(filePath,sizeof(filePath),"%s%d.pcm",RES_DUMP_FILE_PATH,index);
			ppFile[2+index] = fopen(filePath,"wb");
			if(NULL == ppFile[algoFileNumber+index]) {
				ret = ERROR_AUDIO;
				break;
			}
			indexFile++;
			memset(filePath,0,sizeof(filePath));
		}

		if(SUCCESS_AUDIO != ret) {
			break;
		}
		
		unsigned int tmpIndex = indexFile;
		memset(filePath,0,sizeof(filePath));
		for(index = 0;index < micFileNumber;index++) {
			snprintf(filePath,sizeof(filePath),"%s%d.pcm",MIC_AFTER_DUMP_FILE_PATH,index);
			ppFile[tmpIndex+index] = fopen(filePath,"wb");
			if(NULL == ppFile[tmpIndex+index]) {
				ret = ERROR_AUDIO;
				break;
			}
			indexFile++;
			memset(filePath,0,sizeof(filePath));
		}

		if(SUCCESS_AUDIO != ret) {
			break;
		}

		tmpIndex = indexFile;
		memset(filePath,0,sizeof(filePath));
		
		for(index = 0;index < micFileNumber;index++) {
			snprintf(filePath,sizeof(filePath),"%s%d.pcm",MIC_BEFORE_DUMP_FILE_PATH,index);
			ppFile[tmpIndex+index] = fopen(filePath,"wb");
			if(NULL == ppFile[tmpIndex+index]) {
				ret = ERROR_AUDIO;
				break;
			}
			memset(filePath,0,sizeof(filePath));
		}

		
	} while(0);

	if(SUCCESS_AUDIO != ret) {
		close();
	}
	return ret;
}

void handlerSignal(int signal) {

    if(SIGUSR1 == signal) {

      if(false == AlgoDump::enableDump) {
		AlgoDump::enableDump = true;
		AlgoDump::enableBeforeDump = true;
	  }
	  else {
		AlgoDump::enableDump = false;
		AlgoDump::enableBeforeDump = false;
	  }

    }    
    
}

void AlgoDump::registerSignal() {                                                                                                                                                                

    signal(SIGUSR1,handlerSignal);
}

int AlgoDump::close() {

	int ret = 0;
	unsigned int index = 0;

	do {
		if(false == enableDump) {
			ret = ERROR_AUDIO;
			break;
		}
				
		for(index = 0;index < sumFileNumber;index++) {
			if(NULL != ppFile[index]) {
				fflush(ppFile[index]);
				fclose(ppFile[index]);
				ppFile[index] = NULL;
			}
		}
	} while(0);

	if(NULL != ppFile) {
		free(ppFile);
		ppFile = NULL;
	}
	
	return ret;
}

int AlgoDump::writeWakeAndRec(char * pWake, unsigned int wakeDataSize, char * pRec, unsigned int recDataSize) {

	int ret = 0;

	do {

		if(false == enableDump) {
			ret = ERROR_AUDIO;
			break;
		}
				
		if((NULL == pWake) || (NULL == pRec)) {
			ret = ERROR_AUDIO;
			break;
		}

		if((NULL == ppFile[0]) || (NULL == ppFile[1])) {
			ret = ERROR_AUDIO;
			break;
		}

		fwrite(pWake, 1, wakeDataSize,  ppFile[0]);
		fwrite(pRec, 1, recDataSize,  ppFile[1]);
	} while(0);

	return ret;
}

int AlgoDump::writeMicAndRes(char * pMic, unsigned int micDataSize, char * pRes, unsigned int resDataSize, int flag) {

	int ret = 0;

	do {

		if(false == enableDump) {
			ret = ERROR_AUDIO;
			break;
		}
				
		if((NULL == pMic) || (NULL == pRes)) {
			ret = ERROR_AUDIO;
			break;
		}
		
		unsigned int index = 0;
		unsigned int offSet = 0;
		unsigned int perSize = resDataSize / resFileNumber;

		if(before == flag) {
			for(index = 0;index < resFileNumber;index++) {
				fwrite(pRes + offSet, 1, perSize,  ppFile[algoFileNumber+index]);
				offSet += perSize;
			}
		}

		perSize = micDataSize / micFileNumber;

		if(after == flag) {
			offSet = 0;
			unsigned int sub = 2 + resFileNumber;
		
			for(index = 0;index < micFileNumber;index++) {
				fwrite(pMic + offSet, 1, perSize,  ppFile[sub+index]);
				offSet += perSize;
			}		
		}
				
		if(before == flag) { //write before
			if(true == enableBeforeDump) {
				offSet = 0;
				unsigned int sub = 2 + resFileNumber + micFileNumber;			
				for(index = 0;index < micFileNumber;index++) {
					fwrite(pMic + offSet, 1, perSize,  ppFile[sub+index]);
					offSet += perSize;
				}
			}
		}
	} while(0);

	return ret;
}

void AlgoDump::setMicFileNumber(unsigned int number) {

	micFileNumber = number;
}

void AlgoDump::setResFileNumebr(unsigned int number) {

	resFileNumber = number;
}

