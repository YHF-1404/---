/***
	************************************************************************************************************************
	*	@file  	aht15.c
	*	@version V1.0
	*   @date    2021-6-20
	*	@author  YHF
	*	@brief   温湿度传感器AHT15功能函数
   ************************************************************************************************************************
   *  @description
	*   获取温度和湿度值等数据
	*	应用软件模拟IIC的方式
	*
	************************************************************************************************************************
***/
#include "aht15.h"

sensorData_TypeDef hsensor1;	//传感器句柄
/**
  * @brief  获取温度数据函数.
  * @param  argument: hsensor
  * @retval 温度数值
**/
double getSensorTemper(sensorData_TypeDef *hsensor)
{
    uint8_t cmd[3] = {0};
    int64_t tempdata = 0;
	double returnval = 0;
    cmd[0] = 0xac;
    cmd[1] = 0x33;
    cmd[2] = 0x00;
    AHT15_WriteReg(AHT15_address_write, 3, cmd);
    HAL_Delay(100);
    if(AHT15_ReadReg(AHT15_address_read, 6, (uint8_t *)hsensor) == SUCCESS)
    {
        if(getSensorStatus(hsensor) == 0)
        {
			//根据数据手册得知接收到的温度数据以下列方式拼装
            tempdata =  (int64_t)(hsensor -> sensorDataBuf[2] & 0x0F ) << 16  | \
                        (int64_t)(hsensor -> sensorDataBuf[3]) << 8 | \
                        (int64_t)(hsensor -> sensorDataBuf[4]);
			returnval = (double)(((double)tempdata/(1048576.0))*200 -50); //转换成摄氏度
			return returnval;
        }
    }
	
    return -100;
}
/**
  * @brief  获取湿度数据函数.
  * @param  argument: hsensor
  * @retval 湿度数值
**/
double getSensorHumidity(sensorData_TypeDef *hsensor)
{
    uint8_t cmd[3] = {0};
    cmd[0] = 0xac;
    cmd[1] = 0x33;
    cmd[2] = 0x00;
    int64_t tempdata = 0;
	double returnval = 0;
    AHT15_WriteReg(AHT15_address_write, 3, cmd);
    HAL_Delay(100);
    if(AHT15_ReadReg(AHT15_address_read, 6, (uint8_t *)hsensor) == SUCCESS) //判断数据是否转换完成
    {
        if(getSensorStatus(hsensor) == 0)
        {
			//根据数据手册得知接收到的湿度数据以下列方式拼装
            tempdata =  (int64_t)(hsensor -> sensorDataBuf[0]) << 12  | \
                        (int64_t)(hsensor -> sensorDataBuf[1]) << 4 | \
                        (int64_t)(hsensor -> sensorDataBuf[2] & 0xF0 >> 4);
			returnval = (double)(((double)tempdata/(1048576.0))*100); //转换成湿度值
			return returnval;
        }
    }
	
    return -100;
}
//返回传感器状态
uint8_t getSensorStatus(sensorData_TypeDef *hsensor)
{
    return (hsensor -> sensorStatus & 0x80) ? 1:0;
}

/*************************************************************************************************************************************
*	函 数 名:	AHT15_WriteHandle
*	入口参数:	addr - 要操作的寄存器
*	返 回 值:	SUCCESS - 操作成功
*					ERROR	  - 操作失败
*	函数功能:	AHT15 写操作
*	说    明:	对指定的寄存器执行写操作
************************************************************************************************************************************/

uint8_t AHT15_WriteHandle_W (uint16_t addr)
{
	uint8_t status;				// 状态标志位

	software_IIC_Start();	// 启动IIC通信
    if( software_IIC_WriteByte((uint8_t)(addr >> 8)) == ACK_OK ) //写入16位地址
    {
        if( software_IIC_WriteByte((uint8_t)(addr)) != ACK_OK )
        {
            status = ERROR;	// 操作失败
        }			
    }
	status = SUCCESS;	// 操作成功
	return status;	
}

uint8_t AHT15_WriteHandle_B (uint8_t addr)
{
	uint8_t status;				// 状态标志位

	software_IIC_Start();	// 启动IIC通信
    if( software_IIC_WriteByte((uint8_t)(addr)) != ACK_OK )
    {
        status = ERROR;	// 操作失败
    }			
	status = SUCCESS;	// 操作成功
	return status;	
}
/*************************************************************************************************************************************
*	函 数 名:	AHT15_WriteReg
*	入口参数:	addr - 要写入的寄存器区域首地址
*					cnt  - 数据长度
*					value - 要写入的数据区
*	返 回 值:	SUCCESS - 操作成功
*					ERROR	  - 操作失败
*	函数功能:	AHT15 写寄存器
*	说    明:	往芯片的寄存器区写入指定长度的数据
************************************************************************************************************************************/

uint8_t AHT15_WriteReg (uint8_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	software_IIC_Start();

	if( AHT15_WriteHandle_B(addr) == SUCCESS) 	// 写入要操作的寄存器
	{
		for(i = 0 ; i < cnt; i++)			 	// 计数
		{
			software_IIC_WriteByte(value[i]);	// 写入数据
		}					
		software_IIC_Stop();		// 停止IIC通信
		status = SUCCESS;		// 写入成功
	}
	else
	{
		software_IIC_Stop();		// 停止IIC通信
		status = ERROR;		// 写入失败
	}
	return status;	
}

/*************************************************************************************************************************************
*	函 数 名:	AHT15_ReadReg
*	入口参数:	addr - 要读取的寄存器区域首地址
*					cnt  - 数据长度
*					value - 要读取的数据区
*	返 回 值:	SUCCESS - 操作成功
*					ERROR	  - 操作失败
*	函数功能:	AHT15 读寄存器
*	说    明:	从芯片的寄存器区读取指定长度的数据
************************************************************************************************************************************/

uint8_t AHT15_ReadReg (uint8_t addr, uint8_t cnt, uint8_t *value)
{
	uint8_t status;
	uint8_t i;

	status = ERROR;
	//software_IIC_Start();		// 启动IIC通信

	if( AHT15_WriteHandle_B(addr) == SUCCESS) //写入要操作的寄存器
	{
		for(i = 0 ; i < cnt; i++)	// 计数
		{
			if (i == (cnt - 1))
			{
				value[i] = software_IIC_ReadByte(0);	// 读到最后一个数据时发送 非应答信号
			}
			else
			{
				value[i] = software_IIC_ReadByte(1);	// 发送应答信号
			}
		}					
		status = SUCCESS;
	}
    else
    {
        status = ERROR;
    }
	software_IIC_Stop();	// 停止IIC通信
	return (status);	
}
