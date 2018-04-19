/******************************************************************************* 
 * File name: usb_audio_app.cpp
 * Description: Implement usb audio device embedded application. 
 * Author: Tim Jiang
 * Revision: 0.1
 * History:
 *          Initial version, Oct 27th, 2015, Tim Jiang
 *
 ******************************************************************************/
 
#include <stdint.h>
#include <assert.h>
#include "error_code.h"
#include "utils_ringbuf.h"
#include "aud_wrapper_log.h"
#include <cutils/log.h>
#include "RTInterphoneLog.h"


/*
  * Internal functions
  */
static inline void	_getRingBufferAvailBytes(MediaRingBufferT *pRingBuf, uint32_t *read, uint32_t *write)
{
	uint32_t read_loc, write_loc;

	/* Capture the read/write indices before they changed */
	read_loc = pRingBuf->readPos;
	write_loc = pRingBuf->writePos;

	*write = BYTES_AVAIL_TO_WRITE(read_loc, write_loc, pRingBuf->size);
	*read = pRingBuf->size - *write;
}

static inline uint32_t _getNextWriteLocation(MediaRingBufferT *pRingBuf)
{
	uint32_t next = pRingBuf->writePos;

	assert(next < pRingBuf->size);

	return next;
}

static inline void _setNextWriteLocation(MediaRingBufferT *pRingBuf, uint32_t next)
{
	pRingBuf->writePos = next;
}

static inline uint32_t _getNextReadLocation(MediaRingBufferT *pRingBuf)
{
	uint32_t next = pRingBuf->readPos;

	assert(next < pRingBuf->size);

	return next;
}

static inline uint32_t _getNextReadLocationWithOffset(MediaRingBufferT *pRingBuf, uint32_t Offset)
{
	uint32_t next = pRingBuf->readPos;

	assert(next < pRingBuf->size);
	next += Offset;
	next %= pRingBuf->size;

	return next;
}

static inline void _setNextReadLocation(MediaRingBufferT *pRingBuf, uint32_t next)
{
	pRingBuf->readPos = next;
}

static inline void* _getRingBuffer(MediaRingBufferT *pRingBuf)
{
	return (void *)pRingBuf->pBuf;
}

static inline uint32_t _getRingBufferSize(MediaRingBufferT *pRingBuf)
{
	return pRingBuf->size;
}

static uint32_t _copyToRingBuffer(MediaRingBufferT *pRingBuf, void* Src, uint32_t SrcLen, uint32_t startPos)
{
	void* ringBuffer =_getRingBuffer(pRingBuf);
	uint32_t ringBufferSize = _getRingBufferSize(pRingBuf);
	uint32_t fragLen;

	if (SrcLen > (ringBufferSize - startPos))
	{
		fragLen = ringBufferSize - startPos;
		memcpy((char *)ringBuffer + startPos, Src, fragLen);
		memcpy(ringBuffer, (char *)Src + fragLen, SrcLen - fragLen);
	}
	else
	{
		memcpy((char *)ringBuffer + startPos, Src, SrcLen);
	}

	return ((startPos + SrcLen) % ringBufferSize);
}

static uint32_t _copyFromRingBuffer(MediaRingBufferT *pRingBuf, void* Dest, uint32_t DestLen, uint32_t startPos)
{
	void* ringBuffer = _getRingBuffer(pRingBuf);
	uint32_t ringBufferSize = _getRingBufferSize(pRingBuf);
	uint32_t fragLen;

	if (DestLen > ringBufferSize - startPos)
	{
		fragLen = ringBufferSize - startPos;

		memcpy(Dest, (char *)ringBuffer + startPos, fragLen);
		memcpy((char *)Dest + fragLen, ringBuffer, DestLen - fragLen);
	}
	else
	{
		memcpy(Dest, (char *)ringBuffer + startPos, DestLen);
	}

	return ((startPos + DestLen) % ringBufferSize);
}


