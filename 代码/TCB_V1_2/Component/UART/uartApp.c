/***
	************************************************************************************************************************
	*	@file  	uartApp.c
	*	@version V1.0
	*   @date    2021-6-20
	*	@author  YHF
	*	@brief   串口应用函数
   ************************************************************************************************************************
   	*  @description
	*   实现了串口1和串口3数据透传
	*	实现JSON数据封装和解析,按照阿里云物联网平台要求
	*	与4G LTE模块对接（uart3）
	*	最大单次传输字节数要求小于1024字节
	*	
	*
	************************************************************************************************************************
***/

#include "freertosTask.h"
#include "limits.h"//ULONG_MAX
#include "uart.h"
#include "string.h"
#include "main.h"
#include "stdio.h"
#include "cJSON.h"
#include "uartApp.h"
//#define Uart1RxCompleteFlag  0x01
setVal_Typedef setVal;

extern UART_HandleTypeDef hUart1;
extern void StartUartTask(void *argument);
extern osMessageQueueId_t usart1_rxHandle;

extern UART_HandleTypeDef hUart3;
extern void StartUart3Task(void *argument);
extern osMessageQueueId_t usart3_rxHandle;


extern void StartDataTask(void *argument);
extern osMessageQueueId_t dataHandle;
extern osThreadId_t dataTaskHandle;

static uint8_t *Uart1RxData;
static uint16_t Uart1RxCount;
TaskHandle_t Uart1TaskHandle;

static uint8_t *Uart3RxData;
static uint16_t Uart3RxCount;
TaskHandle_t Uart3TaskHandle;

void Uart1RxIRQ(uint8_t *pData,uint16_t *Count);
void Uart3RxIRQ(uint8_t *pData,uint16_t *Count);

TaskHandle_t *GetUart1TaskHandle(void)
{
 return &Uart1TaskHandle;
}
TaskHandle_t *GetUart3TaskHandle(void)
{
 return &Uart3TaskHandle;
}

/**
  * @brief  串口1交互任务
  * @param  argument: none
  * @retval none
**/
void StartUartTask(void *argument)
{
    uint8_t rx_len = 0;
	cJSON *fmt = NULL;	//定义json数据节点
	cJSON *root = NULL;	//定义json数据节点
	char cmpstr[3] = {0};
    Uart1Init(115200,Uart1RxIRQ); //串口1初始化

    for(;;)
    {
        if( osMessageQueueGet(usart1_rxHandle, &rx_len, NULL, osWaitForever)  == osOK)
        {
			memcpy(cmpstr,(char *)Uart1RxData,3);
			//根据4GLTE 数据手册 AT指令手册 "+++"为进入命令模式 "ATO"为退出命令模式
			if(strcmp(cmpstr,"+++") == 0)	//进入命令模式
			{
				osThreadSuspend(dataTaskHandle);	//暂停 （发送数据到4G LTE模块） 任务
				osDelay(50);	//等待DMA传输完成
				Uart3TxData(Uart1RxData,Uart1RxCount);
				Uart1TxData(Uart1RxData,Uart1RxCount);
			}
			else if(strcmp(cmpstr,"ATO") == 0)	//退出命令模式进入透传模式
			{
				Uart3TxData(Uart1RxData,Uart1RxCount);
				Uart1TxData(Uart1RxData,Uart1RxCount);
				osDelay(50);
				osThreadResume(dataTaskHandle);		//继续运行 （发送数据到4G LTE模块） 任务
			}
			else
			{
				Uart3TxData(Uart1RxData,Uart1RxCount);
				Uart1TxData(Uart1RxData,Uart1RxCount);
				root = cJSON_Parse((char *)Uart1RxData);	//解析json数据 具体用法可到github上查看
				if(root)
				{
					fmt = cJSON_GetObjectItem(root,"set_temper");
					if(fmt)
					{
						setVal.interTemper = fmt -> valuedouble; 
					}
					fmt = cJSON_GetObjectItem(root,"get_time");
					if(fmt)
					{
						memcpy(setVal.getTime, fmt -> valuestring, strlen(fmt -> valuestring)); 
					}
					fmt = cJSON_GetObjectItem(root,"get_address");
					if(fmt)
					{
						memcpy(setVal.getAddr, fmt -> valuestring, strlen(fmt -> valuestring)); 
					}	
					fmt = cJSON_GetObjectItem(root,"onoff_lock");
					if(fmt)
					{
						setVal.onOffLock = fmt -> valueint; 
					}

					cJSON_Delete(root);
					root = NULL;
					fmt = NULL;
				}
			}

        }
    }
}

