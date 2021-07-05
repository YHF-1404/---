/***
	************************************************************************************************************************************************************************************************
	*	@file  	dataCollection.h
	*	@version V1.0
	*  	@date    2021-6-15
	*	@author  YHF
	*	@brief   data collection	
   **********************************************************************************************************************************************************************************************
   *  @description
   *	里面包含了电压和电流信号的采集、温度的采集
   *	这里用到了 ADC1 的 5个通道分别对三个电压值和两个电流值进行采集 
   *    采用DMA 外设-->内存 自动搬运采集完成的数据到内存
   *   	ADC1 GPIO Configuration
   *    PA0     ------> ADC1_INP16
   *    PA2     ------> ADC1_INP14
   *    PA7     ------> ADC1_INP7
   *    PC4     ------> ADC1_INP4
   *    PC5     ------> ADC1_INP8
	*
***/

#include "dataCollection.h"
#include "freertosTask.h"
#include "uartApp.h"
#include "main.h"
#include "adc.h"
#include "aht15.h"

#define voltage1_ratio 6.4	//电压采样电阻分压倍率
#define voltage2_ratio 6.4
#define voltage3_ratio 6.4
#define eleCurrent1_ratio 6.6	//电流采样分压倍率
#define eleCurrent2_ratio 10.6

double Temper = 0;

extern void StartDataCollectionTask(void *argument);
extern osMessageQueueId_t dataHandle;
extern sensorData_TypeDef hsensor1;

/**
* @brief ADC采集任务.
* @param argument: Not used
* @retval None
*/
void StartDataCollectionTask(void *argument)
{
    DATA_TypeDef datax;
    uint32_t ADC_Value[100] = {0};  //定义采样存放位置
    uint8_t i;
	uint8_t flag = 0;   //用作初始化校准制冷模块功率的标志位
    uint32_t adc1 = 0;
    uint32_t adc2 = 0;
    uint32_t adc3 = 0;
    uint32_t adc4 = 0;
    uint32_t adc5 = 0;
    double voltage1 = 0;
    double voltage2 = 0;
    double voltage3 = 0;
    double eleCurrent1 = 0;
    double eleCurrent2 = 0;
	
	double diff_voltage1 = 0;   //用作初始化校准制冷模块数值
	double diff_voltage2 = 0;


    //double Temper = 0;
	double Humidity = 0;
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 100);  //开始DMA传输

    for(;;)
    {
        osDelay(1500);  //1.5s进行一次采样
        adc1 = 0;
        adc2 = 0;
        adc3 = 0;
        adc4 = 0;
        adc5 = 0;
        //分离5个通道数据，每个通道重复采样20次并累加
        for(i = 0; i < 100;)
        {
            adc1 += ADC_Value[i++];
            adc2 += ADC_Value[i++];
            adc3 += ADC_Value[i++];
            adc4 += ADC_Value[i++];
            adc5 += ADC_Value[i++];
        }
		//对采样数据进行封装
        //指定采样到的数据得类型
        datax.dataType = DOUBLE_TYPE;

        voltage1 = voltage1_ratio * (adc1/20)*3.3/65535; //计算平均电压
        datax.name = "BatteryVoltage";        //电池电压
        datax.doubleData = voltage1;
        //封装好的数据放入队列
        osMessageQueuePut(dataHandle, &datax, NULL, osWaitForever);

		if(flag == 0)
		{
			flag = 1;
			diff_voltage1 = eleCurrent1_ratio * (adc3/20)*3.3/65535; 
			diff_voltage2 = eleCurrent2_ratio * (adc5/20)*3.3/65535;      
		}
		
        voltage2 = voltage2_ratio * (adc2/20)*3.3/65535;
		eleCurrent1 = eleCurrent1_ratio * (adc3/20)*3.3/65535 - diff_voltage1;
        datax.name = "F_power";              //前置温控模块功率
        datax.doubleData = eleCurrent1 * voltage2;
        osMessageQueuePut(dataHandle, &datax, NULL, osWaitForever);


        voltage3 = voltage3_ratio * (adc4/20)*3.3/65535;      //后置温控模块功率
        eleCurrent2 = eleCurrent2_ratio * (adc5/20)*3.3/65535 - diff_voltage2;
        datax.name = "B_power";
        datax.doubleData = eleCurrent2 * voltage3;
        osMessageQueuePut(dataHandle, &datax, NULL, osWaitForever);

        Temper = getSensorTemper(&hsensor1);
        datax.name = "Temper";
        datax.doubleData = Temper;
        osMessageQueuePut(dataHandle, &datax, NULL, osWaitForever); 

        Humidity = getSensorHumidity(&hsensor1);
        datax.name = "Humidity";
        datax.doubleData = Humidity;
        osMessageQueuePut(dataHandle, &datax, NULL, osWaitForever);       		
    }
}