/*
  * Function: initMediaRingBuf
  * Description: Initialize ring buffer for audio media data
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  *	unsigned int bufSize: Total buffer size of the ring buffer
  * Return:
  *	0 - success
  *	-1 - failure
  */
int initMediaRingBuf(MediaRingBufferT *pRingBuf, uint32_t bufSize, uint32_t queueSize)
{
	int ret = ERR_NOERROR;

	do
	{
		if (pRingBuf == NULL)
		{
			LOGE(" <%s> invalid input params. \n", __FUNCTION__);
			ret = -ERR_NORMALERROR;
			break;
		}
		
		pRingBuf->cx = CriticalSectionPosix::Create();
        if (pRingBuf->cx == NULL)
        {
			LOGE(" <%s> invalid input params. \n", __FUNCTION__);
			ret = -ERR_NORMALERROR;
			break;	            
        }

		pRingBuf->pBuf = (char *)malloc(bufSize);
		if (pRingBuf->pBuf == NULL)
		{
			LOGE(" <%s> invalid input params. \n", __FUNCTION__);
			ret = -ERR_NORMALERROR;
			break;			
		}

		pRingBuf->size = bufSize;
		pRingBuf->readPos = 0;
		pRingBuf->writePos = 0;
		pRingBuf->queueSize = queueSize;
		pRingBuf->queueCnt = pRingBuf->size / pRingBuf->queueSize;
	}
	while (0);

	if (ret != ERR_NOERROR)
	{
		releaseMediaRingBuf(pRingBuf);
	}

	return ret;
}

/*
  * Function: releaseMediaRingBuf
  * Description: release ring buffer for audio media data
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  *	
  * Return:
  *	None
  */
void releaseMediaRingBuf(MediaRingBufferT *pRingBuf)
{
	do
	{
		if (pRingBuf == NULL)
		{
			LOGE(" <%s> invalid input params. \n", __FUNCTION__);
			break;
		}

		if  (pRingBuf->pBuf != NULL)
		{
			free(pRingBuf->pBuf);
			pRingBuf->pBuf = NULL;
		}
        
        if (pRingBuf->cx != NULL)
        {
            delete pRingBuf->cx;
            pRingBuf->cx = NULL;
        } 
	}
	while (0);
}

/*
  * Function: readMediaRingBuf
  * Description: read specified length data from ring buffer.
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  *	char *pData: Buffer to receive data
  *	unsigned int len: Length to read
  * Return:
  *	0 - success
  *	-1 - failure
  */
int readMediaRingBuf(MediaRingBufferT *pRingBuf, char *pData, uint32_t len)
{
	int ret = ERR_NOERROR;
	uint32_t bytesAvailToWrite;
	uint32_t bytesAvailToRead;
	uint32_t startPos;
	uint32_t nextReadLocation=0;

    // FUNC_E;   
   
	do
	{
		if ((pRingBuf == NULL) || (pData == NULL))
		{
			TA_DBG(" <%s> invalid input params. \n", __FUNCTION__);
			ret = -ERR_NORMALERROR;
			break;
		}

        CriticalSectionScoped critScoped(pRingBuf->cx);
        
		_getRingBufferAvailBytes(pRingBuf, &bytesAvailToRead, &bytesAvailToWrite);
	//	ALOGD("==========>availreadlen=%d,readlen=%d,zk",bytesAvailToRead,len);
		if (bytesAvailToRead < len)
		{
			TA_DBG(" <%s> not enough data to read. \n", __FUNCTION__);
			ret = -ERR_NOTENOUGHDATA;
			break;
		}

		startPos = _getNextReadLocation(pRingBuf);
		nextReadLocation = _copyFromRingBuffer(pRingBuf, pData, len, startPos);
		_setNextReadLocation(pRingBuf, nextReadLocation);	
	}
	while (0);

    // FUNC_L;
    
	return ret;	
}

/*
  * Function: readWoCpyMediaRingBuf
  * Description: read specified length data from ring buffer w/o copy.
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  *	char *pData: Buffer to receive data
  *	unsigned int len: Length to read
  * Return:
  *	0 - success
  *	-1 - failure
  */
