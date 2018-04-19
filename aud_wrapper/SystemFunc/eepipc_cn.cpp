#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "eepipc_cn.h"

#include <cutils/log.h>

#define DEBUG 1

static const char *IPCNAME_CN[] =
{
    IPC_IDS_CN(ID_TO_PATH_CN)
};

int name_to_id_cn(const char *name)
{
    int i;

    for (i = 0; i < NUM_OF_IPCS_CN; i++)
    {
        if (!strcmp(IPCNAME_CN[i], name))
            break;
    }
    
#if DEBUG
    if (i >= NUM_OF_IPCS_CN)
    {
        printf("Not a predefined unix path(%s)!\n", name);
    }
#endif
    return i;
}

const char *id_to_name_cn(eepipc_cn_t id)
{
    if (id >= NUM_OF_IPCS_CN)
    {
#if DEBUG
        printf("Not a predefined id(%d>=%d)!\n", id, NUM_OF_IPCS_CN);
#endif        
        return NULL;
    }
    else
        return IPCNAME_CN[id];
}

int ipc_open_cn(eepipc_cn_t id)
{
    int s = -1;
    struct sockaddr_un addr;
    const char *ipcname;
    int enable = 1;

    ALOGD("+++ [ipc_open_cn]");
    
    if ((ipcname = id_to_name_cn(id)) == NULL)
    {
        ALOGE("--- [ipc_open_cn] EINVAL: wrong id");
        return -EINVAL;
    }

    if (unlink(ipcname))
	{
		ALOGD("IPC domain socket unlink failed!\n");
	}
	else
		ALOGD("ipcname = %s", ipcname);

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) != -1)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, ipcname);

        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        {
            ALOGE(" set resuse failed. ");
            close(s);
            return -1;
        }
        
        // unlink(ipcname);
        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) //bind fail
        {
            printf(" <ipc_open_cn> bind failed \n");
            ALOGE(" <ipc_open_cn> bind failed:%s \n", strerror(errno));
            close(s);
            return -1;
        }
    }
    else
    {
        ALOGE(" wrong socket:%d (%s) ", s, strerror(errno));
    }

    return s;
}

void ipc_make_noblock(int s)
{
    fcntl(s, F_SETFL, O_NONBLOCK);
}

int ipc_listen_cn(int s, int listen_q_len)
{
    struct sockaddr_un addr;
    int ret = -1;
    const char *ipcname;

    do
    {
        ret = listen(s, listen_q_len);
        if (ret == -1) 
        {
            perror("listen error");
            break;
        }
    }
    while (0);

    return ret;
}

int ipc_accept_cn(int s)
{
    struct sockaddr_un addr;
    int cl_id = -1;
    const char *ipcname;

    do
    {
        cl_id = accept(s, NULL, NULL);
    }
    while (0);

    return cl_id;
}

int ipc_connect_cn(eepipc_cn_t peerid)
{
    int s = -1;
    struct sockaddr_un addr;
    const char *ipcname;
    int enable = 1;

    if ((ipcname = id_to_name_cn(peerid)) == NULL)
        return -EINVAL;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) != -1)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, ipcname);

        printf(" ipcname=%s \n", ipcname);

        fcntl(s, F_SETFL, O_NONBLOCK);

        if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0)
        {
            ALOGE(" set resuse failed. ");
            close(s);
            return -1;
        }
        
        if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) 
        {
              perror("connect error");
              close(s);
              return -EINVAL;
        }
    }

    return s;
}

int ipc_read_cn(int s_cl, void *buf, int len)
{
    int count;

    count = read(s_cl, buf, len); 

    return count;
}

int ipc_write_cn(int s, void *buf, int len)
{
    if (write(s, buf, len) < 0) 
    {
        perror("write error");
        return -1;
    }    

    return 0;
}


int ipc_close_cn(int s)
{
    return close(s);
}

int ipc_clean_cn(eepipc_cn_t id)
{
    const char *ipcname;

    if ((ipcname = id_to_name_cn(id)) == NULL)
        return -EINVAL;

    unlink(ipcname);    

    return 0;
}