/**
  * @brief  串口3交互任务
  * @param  argument: none
  * @retval none
**/
void StartUart3Task(void *argument)
{
    uint8_t rx_len = 0;
	cJSON *fmt = NULL;
	cJSON *root = NULL;
	cJSON *son = NULL;
    Uart3Init(115200,Uart3RxIRQ);
	DATA_TypeDef datax;
    for(;;)
    {
		//接收到数据
        if( osMessageQueueGet(usart3_rxHandle, &rx_len, NULL, osWaitForever)  == osOK)
        {
			//将接收到的数据通过串口1发送
			Uart1TxData(Uart3RxData,Uart3RxCount);
			//osDelay(100);
			//解析json数据 具体用法可到github上查看
			root = cJSON_Parse((char *)Uart3RxData);
			if(root)
			{
				son = cJSON_GetObjectItem(root,"params");
				if(son)
				{
					//解析云端下发的命令 具体格式参考aliyun物联网平台cSDK开发指南
					fmt = cJSON_GetObjectItem(son,"set_temper");
					if(fmt)
					{
						setVal.interTemper = fmt -> valuedouble; 
						datax.dataType = DOUBLE_TYPE;
						datax.name = "set_temper";
						datax.doubleData = setVal.interTemper;
					}
					fmt = cJSON_GetObjectItem(son,"get_time");
					if(fmt)
					{
						memcpy(setVal.getTime, fmt -> valuestring, strlen(fmt -> valuestring)); 
						datax.name = "get_time";
						datax.dataType = STRING_TYPE;
						memcpy(datax.strData, fmt -> valuestring, strlen(fmt -> valuestring)); 
					}
					fmt = cJSON_GetObjectItem(son,"get_address");
					if(fmt)
					{
						memcpy(setVal.getAddr, fmt -> valuestring, strlen(fmt -> valuestring)); 
					}	
					fmt = cJSON_GetObjectItem(son,"onoff_lock");
					if(fmt)
					{
						setVal.onOffLock = fmt -> valueint; 
						datax.dataType = DOUBLE_TYPE;
						datax.name = "onoff_lock";
						datax.doubleData = setVal.onOffLock;
					}
					osMessageQueuePut(dataHandle, &datax, NULL, osWaitForever);
				}

				cJSON_Delete(root);
				root = NULL;
				fmt = NULL;
			}
        }
    }
}


// 发送数据到4G LTE模块 任务
// 数据封装成阿里云物联网规定的json格式
void StartDataTask(void *argument)
{
    cJSON *headx = NULL;
    cJSON *sonx = NULL;
    DATA_TypeDef data1;
    char *string = NULL;
	uint8_t flag = 0;
	uint8_t once = 0;
	cJSON_Hooks hooks;
	hooks.malloc_fn = pvPortMalloc;
	hooks.free_fn = vPortFree;
	cJSON_InitHooks(&hooks);

    for(;;)
    {
		if(once == 0)
		{
			once = 1;
			/* 创建一个JSON数据对象(链表头结点) */
			headx = cJSON_CreateObject();
			/* 添加一个嵌套的JSON数据（添加一个链表节点） */
			sonx = cJSON_CreateObject();			
		}

		
        while( osMessageQueueGet(dataHandle, &data1, NULL, 50)  == osOK)
        {
			flag = 1;
            /* 添加JSON数据（根据接收到的数据类型自动分类添加） */
            switch(data1.dataType)
            {
                case INT32_TYPE : cJSON_AddNumberToObject(sonx, data1.name, data1.int32Data);break;
                case FLOAT_TYPE : cJSON_AddNumberToObject(sonx, data1.name, data1.floatData);break;
                case DOUBLE_TYPE: cJSON_AddNumberToObject(sonx, data1.name, data1.doubleData);break;	//data1.doubleData
                case ENUM_TYPE  : cJSON_AddNumberToObject(sonx, data1.name, data1.enumData);break;
                default  : ;break;
            }

        }
		//uint32_t ikjkokomk=osThreadGetStackSpace( dataTaskHandle );
		if(flag == 1)
		{
			flag = 0;
			once = 0;
			/* 添加一个嵌套的JSON数据（添加一个链表节点） */
			cJSON_AddItemToObject(headx, "params", sonx);
			/* 打印JSON对象(整条链表)的所有数据 */
			string = cJSON_Print(headx);

			Uart1TxData((uint8_t *)string,strlen(string));
			Uart3TxData((uint8_t *)string,strlen(string));

			cJSON_Delete(headx);
			cJSON_free(string);
			osDelay(1000);
		}

    }
}
//串口1中断 接收完成数据中断回调
void Uart1RxIRQ(uint8_t *pData,uint16_t *Count)
{
    Uart1RxData = pData;
    Uart1RxCount = *Count;
    if( osMessageQueuePut( usart1_rxHandle, Count, 0, 0 ) != pdPASS )
    {

    }
}

//串口3中断 接收完成数据中断回调
void Uart3RxIRQ(uint8_t *pData,uint16_t *Count)
{
    Uart3RxData = pData;
    Uart3RxCount = *Count;
    if( osMessageQueuePut( usart3_rxHandle, Count, 0, 0 ) != pdPASS )
    {

    }
}





