#define LOG_TAG "AudioIpc"

#include "AudioIpc.h"

AudioIpc::AudioIpc(AudioAlgo * obj) {

	log("AudioIpc construct\n");
	
	fdDir = -1;
    fdDirCl = -1;

	dirCtrlReady = false;
	dirCtrlThread = NULL;;
    dirCtrlReadyCx = NULL;

	algo = obj;
}

AudioIpc::AudioIpc() {

	log("AudioIpc destructor\n");
}

bool handlerEvent(void *ptr) {

	bool ret = true;
    float dir = 0.0f;
    struct timeval tv;
    int retval;
    fd_set readset;
    float wakeup_dir = 0.0F;

	AudioIpc *obj = (AudioIpc *)ptr;
    do {
		
        if (obj == NULL) {
            ret = false;
            break;
        }

		if (false == obj->isDirCtlReady()) {
			 usleep(40000);
			break;
		}
		
        if (obj->fdDirCl == -1) {
            tv.tv_sec = 0;
            tv.tv_usec = 500000;

            FD_ZERO(&readset);
            FD_SET(obj->fdDir, &readset);

            retval = select(obj->fdDir + 1, &readset, NULL, NULL, &tv);

            if (retval > 0) {
                if (FD_ISSET(obj->fdDir, &readset)) {
                    obj->fdDirCl = ipc_accept_cn(obj->fdDir);

                    if (obj->fdDirCl < 0) {
                        log("accept failed\n");
                        break;
                    }

                    ipc_make_noblock(obj->fdDirCl);
                }
            }
			else if (retval == 0) {
                log(" select timeout...\n");
                break;
            } 
			else {
                log("select failed...\n");
                break;
            }
        } 
		else {
            tv.tv_sec = 0;
            tv.tv_usec = 300000;

            FD_ZERO(&readset);
            FD_SET(obj->fdDirCl, &readset);

            retval = select(obj->fdDirCl + 1, &readset, NULL, NULL, &tv);

            if (retval > 0) {
                if (FD_ISSET(obj->fdDirCl, &readset)) {
                    ipc_read_cn(obj->fdDirCl, (void *)&obj->dirMsg, sizeof (obj->dirMsg));

					switch(obj->dirMsg.type) {
						case DIR_DATA_TYPE: {
                            if ((obj->dirMsg.body.dir_val >= 0) && (obj->dirMsg.body.dir_val <= 360)) {
                    
                            } 
							else {
                                log(" <%s> invalid direction data.\n", __func__);
                            }
							break;
						}
						case TI_DATA_TYPE: {
							if(NULL == obj->algo) {
								break;
							}

							log(" obj->dir_msg.type = %d \n", obj->dirMsg.type);
                            log(" obj->dir_msg.body.wakloca_timeinfo.WakTimeTickH = %d \n", obj->dirMsg.body.wakloca_timeinfo.WakTimeTickH);
                            log(" obj->dir_msg.body.wakloca_timeinfo.WakTimeTickL = %d \n", obj->dirMsg.body.wakloca_timeinfo.WakTimeTickL);
                            log(" obj->dir_msg.body.wakloca_timeinfo.WakWordNum = %d \n", obj->dirMsg.body.wakloca_timeinfo.WakWordNum);
                            log(" obj->dir_msg.body.wakloca_timeinfo.WakWordStartTimeOffset[0] = %d \n", obj->dirMsg.body.wakloca_timeinfo.WakWordStartTime[0]);
                            log(" obj->dir_msg.body.wakloca_timeinfo.WakWordEndTimeOffset[0] = %d \n", obj->dirMsg.body.wakloca_timeinfo.WakWordEndTime[0]);
							
							if(obj->algo->mapm_compute(&obj->dirMsg.body.wakloca_timeinfo, &wakeup_dir) != 0) {
								log("mapm_compute_wakLoca() failed.\n");
                                break;
							}
							
                            if (obj->sverifyDone && !obj->securityVerify) {
                                log("Security verify not pass, sending unexpect data\n");
                                obj->halMsg.type = DIR_WAKEUP_MSG_TYPE;
                                obj->halMsg.body.dir_wakeup = (int)0;
                            } 
							else {
                                obj->halMsg.type = DIR_WAKEUP_MSG_TYPE;
                                obj->halMsg.body.dir_wakeup = (int)wakeup_dir;
                            }
							
                            if (ipc_write_cn(obj->fdDirCl, &obj->halMsg, sizeof(obj->halMsg)) < 0) {                   
                                log("send msg failed\n");
                                break;
                            }
							break;
						}
						case LIC_DATA_TYPE: {
							unsigned int appID;
                            char *p_pktName;
                            char *p_appCode;
                            char *p_licPath;
                            char *p_plaform;
                            char *p_cUID;

                            appID		= obj->dirMsg.body.license_info.appID;
                            p_pktName	= (char *)(obj->dirMsg.body.license_info.pktName);
                            p_appCode	= (char *)(obj->dirMsg.body.license_info.appCode);
                            p_licPath	= (char *)(obj->dirMsg.body.license_info.licPath);
                            p_plaform	= (char *)(obj->dirMsg.body.license_info.pltform);
                            p_cUID		= (char *)(obj->dirMsg.body.license_info.cUID);

                            if (!appID) {
                                log("Warnnig: license APPID is empty!\n");
                            }

							log("license info: \n%d, \n%s, \n%s, \n%s, \n%s, \n%s\n", appID, p_pktName, p_appCode, p_licPath, p_plaform, p_cUID);

                            ret  = obj->strRangeValidation(p_pktName, LICENSE_PRAMS_LEN_MAX);
                            ret |= obj->strRangeValidation(p_appCode, LICENSE_PRAMS_LEN_MAX);
                            ret |= obj->strRangeValidation(p_licPath, LICENSE_PRAMS_LEN_MAX);
                            ret |= obj->strRangeValidation(p_plaform, LICENSE_PRAMS_LEN_MAX);
                            ret |= obj->strRangeValidation(p_cUID, LICENSE_PRAMS_LEN_MAX);

                            if (ret) {
                                log("<%s> error license params\n", __FUNCTION__);
                                break;
                            }

                            int myRet = 0;

                            myRet = VerifyIdentifyLicense(p_pktName, p_appCode, p_licPath, appID, p_plaform, p_cUID);
                            obj->sverifyDone = 1;

                            if ((myRet >= 0)) {
                                log("license verify pass, ret: %d!\n", ret);
                                obj->securityVerify = 1;
                            } else {
                                log("license verify failed!, %d\n", myRet);
                                break;
                            }
							break;
						}						
					}
				} 
				else {
					
                }
            } 
			else if (retval == 0) {
                break;
            }
			else {
                log("select failed\n");
                break;
            }
        }
    } while (0);

	return ret;
}

