#ifndef __IIC_H
#define __IIC_H

//#include "stm32h7xx_hal.h"
#include "main.h"


/*------------------------------------------ IIC相关定义 -------------------------------------------------*/

#define ACK_OK  	1  			// 响应正常
#define ACK_ERR 	0				// 响应错误

// IIC通信延时，Touch_IIC_Delay()函数使用，
//	通信速度在100KHz左右
#define IIC_DelayVaule  50  	


/*--------------------------------------------- 函数声明 --------------------------------------------------*/  		
					
//void 		Touch_IIC_GPIO_Config (void);					// IIC引脚初始化
void 		software_IIC_Delay(uint32_t a);					// IIC延时函数
//void		Touch_INT_Out(void);								// INT数据脚配置为输出模式
//void		Touch_INT_In(void);								// INT数据脚配置为输入模式								
void 		software_IIC_Start(void);							// 启动IIC通信
void 		software_IIC_Stop(void);							// IIC停止信号
void 		software_IIC_ACK(void);								//	发送响应信号
void 		software_IIC_NoACK(void);							// 发送非应答信号
uint8_t 	software_IIC_WaitACK(void);						//	等待应答信号
uint8_t		software_IIC_WriteByte(uint8_t IIC_Data); 	// 写字节函数
uint8_t 	software_IIC_ReadByte(uint8_t ACK_Mode);		// 读字节函数
		
#endif //__IIC_H
