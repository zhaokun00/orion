#include "auto_loudness_control.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#define DIR_TYPE   4
#define FILE_TYPE  8
#define LINK_TYPE 10

#define PATH_LEN 1024

#define FILE_NUMBER	100
#define FILE_PATH_LEN 256

#define ROOT "voice"

int readFileList(char *pPath);
void playing_demo();

#if 0
char fname[FILE_NUMBER][FILE_PATH_LEN] =
{
	"voice/北京你好46104849.wav",
	"voice/吃论46104839.wav",
	"voice/赌论46104718.wav",
	"voice/富贵图46104837.wav",
	"voice/婚姻与家庭46104829.wav",
	"voice/论梦46104774.wav",
	"voice/卖布头46104773.wav",
	"voice/文王卦46104779.wav",
	"voice/我要闹绯闻46104769.wav",
	"voice/学电台46104715.wav"
};
#endif

static char fname[FILE_NUMBER][FILE_PATH_LEN] = {0};
static int gFileCount = 0;
void main()
{
//	playing_demo();		
	char basePath[1024] = {0};
	int i = 0;
    getcwd(basePath, sizeof(basePath) - 1);

	strcat(basePath,"/");
	strcat(basePath,"voice");
	readFileList(basePath);

#if 0
	for(i = 0;i < gFileCount;i++) {
		printf("%s\n",fname[i]);
	}
#endif

	playing_demo();
}

void playing_demo()
{
	int i, level, channels = 2;
	short buf[256];
	FILE *fp, *fpo;
	float gain = 1;

	char *pFile = NULL;
	
	for (i = 0; i < gFileCount; i++) {

		gain = get_gain(fname[i]);
		pFile = strchr(fname[i],'/');
	
		log("gain = %lf %s\n",gain,pFile+1);

#if 0
		fp = fopen(fname[i], "rb");
		if (!fp)	continue;

		strcpy(fname[i] + strlen(fname[i]) - 4, "_alc.wav");
		fpo = fopen(fname[i], "wb");
		if (!fp)	continue;
		
		fread(buf, 78, 1, fp);
		fwrite(buf, 78, 1, fpo);

		while (fread(buf, channels * sizeof(short), 1, fp)) {
			
			alc_run(buf, channels, 1,gain);
			
//			fwrite(buf, channels * sizeof(short), 1, fpo);
		}
		
		fclose(fp);
		fclose(fpo);
#endif

	}
}

int readFileList(char *pPath) {

	int ret = 0;

	do {

		if(NULL == pPath) {
			ret = -1;
			break;
		}
		
		DIR *pDir = NULL;

	    struct dirent *ptr;
	    char path[PATH_LEN] = {0};

	    if ((pDir=opendir(pPath)) == NULL) { //打开文件夹
	        perror("Open dir error");
	        ret = -1;
			break;
	    }

	    while ((ptr=readdir(pDir)) != NULL) { //读取文件夹

	        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0) { //忽略文件夹中的.和..
				continue;
			}
	        else if(ptr->d_type == FILE_TYPE) { //文件
//				 printf("d_name:%s/%s\n",pPath,ptr->d_name);
				 snprintf(fname[gFileCount++],sizeof(fname[gFileCount]),"%s/%s",ROOT,ptr->d_name);
			}
	        else if(ptr->d_type == LINK_TYPE) { //链接文件
				printf("d_name:%s/%s\n",pPath,ptr->d_name);
			}	            
	        else if(ptr->d_type == DIR_TYPE) { //目录
				memset(path,'\0',sizeof(path));
	            strncpy(path,pPath,strlen(pPath));
	            strcat(path,"/");
	            strcat(path,ptr->d_name);
	            readFileList(path);
			}
	    }
		
		closedir(pDir);
		pDir = NULL;
		
	}while(0);

	return ret;
}

