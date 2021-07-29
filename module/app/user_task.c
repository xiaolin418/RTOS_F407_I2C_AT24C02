#include "user_task.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "app_lwip.h"
#include "app_i2c.h"





void StartDefaultTask(void *argument)
{


	for(;;)
	{
		osDelay(10);
	}

}


void StartETHTask(void *argument)
{
 
	for(;;)
	{
	  
		UDP_Test();
		osDelay(10);
	}
 
}


void StartSMBusTask(void *argument)
{
	uint8_t cnt = 0;
	uint8_t cnt1 = 0;
	
	for(;;)
	{
		if(KEY1 == 0)
		{
			if(cnt == 0)
			{
				cnt = 1;
//				At24c02_W_Byte_Test();
				At24c02_W_Page_Test();
			}
		}
		else
		{
			cnt = 0;
		}
		
		if(KEY0 == 0)
		{
			if(cnt1 == 0)
			{
				cnt1 = 1;
//				At24c02_R_Byte_Test();
				At24c02_R_Page_Test();
			}
		}
		else
		{
			cnt1 = 0;
		}
		
		osDelay(10);
	}

}


void StartOtherTask(void *argument)
{
	for(;;)
	{
		osDelay(10);
	}

}






























