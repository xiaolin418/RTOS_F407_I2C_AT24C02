#include "app_i2c.h"
#include "usart.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;


uint8_t Rec_Data[100] = {0};
uint8_t Send_Data[] = "ttan̷С��xaskinas89qqqqqqqqq";

/* ����д���ݵ�ʱ����Ҫ��д������ж� */
static HAL_StatusTypeDef I2C_Write_Data(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef Write_Status;
	
	Write_Status = HAL_I2C_Mem_Write(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout);
	/* ����豸�Ƿ�д����� */
	while(HAL_I2C_IsDeviceReady(hi2c, DevAddress, 5, Timeout) != HAL_OK);		//5��ָ���������5��
	
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
 * @brief		AT24C02�����ַдһ���ֽ�����
 * @param		addr ���� д���ݵĵ�ַ��0-255��
 * @param		dat  ���� ���д�����ݵĵ�ַ
 * @retval		�ɹ� ���� HAL_OK
*/
static uint8_t At24c02_Write_Byte(uint16_t addr, uint8_t* dat)
{
	return HAL_I2C_Mem_Write(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, 1, 0xFFFF);
}


/**
 * @brief		AT24C02�����ַ��һ���ֽ�����
 * @param		addr ���� �����ݵĵ�ַ��0-255��
 * @param		read_buf ���� ��Ŷ�ȡ���ݵĵ�ַ
 * @retval		�ɹ� ���� HAL_OK
*/
static uint8_t At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf)
{
	return HAL_I2C_Mem_Read(&hi2c1, AT24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, read_buf, 1, 0xFFFF);
}

/**
 * @brief		AT24C02�����ַ����д����ֽ�����
 * @param		addr ���� д���ݵĵ�ַ��0-255��
 * @param		dat  ���� ���д�����ݵĵ�ַ
 * @retval		�ɹ� ���� HAL_OK
*/
uint8_t At24c02_Write_Page(uint16_t addr, uint8_t* dat, uint16_t size)
{
    uint8_t i = 0;
    uint16_t cnt = 0;		//д���ֽڼ���
    
    /* ������ʼ��ַ��������������ֱ��ж� */
    if(0 == addr % 8 )
    {
        /* ��ʼ��ַ�պ���ҳ��ʼ��ַ */
        
        /* ����д����ֽ�����������������ֱ��ж� */
        if(size <= 8)
        {
            //д����ֽ���������һҳ��ֱ��д��
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, size, 0xFFFF);
        }
        else
        {
            //д����ֽ�������һҳ���Ƚ���ҳѭ��д��
            for(i = 0;i < size/8; i++)
            {
                I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], 8, 0xFFFF);
                addr += 8;
                cnt += 8;
            }
            //��ʣ����ֽ�д��
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], size - cnt, 0xFFFF);
        }
    }
    else
    {
        /* ��ʼ��ַƫ��ҳ��ʼ��ַ */
        /* ����д����ֽ�����������������ֱ��ж� */
        if(size <= (8 - addr%8))
        {
            /* �ڸ�ҳ����д�� */
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, size, 0xFFFF);
        }
        else
        {
            /* ��ҳд���� */
            //�Ƚ���ҳд��
            cnt += 8 - addr%8;
            I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, dat, cnt, 0xFFFF);
            addr += cnt;
            
            //ѭ��д��ҳ����
            for(i = 0;i < (size - cnt)/8; i++)
            {
                I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], 8, 0xFFFF);
                addr += 8;
                cnt += 8;
            }
            
            //��ʣ�µ��ֽ�д��
            return I2C_Write_Data(&hi2c1, AT24C02_ADDR_WRITE, addr, I2C_MEMADD_SIZE_8BIT, &dat[cnt], size - cnt, 0xFFFF);
        }			
    }
}

/**
 * @brief		AT24C02�����ַ����������ֽ����ݣ�����������ֽ�û�����ƣ�ֱ�Ӷ�ȡ���ɣ�
 * @param		addr ���� �����ݵĵ�ַ��0-255��
 * @param		dat  ���� ��Ŷ������ݵĵ�ַ
 * @retval		�ɹ� ���� HAL_OK
*/
uint8_t At24c02_Read_Page(uint16_t addr, uint8_t* recv_buf, uint16_t size)
{
	return I2C_Read_Data(&hi2c1, AT24C02_ADDR_READ, addr, I2C_MEMADD_SIZE_8BIT, recv_buf, size, 0xFFFF);
}


/* �ڵ�ַ10����д��һ���ֽ� */
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

/* �ڵ�ַ10������ȡһ���ֽ� */
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

/* �ڵ�ַ10����д�������� */
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

/* �ڵ�ַ10������ȡ����ֽ����� */
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