int readWoCpyMediaRingBuf(MediaRingBufferT *pRingBuf, uint32_t len)
{
	int ret = ERR_NOERROR;
	uint32_t bytesAvailToWrite;
	uint32_t bytesAvailToRead;
	uint32_t startPos;
	uint32_t nextReadLocation=0;
	uint32_t ringBufferSize = 0;
	
	do
	{
		if (pRingBuf == NULL)
		{
			LOGE(" <%s> invalid input params. \n", __FUNCTION__);
			ret = -ERR_NORMALERROR;
			break;
		}

        CriticalSectionScoped critScoped(pRingBuf->cx);
        
		_getRingBufferAvailBytes(pRingBuf, &bytesAvailToRead, &bytesAvailToWrite);

        LOGE(" <%s> bytesAvailToRead=%d;bytesAvailToWrite=%d \n", 
            __FUNCTION__, bytesAvailToRead, bytesAvailToWrite);
        
		if (bytesAvailToRead < len)
		{
			LOGE(" <%s> not enough data to read. \n", __FUNCTION__);
			ret = -ERR_NOTENOUGHDATA;
			break;
		}

		startPos = _getNextReadLocation(pRingBuf);
		ringBufferSize = _getRingBufferSize(pRingBuf);
		nextReadLocation = (startPos + len) % ringBufferSize;
		_setNextReadLocation(pRingBuf, nextReadLocation);	
	}
	while (0);

	return ret;	
}


/*
  * Function: writeMediaRingBuf
  * Description: write specified length data ring buffer for audio media data
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  *	unsigned int bufSize: Total buffer size of the ring buffer
  * Return:
  *	0 - success
  *	-1 - failure
  */
int writeMediaRingBuf(MediaRingBufferT *pRingBuf, char *pData, uint32_t len)
{
	int ret = ERR_NOERROR;
	uint32_t bytesAvailToWrite;
	uint32_t bytesAvailToRead;
	uint32_t totalBytesToWrite=0;
	uint32_t startPos;
	uint32_t nextWriteLocation;

    // FUNC_E;

	do
	{
		if ((pRingBuf == NULL) || (pData == NULL))
		{
			TA_DBG(" <%s> invalid input params. \n", __FUNCTION__);
            ret = -ERR_NORMALERROR;
			break;
		}

        CriticalSectionScoped critScoped(pRingBuf->cx);

		_getRingBufferAvailBytes(pRingBuf, &bytesAvailToRead, &bytesAvailToWrite);	
		//ALOGD("==========>availwritelen=%d,writelen=%d,zk",bytesAvailToWrite,len);
		
		if (bytesAvailToWrite < len)
		{
			TA_DBG(" <%s> not enough buffer to write. \n", __FUNCTION__);
			ret = -ERR_NOTENOUGHDATA;
			break;
		}	
		
		startPos = _getNextWriteLocation(pRingBuf);
		nextWriteLocation = _copyToRingBuffer(pRingBuf, pData, len, startPos);
		_setNextWriteLocation(pRingBuf, nextWriteLocation);
		
		if(pRingBuf->readPos == pRingBuf->writePos) {

			ALOGD("========>ringbuffer is full<========number");
			return -1;
		}
	}
	while (0);

    // FUNC_L;

	return ret;	
}

/*
  * Function: getWritePosMediaRingBuf
  * Description: get the current wirte position of ring buffer.
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  * Return:
  *	Current write position
  */
uint32_t getWritePosMediaRingBuf(MediaRingBufferT *pRingBuf)
{
	return _getNextWriteLocation(pRingBuf);
}

/*
  * Function: getReadPosMediaRingBuf
  * Description: get the current read position of ring buffer.
  * Parameters:
  *	MediaRingBufferT *pRingBuf: Ring buffer object PTR
  * Return:
  *	Current read position
  */
uint32_t getReadPosMediaRingBuf(MediaRingBufferT *pRingBuf)
{
	return _getNextReadLocation(pRingBuf);
}

