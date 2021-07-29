/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	app_lwip.h
  *Author:    	xuesi.huang
  *Version:   	v1.0
  *Date:      	2018/09/15
  *Description:  
  *History:  
**********************************************************************************/
#ifndef __APP_LWIP_H
#define __APP_LWIP_H


#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
//#include "FreeRTOS_net_config.h"
#include "tcpip.h"
#include <stdio.h>
//#include "user_gfunction.h"
#include "lwip/udp.h"



extern struct netif xnetif;

void UDP_Test(void);

#endif /*endif __APP_LWIP_H*/
