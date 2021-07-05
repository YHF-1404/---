/***
	************************************************************************************************************************
	*	@file  	software_iic.c
	*	@version V1.0
	*   @date    2020-9-3
	*	@author  反客科技
	*	@brief   软件IIC
   ************************************************************************************************************************
   *  @description
	*
	*	实验平台：反客STM32H750VBT6核心板 （型号：FK750M3-VBT6） + 4.3寸RGB液晶屏(屏幕型号RGB043M1-480*272)
	*	淘宝地址：https://shop212360197.taobao.com
	*	QQ交流群：536665479
	*		
	*
	************************************************************************************************************************
***/

#include "aht15_iic.h"  

/*-------------------------------------------- IO口操作 ---------------------------------------------------*/   

#define software_IIC_SCL(a)	if (a)	\
										HAL_GPIO_WritePin(MLX_SCL_GPIO_Port, MLX_SCL_Pin, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(MLX_SCL_GPIO_Port, MLX_SCL_Pin, GPIO_PIN_RESET)	

#define software_IIC_SDA(a)	if (a)	\
										HAL_GPIO_WritePin(MLX_SDA_GPIO_Port, MLX_SDA_Pin, GPIO_PIN_SET); \
									else		\
										HAL_GPIO_WritePin(MLX_SDA_GPIO_Port, MLX_SDA_Pin, GPIO_PIN_RESET)		


/*****************************************************************************************
*	函 数 名: software_IIC_Delay
*	入口参数: a - 延时时间
*	返 回 值: 无
*	函数功能: 简单延时函数
*	说    明: 为了移植的简便性且对延时精度要求不高，所以不需要使用定时器做延时
******************************************************************************************/

void software_IIC_Delay(uint32_t a)
{
	volatile uint16_t i;
	while (a --)				
	{
		for (i = 0; i < 8; i++);
	}
}


/*****************************************************************************************
*	函 数 名: software_IIC_Start
*	入口参数: 无
*	返 回 值: 无
*	函数功能: IIC起始信号
*	说    明: 在SCL处于高电平期间，SDA由高到低跳变为起始信号
******************************************************************************************/

void software_IIC_Start(void)
{
	software_IIC_SDA(1);		
	software_IIC_SCL(1);
	software_IIC_Delay(IIC_DelayVaule);
	
	software_IIC_SDA(0);
	software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SCL(0);
	software_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	函 数 名: software_IIC_Stop
*	入口参数: 无
*	返 回 值: 无
*	函数功能: IIC停止信号
*	说    明: 在SCL处于高电平期间，SDA由低到高跳变为起始信号
******************************************************************************************/

void software_IIC_Stop(void)
{
	// software_IIC_Delay(IIC_DelayVaule);
	// software_IIC_SCL(0);
	software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SDA(0);
	software_IIC_Delay(IIC_DelayVaule);
	
	software_IIC_SCL(1);
	software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SDA(1);
	software_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	函 数 名: software_IIC_ACK
*	入口参数: 无
*	返 回 值: 无
*	函数功能: IIC应答信号
*	说    明: 在SCL为高电平期间，SDA引脚输出为低电平，产生应答信号
******************************************************************************************/

void software_IIC_ACK(void)
{
	// software_IIC_SCL(0);
	// software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SDA(0);
	software_IIC_Delay(IIC_DelayVaule);	
	software_IIC_SCL(1);
	software_IIC_Delay(IIC_DelayVaule);
	
	software_IIC_SCL(0);		// SCL输出低时，SDA应立即拉高，释放总线
	software_IIC_SDA(1);		
	
	software_IIC_Delay(IIC_DelayVaule);
	// software_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	函 数 名: software_IIC_NoACK
*	入口参数: 无
*	返 回 值: 无
*	函数功能: IIC非应答信号
*	说    明: 在SCL为高电平期间，若SDA引脚为高电平，产生非应答信号
******************************************************************************************/

void software_IIC_NoACK(void)
{
	// software_IIC_SCL(0);	
	// software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SDA(1);
	software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SCL(1);
	software_IIC_Delay(IIC_DelayVaule);
	
	software_IIC_SCL(0);
	software_IIC_Delay(IIC_DelayVaule);
	//software_IIC_Delay(IIC_DelayVaule);
}

/*****************************************************************************************
*	函 数 名: software_IIC_WaitACK
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 等待接收设备发出应答信号
*	说    明: 在SCL为高电平期间，若检测到SDA引脚为低电平，则接收设备响应正常
******************************************************************************************/

uint8_t software_IIC_WaitACK(void)
{
	//software_IIC_SDA(1);
	//software_IIC_Delay(IIC_DelayVaule);
	software_IIC_SCL(0);
	software_IIC_Delay(IIC_DelayVaule);	
	
	if( HAL_GPIO_ReadPin(MLX_SDA_GPIO_Port,MLX_SDA_Pin) != 0) //判断设备是否有做出响应		
	{
		software_IIC_SCL(1);	
		software_IIC_Delay( IIC_DelayVaule );		
		return ACK_ERR;	//无应答
	}
	else
	{
		software_IIC_SCL(1);	
		software_IIC_Delay( IIC_DelayVaule );	
		software_IIC_SCL(0);	
		software_IIC_Delay( IIC_DelayVaule );	
		software_IIC_Delay( IIC_DelayVaule );	
		return ACK_OK;	//应答正常
	}
}

/*****************************************************************************************
*	函 数 名:	software_IIC_WriteByte
*	入口参数:	IIC_Data - 要写入的8位数据
*	返 回 值:	ACK_OK  - 设备响应正常
*          	ACK_ERR - 设备响应错误
*	函数功能:	写一字节数据
*	说    明:高位在前
******************************************************************************************/

uint8_t software_IIC_WriteByte(uint8_t IIC_Data)
{
	uint8_t i;
	software_IIC_Delay( IIC_DelayVaule );	
	for (i = 0; i < 8; i++)
	{
		software_IIC_SDA(IIC_Data & 0x80);
		
		software_IIC_Delay( IIC_DelayVaule );
		software_IIC_SCL(1);
		software_IIC_Delay( IIC_DelayVaule );
		software_IIC_SCL(0);		
		if(i == 7)
		{
			software_IIC_SDA(1);			
		}
		IIC_Data <<= 1;
	}

	return software_IIC_WaitACK(); //等待设备响应
}

/*****************************************************************************************
*	函 数 名:	software_IIC_ReadByte
*	入口参数:	ACK_Mode - 响应模式，输入1则发出应答信号，输入0发出非应答信号
*	返 回 值:	ACK_OK  - 设备响应正常
*          	ACK_ERR - 设备响应错误
*	函数功能:读一字节数据
*	说    明:1.高位在前
*				2.应在主机接收最后一字节数据时发送非应答信号
******************************************************************************************/

uint8_t software_IIC_ReadByte(uint8_t ACK_Mode)
{
	uint8_t IIC_Data = 0;
	uint8_t i = 0;
	
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		software_IIC_Delay( IIC_DelayVaule );		
		software_IIC_SCL(1);
		software_IIC_Delay( IIC_DelayVaule );
		IIC_Data |= (HAL_GPIO_ReadPin(MLX_SDA_GPIO_Port,MLX_SDA_Pin) & 0x01);	
		software_IIC_SCL(0);

	}
	
	if ( ACK_Mode == 1 )				//	应答信号
		software_IIC_ACK();
	else
		software_IIC_NoACK();		 	// 非应答信号
	
	return IIC_Data; 
}

/********************************************************************************************/
