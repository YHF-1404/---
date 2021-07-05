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
	*		
	*
	************************************************************************************************************************
***/
#include "aht15_iic.h"
#include "main.h"

#define AHT15_address_write 0x70
#define AHT15_address_read 0x71

typedef struct 
{
    uint8_t sensorStatus;
    uint8_t sensorDataBuf[5];
}sensorData_TypeDef;

uint8_t AHT15_ReadReg (uint8_t addr, uint8_t cnt, uint8_t *value);
uint8_t AHT15_WriteReg (uint8_t addr, uint8_t cnt, uint8_t *value);
uint8_t AHT15_WriteHandle_W (uint16_t addr);
uint8_t AHT15_WriteHandle_B (uint8_t addr);

uint8_t getSensorStatus(sensorData_TypeDef *hsensor);

double getSensorTemper(sensorData_TypeDef *hsensor);
double getSensorHumidity(sensorData_TypeDef *hsensor);
