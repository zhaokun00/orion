/******************************************************************************* 
 * File name: utils_ringbuf.h
 * Description: Declaration and definition for the utility libary, ring buffer
 * Author: Tim Jiang
 * Revision: 0.1
 * History:
 *          Initial version, Dec 3rd, 2015, Tim Jiang
 *
 ******************************************************************************/

#ifndef UTILS_RINGBUF_H____
#define UTILS_RINGBUF_H____

/*/
  * Header files
  */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "CriticalSectionPosix.h"

/*/
  * Macro definitions
  */
#define BYTES_AVAIL_TO_WRITE(r, w, z) ((w) >= (r))?((z) - ((w) - (r))):((r) - (w))


/*/
  * Data structures
  */
typedef struct MediaRingBufferS
{
	unsigned int size;
	char *pBuf;

	uint32_t writePos;
	uint32_t readPos;

	CriticalSectionPosix *cx;

	// member info: list queue
	uint32_t queueSize;
	uint32_t queueCnt;

	// member info: pcm device
	uint32_t pcmPeriodSize;
} MediaRingBufferT;


/*/
  * Function declaration
  */
int initMediaRingBuf(MediaRingBufferT *pRingBuf, uint32_t bufSize, uint32_t queueSize);
void releaseMediaRingBuf(MediaRingBufferT *pRingBuf);
int readMediaRingBuf(MediaRingBufferT *pRingBuf, char *pData, uint32_t len); 
int readWoCpyMediaRingBuf(MediaRingBufferT *pRingBuf, uint32_t len);
int writeMediaRingBuf(MediaRingBufferT *pRingBuf, char *pData, uint32_t len);
uint32_t getWritePosMediaRingBuf(MediaRingBufferT *pRingBuf);
uint32_t getReadPosMediaRingBuf(MediaRingBufferT *pRingBuf);


// int getWritePosMediaRingBuf(MediaRingBufferT *pRingBuf);

#endif

