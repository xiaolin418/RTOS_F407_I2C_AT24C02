#ifndef _APP_I2C_H_
#define _APP_I2C_H_


#include "stm32f4xx_hal.h"

#define KEY0 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4) //PE4
#define KEY1 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)	//PE3 
#define KEY2 		HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_2) //PE2

#define	AT24C02_ADDR_WRITE	0xA0
#define	AT24C02_ADDR_READ	0xA1


void At24c02_W_Byte_Test(void);
void At24c02_R_Byte_Test(void);
void At24c02_W_Page_Test(void);
void At24c02_R_Page_Test(void);

#endif  /* _APP_I2C_H_ */
