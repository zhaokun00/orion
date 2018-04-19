#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "eepipc.h"

#define DEBUG 1

static const char *IPCNAME[] =
{
    IPC_IDS(ID_TO_PATH)
};

int name_to_id(const char *name)
{
    int i;

    for (i = 0; i < NUM_OF_IPCS; i++)
    {
        if (!strcmp(IPCNAME[i], name))
            break;
    }
    
#if DEBUG
    if (i >= NUM_OF_IPCS)
    {
        printf("Not a predefined unix path(%s)!\n", name);
    }
#endif
    return i;
}

const char *id_to_name(eepipc_t id)
{
    if (id >= NUM_OF_IPCS)
    {
#if DEBUG
        printf("Not a predefined id(%d>=%d)!\n", id, NUM_OF_IPCS);
#endif        
        return NULL;
    }
    else
        return IPCNAME[id];
}

int ipc_open(eepipc_t id)
{
    int s;
    struct sockaddr_un addr;
    const char *ipcname;

    if ((ipcname = id_to_name(id)) == NULL)
        return -EINVAL;
        
    if ((s = socket(AF_UNIX, SOCK_DGRAM, 0)) != -1)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, ipcname);

        unlink(ipcname);
        if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) //bind fail
        {
            close(s);
            return -1;
        }
    }

    return s;
}

int ipc_sendto(int s, eepipc_t peerid, const void *msg, int len)
{
    struct sockaddr_un addr;
    int count;
    const char *ipcname;

    if ((ipcname = id_to_name(peerid)) == NULL)
        return -EINVAL;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, ipcname);

    do
    {
        count = sendto(s, msg, len, 0, (struct sockaddr *)&addr, sizeof(addr));
    } while (count < 0 && errno == EINTR);  // we just handle EINTR

    return count;
}

int ipc_boadcast(int s, unsigned int mask, const void *msg, int len)
{
    int i, count;

    for (i = 0; i < NUM_OF_IPCS; i++)
    {
        if ((1 << i) & mask)
        {
            count = ipc_sendto(s, (eepipc_t)i, msg, len);
            if (count == len)
                mask &= ~(1 << i);
        }
    }
    
    return mask;
}

int ipc_recvfrom(int s, void *buf, int len, eepipc_t *peerid)
{
    struct sockaddr_un addr;
    socklen_t addr_len = sizeof(addr);
    int count;

    memset(&addr, 0, addr_len);

    do
    {
        count = recvfrom(s, buf, len, 0, (struct sockaddr *)&addr, &addr_len);
    } while (count < 0 && errno == EINTR);  //we just handle EINTR

    if (count > 0) //recvfrom is successfully executed
        *peerid = (eepipc_t)name_to_id(addr.sun_path);  //convert path into eep thread id

    return count;
}

int ipc_close(int s)
{
    return close(s);
}