int AudioIpc::init() {

	int ret = 0;

	do {
        fdDir = ipc_open_cn(bf_dir);
		
        if(-1 == fdDir) {
            log(" ipc_open_cn() failed.");
            ret = -1;
			break;
        }
		
        system("chmod 777 /data/local/ipc/bf_dir");

        ipc_make_noblock(fdDir);
        ipc_listen_cn(fdDir, MAX_IPC_LSN_NUM);

		dirCtrlReady = false;
		dirCtrlReadyCx = CriticalSectionPosix::Create();
		dirCtrlThread = ThreadPosix::Create(handlerEvent, this, kRealtimePriority, "ctl direction Thread");
		
	} while(0);
	

	return ret;
}

int AudioIpc::start() {

	int ret = 0;

	setDirCtlReady(true);
	
	Rtip_UWord32 dirCtlId = 0;
	dirCtrlThread->Start(dirCtlId);
	
	return ret;
}

int AudioIpc::release() {

	int ret = 0;

	if(NULL != dirCtrlReadyCx) {
		setDirCtlReady(false);
	}
	
	if(NULL != dirCtrlThread) {
		dirCtrlThread->Stop();
	}

	usleep(100);

	if(NULL != dirCtrlThread) {	
		delete dirCtrlThread;
		dirCtrlThread = NULL;
	}

	if(NULL != dirCtrlReadyCx) {
		delete dirCtrlReadyCx;
		dirCtrlReadyCx = NULL;
	}

	if(fdDirCl > 0) {
        close(fdDirCl);
        fdDirCl = -1;
    }

    if(fdDir > 0) {
    	close(fdDir);
     	fdDir = -1;
    }

    ipc_clean_cn(bf_dir);
	
	return ret;
}

bool AudioIpc::isDirCtlReady() {

	CriticalSectionScoped critScoped(dirCtrlReadyCx);

	return dirCtrlReady;
}

void AudioIpc::setDirCtlReady(bool val) {

	CriticalSectionScoped critScoped(dirCtrlReadyCx);
	dirCtrlReady = val;
}

int AudioIpc::strRangeValidation(const char * str, unsigned int maxLen) {

    int ret = 0;
    unsigned int len = 0;

    if (NULL == str) {
        log("<%s>: empty string!\n", __FUNCTION__);
        return -1;
    }

    if ((len = strlen(str)) > maxLen) {
        log("<%s>: string length %d over max %d!\n", __FUNCTION__, len, maxLen);
        return -1;
    }

    return ret;
}
