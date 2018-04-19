#ifndef EEPIPC_CN_H
#define EEPIPC_CN_H

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_IPC_PKG_LEN 1024
#define MAX_IPC_LSN_NUM 16


#ifdef ANDROID
#define ROOTDIR_CN "/data/local/ipc/"	// "/dev/socket/"
#else
#define ROOTDIR_CN "/tmp/"
#endif
#define CONCAT_CN(a, b) a b
#define ID_TO_PATH_CN(x) CONCAT_CN(ROOTDIR_CN, #x)
#define NOTHING_CN(x) x

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
#define IPC_IDS_CN(CONVERTER_CN) \
/* CONTROL PROCESS */\
CONVERTER_CN(IPC_AUD_CTRL_CN),\
CONVERTER_CN(IPC_AUD_DATA_CN), \
CONVERTER_CN(bf_dir)
#endif

typedef enum
{
    IPC_IDS_CN(NOTHING_CN),

    NUM_OF_IPCS_CN
}eepipc_cn_t;

//get id from IPC path
int name_to_id_cn(const char *name);

//get IPC path from id
const char *id_to_name_cn(eepipc_cn_t id);

//open a socket, the IPC path is get from id
int ipc_open_cn(eepipc_cn_t id);

void ipc_make_noblock(int s);

int ipc_listen_cn(int s, int listen_q_len);

int ipc_accept_cn(int s);

int ipc_connect_cn(eepipc_cn_t peerid);

int ipc_read_cn(int s_cl, void *buf, int len);

int ipc_write_cn(int s, void *buf, int len);

//close a socket
int ipc_close_cn(int s);

int ipc_clean_cn(eepipc_cn_t id);

#ifdef __cplusplus
}
#endif

#endif /* EEPIPC_H */
