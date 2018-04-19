#ifndef _EASR_IDENTIFYMODULE_
#define _EASE_IDENTIFYMODULE_

#ifdef __cplusplus 
extern "C"
{
#endif
int MakeIdentifyLicense(unsigned int appID, const char* platform, const char* packageName, const char* signMD5, const char* cuid, const char* deadTime, char* out_license);
int VerifyIdentifyLicense(const char* packageName, const char* appCode, const char* license, unsigned int& appID,const char* platform,const char* cuid);

#ifdef __cplusplus
}
#endif

#endif
