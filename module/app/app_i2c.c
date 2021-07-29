#include "app_i2c.h"
#include "usart.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;


uint8_t Rec_Data[100] = {0};
uint8_t Send_Data[] = "ttan谭小林xaskinas89qqqqqqqqq";

/* 连续写数据的时候，需要加写入完成判断 */
static HAL_StatusTypeDef I2C_Write_Data(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef Write_Status;
	
	Write_Status = HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	/* 检查设备是否写入完成 */
	while(HAL_I2C_IsDeviceReady(hi2c, DevAddress, 5, Timeout) != HAL_OK);		//5是指次数，检测5次
	
	return Write_Status;
}

static HAL_StatusTypeDef I2C_Read_Data(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef Read_Status;
	
	Read_Status = HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	while(HAL_I2C_IsDeviceReady(hi2c, DevAddress, 5, Timeout) != HAL_OK);
	
	return Read_Status;
}


/**
 * @brief		AT24C02任意地址写一个字节数据
 * @param		addr —— 写数据的地址（0-255）
 * @param		dat  —— 存放写入数据的地址
 * @retval		成功 —— HAL_OK
*/
static uint8_t At24c02_Write_Byte(uint16_t addr, uint8_t* dat)
{
	return HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, 1, 0xFFFF);
}


/**
 * @brief		AT24C02任意地址读一个字节数据
 * @param		addr —— 读数据的地址（0-255）
 * @param		read_buf —— 存放读取数据的地址
 * @retval		成功 —— HAL_OK
*/
static uint8_t At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf)
{
	return HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, read_buf, 1, 0xFFFF);
}

/**
 * @brief		AT24C02任意地址连续写多个字节数据
 * @param		addr —— 写数据的地址（0-255）
 * @param		dat  —— 存放写入数据的地址
 * @retval		成功 —— HAL_OK
*/
uint8_t At24c02_Write_Page(uint16_t addr, uint8_t* dat, uint16_t size)
{
    uint8_t i = 0;
    uint16_t cnt = 0;		//写入字节计数
    
    /* 对于起始地址，有两种情况，分别判断 */
    if(0 == addr % 8 )
    {
        /* 起始地址刚好是页开始地址 */
        
        /* 对于写入的字节数，有两种情况，分别判断 */
        if(size <= 8)
        {
            //写入的字节数不大于一页，直接写入
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, size, 0xFFFF);
        }
        else
        {
            //写入的字节数大于一页，先将整页循环写入
            for(i = 0;i < size/8; i++)
            {
                I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], 8, 0xFFFF);
                addr += 8;
                cnt += 8;
            }
            //将剩余的字节写入
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], size - cnt, 0xFFFF);
        }
    }
    else
    {
        /* 起始地址偏离页开始地址 */
        /* 对于写入的字节数，有两种情况，分别判断 */
        if(size <= (8 - addr%8))
        {
            /* 在该页可以写完 */
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, size, 0xFFFF);
        }
        else
        {
            /* 该页写不完 */
            //先将该页写完
            cnt += 8 - addr%8;
            I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, cnt, 0xFFFF);
            addr += cnt;
            
            //循环写整页数据
            for(i = 0;i < (size - cnt)/8; i++)
            {
                I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], 8, 0xFFFF);
                addr += 8;
                cnt += 8;
            }
            
            //将剩下的字节写入
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], size - cnt, 0xFFFF);
        }			
    }
}

/**
 * @brief		AT24C02任意地址连续读多个字节数据（连续读多个字节没有限制，直接读取即可）
 * @param		addr —— 读数据的地址（0-255）
 * @param		dat  —— 存放读出数据的地址
 * @retval		成功 —— HAL_OK
*/
uint8_t At24c02_Read_Page(uint16_t addr, uint8_t* recv_buf, uint16_t size)
{
	return I2C_Read_Data(&hi2c1, AT24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, recv_buf, size, 0xFFFF);
}


/* 在地址10处，写入一个字节 */
void At24c02_W_Byte_Test(void)
{
	if(HAL_OK == At24c02_Write_Byte(10, Send_Data))
	{
		printf("Write ok\n");
	}
	else
	{
		printf("Write fail\n");
	}
}

/* 在地址10处，读取一个字节 */
void At24c02_R_Byte_Test(void)
{
	if(HAL_OK == At24c02_Read_Byte(10, Rec_Data))
	{
		printf("Read ok, Rec_Data = %s\n", Rec_Data);

	}
	else
	{
		printf("Read fail\n");
	}
}

/* 在地址10处，写入多个数据 */
void At24c02_W_Page_Test(void)
{
	if(HAL_OK == At24c02_Write_Page(10, Send_Data, sizeof(Send_Data)))
	{
		printf("Write ok\n");
	}
	else
	{
		printf("Write fail\n");
	}
}

/* 在地址10处，读取多个字节数据 */
void At24c02_R_Page_Test(void)
{
	if(HAL_OK == At24c02_Read_Page(10, Rec_Data, sizeof(Rec_Data)))
	{
		printf("Read ok, Rec_Data = %s\n", Rec_Data);
	}
	else
	{
		printf("Read fail\n");
	}
}







