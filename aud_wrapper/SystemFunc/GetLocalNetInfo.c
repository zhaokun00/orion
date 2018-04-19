// Get local net information function
// Jianqiang Wei
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "GetLocalNetInfo.h"
#include "RTInterphoneLog.h"

void GetLocalIp(char* ifname, char* ipaddr)
{
    int sock_get_ip;
    struct sockaddr_in *sin;
    struct ifreq ifr_ip;

    if ((sock_get_ip = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
         LOGI("[%s : %s] socket create failure.", __FILE__, __FUNCTION__);
         strcpy(ipaddr, "");
         return;
    }

    memset(&ifr_ip, 0, sizeof(ifr_ip));
    strncpy(ifr_ip.ifr_name, ifname, sizeof(ifr_ip.ifr_name)-1);

    if (ioctl(sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0)
    {
         LOGI("[%s : %s] ioctl failure.", __FILE__, __FUNCTION__);
         strcpy(ipaddr, "");
         return;
    }

    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;
    strcpy(ipaddr, inet_ntoa(sin->sin_addr));

    close(sock_get_ip);

    return;
}

void GetLocalMac(char* ifname, char* macaddr)
{
    int sock_mac;
    struct ifreq ifr_mac;

    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_mac == -1)
    {
        LOGI("[%s : %s] socket create failure.", __FILE__, __FUNCTION__);
        strcpy(macaddr, "");
        return;
    }

    memset(&ifr_mac, 0, sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, ifname, sizeof(ifr_mac.ifr_name)-1);

    if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        LOGI("[%s : %s] ioctl failure.", __FILE__, __FUNCTION__);
        strcpy(macaddr, "");
        return;
    }

    sprintf(macaddr,"%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

    close(sock_mac);

    return;
}

void GetLocalNetMask(char* ifname, char* netmask_addr)
{
    int sock_netmask;
    struct ifreq ifr_mask;
    struct sockaddr_in *net_mask;

    sock_netmask = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_netmask == -1)
    {
        LOGI("[%s : %s] socket create failure.", __FILE__, __FUNCTION__);
        strcpy(netmask_addr, "");
        return;
    }

    memset(&ifr_mask, 0, sizeof(ifr_mask));
    strncpy(ifr_mask.ifr_name, ifname, sizeof(ifr_mask.ifr_name )-1);

    if ((ioctl(sock_netmask, SIOCGIFNETMASK, &ifr_mask )) < 0)
    {
        LOGI("[%s : %s] ioctl failure.", __FILE__, __FUNCTION__);
        strcpy(netmask_addr, "");
        return;
    }

    net_mask = (struct sockaddr_in*)&(ifr_mask.ifr_netmask);
    strcpy(netmask_addr, inet_ntoa(net_mask->sin_addr));

    close(sock_netmask);

    return;
}

void GetGateWay(char* gateway)
{
    FILE *fp;
    char buf[512];
    char cmd[128];
    char *tmp;

    strcpy(cmd, "ip route");
    fp = popen(cmd, "r");
    if (fp == NULL)
    {
        LOGI("[%s : %s] popen error.", __FILE__, __FUNCTION__);
        strcpy(gateway, "");
        return;
    }
    while(fgets(buf, sizeof(buf), fp) != NULL)
    {
        tmp = buf;
        while(*tmp && isspace(*tmp))
            ++tmp;
        if(strncmp(tmp, "default", strlen("default")) == 0)
            break;
    }
    sscanf(buf, "%*s%*s%s", gateway);
    pclose(fp);

    return;
}

char* getLocalNetInfo()
{
	FILE* fpDev = NULL;
    static char strNetInfo[512] = {0};
    char strLine[128] = {0};
    char strName[64] = {0};

    strcpy(strNetInfo, "[Local Net Info Begin]\n");

    fpDev = fopen("/proc/net/dev", "r");
    if (!fpDev)
    {
    	LOGI("[%s : %s], Can't open net device system file.", __FILE__, __FUNCTION__);
    	strcat(strNetInfo, "Can't open net device system file and fail to get!\n");
    	return strNetInfo;
    }

    fgets(strLine, sizeof(strLine), fpDev);
    fgets(strLine, sizeof(strLine), fpDev);
	while (fgets(strLine, sizeof(strLine), fpDev))
	{
		sscanf(strLine, "%s", strName);
		if (strlen(strName) <= 0)
			continue;
		if (strName[strlen(strName)-1] == ':')
			strName[strlen(strName)-1] = 0;

		strcat(strNetInfo, "<");
		strcat(strNetInfo, strName);
		strcat(strNetInfo, ">\n");

		char strTmp[128];
		// Get local IP address
		strcat(strNetInfo, "IP Addr.: ");
		GetLocalIp(strName, strTmp);
		strcat(strNetInfo, strTmp);
		strcat(strNetInfo, "\n");

		// Get Mac address
		strcat(strNetInfo, "MAC Addr.: ");
		GetLocalMac(strName, strTmp);
		strcat(strNetInfo, strTmp);
		strcat(strNetInfo, "\n");

		// Get Net Mask address
		strcat(strNetInfo, "Net Mask Addr.: ");
		GetLocalNetMask(strName, strTmp);
		strcat(strNetInfo, strTmp);
		strcat(strNetInfo, "\n");
	}

	// Get Gateway address
	strcat(strNetInfo, "Default GateWay Addr.: ");
	GetGateWay(strLine);
	strcat(strNetInfo, strLine);
	strcat(strNetInfo, "\n[Local Net Info End]\n");

	fclose(fpDev);
	fpDev = NULL;

	return strNetInfo;
}
