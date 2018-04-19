#ifndef EEPIPC_H
#define EEPIPC_H

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_IPC_PKG_LEN 1024

#ifdef ANDROID
#define ROOTDIR "/dev/socket/"
#else
#define ROOTDIR "/tmp/"
#endif
#define CONCAT(a, b) a b
#define ID_TO_PATH(x) CONCAT(ROOTDIR, #x)
#define NOTHING(x) x

#if 0
#define IPC_IDS(CONVERTER) \
/* CONTROL PROCESS */\
CONVERTER(IPC_CAMERA_MGR),\
CONVERTER(IPC_PBOX_SVR),\
CONVERTER(IPC_PKG_MGR),\
CONVERTER(IPC_CTRL_CFG),\
\
/* TRACKING PROCESS */\
CONVERTER(IPC_TRACK_LOGIC),\
CONVERTER(IPC_AUDIO_DETECT),\
CONVERTER(IPC_VIDEO_DETECT),\
\
/* MONITOR PROCESS */\
CONVERTER(IPC_EEP_MAIN),\
CONVERTER(IPC_HW_WATCHDOG),\
CONVERTER(IPC_USB_DETECT),\
CONVERTER(IPC_CPU_USAGE),\
CONVERTER(IPC_IR_MONITOR),\
CONVERTER(IPC_NET_MONITOR),\
\
/* LOG PROCESS */\
CONVERTER(IPC_LOG_SVR),\
\
/* USB/MEDIA LOGIC PROCESS */\
CONVERTER(IPC_USB_MEDIA),\
\
/* CALIBRATION*/\
CONVERTER(IPC_EEP_CALIBRATION)
#else
#define IPC_IDS(CONVERTER) \
/* CONTROL PROCESS */\
CONVERTER(IPC_AUD_CTRL),\
CONVERTER(IPC_AUD_DATA)
#endif

typedef enum
{
    IPC_IDS(NOTHING),

    NUM_OF_IPCS
}eepipc_t;

//get id from IPC path
int name_to_id(const char *name);

//get IPC path from id
const char *id_to_name(eepipc_t id);

//open a socket, the IPC path is get from id
int ipc_open(eepipc_t id);

//send packet to remote socket, whose IPC path is get from peerid
int ipc_sendto(int s, eepipc_t peerid, const void *msg, int len);

//send packet to all remote socket marked in mask, 
//remote sockets failed to send to will be remain in mask, which means 0 denotes all socket has been sent to successfully 
int ipc_boadcast(int s, unsigned int mask, const void *msg, int len);

//recevie packet from remote socket, and will return the peerid which is get from IPC path
int ipc_recvfrom(int s, void *buf, int len, eepipc_t *peerid);

//close a socket
int ipc_close(int s);

#ifdef __cplusplus
}
#endif

#endif /* EEPIPC_H */
