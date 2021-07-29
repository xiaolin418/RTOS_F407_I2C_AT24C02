/*********************************************************************************
  *Copyright(C), 2018, Aputure, All rights reserver.
  *FileName:  	app_lwip.c
  *Author:    	xuesi.huang
  *Version:   	v1.0
  *Date:      	2018/09/15
  *Description:  
  *History:  
**********************************************************************************/
#include "app_lwip.h"

extern ETH_HandleTypeDef heth;

/* Private typedef -----------------------------------------------------------*/
typedef enum 
{ 
  DHCP_START=0,
  DHCP_WAIT_ADDRESS,
  DHCP_ADDRESS_ASSIGNED,
  DHCP_TIMEOUT
} DHCP_State_TypeDef;

/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif xnetif; /* network interface structure */

void udp_echoserver_receive(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	//memset(udp_rx_buf, 0x0, sizeof(udp_rx_buf));
	//memcpy(udp_rx_buf, p->payload, p->len);
	/* Tell the client that we have accepted it */
	//addr->addr = 0XFFFFFFFF;
	
	  /* 打印接收到的地址 */
    printf("get msg from %d:%d:%d:%d port:%d:\r\n",
        *((uint8_t *)&addr->addr), *((uint8_t *)&addr->addr + 1),
        *((uint8_t *)&addr->addr + 2), *((uint8_t *)&addr->addr + 3), port);
	
     /* 把数据打印到“网络数据接收”，回显功能（返回服务器显示） */
	udp_sendto(upcb, p, addr, port);
	/* Free the p buffer */
	pbuf_free(p);
}

/* UDP服务器模式 */
void udp_echoserver_init(void)
{
   struct udp_pcb *upcb;
   err_t err;

   /* Create a new UDP control block  */
   upcb = udp_new();  

   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, 8880);   /* 这个端口号不能和ArtNet相同，ArtNet端口号固定位 6454 */

      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive, NULL);   
		printf("udp server connected\r\n");
      }
      else
      {
        udp_remove(upcb);
      }
   }
}

//0:未接网线  1：已接网线
uint8_t GetLinkState(void) 
{
	uint32_t PHY_Value = NULL;
  
    HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &PHY_Value);
    if(((PHY_Value & PHY_LINKED_STATUS) != PHY_LINKED_STATUS))
    {
		return 0;
    }

    return 1;
}

void UDP_Test(void)
{
	if(GetLinkState())
	{
		udp_echoserver_init();
		vTaskDelete(NULL);
	}
}








