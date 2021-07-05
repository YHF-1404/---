#ifndef __UART_APP_H
#define __UART_APP_H

#include "cmsis_os.h"
#include "dataCollection.h"
typedef enum
{
    INT32_TYPE = 0,
    FLOAT_TYPE = 1,
    DOUBLE_TYPE = 2,
    ENUM_TYPE = 3,
	STRING_TYPE = 4
}DATATYPE_TypeDef;


/****
*	待封装为JSON数据格式的临时数据住的地方
*	应对多种数据类型
***/
typedef struct 
{
    DATATYPE_TypeDef dataType;
    char *name;
    int32_t int32Data;
    float floatData;
    double doubleData;
	char strData[50];
    DEVICE_STATUS_TypeDef enumData;
}DATA_TypeDef;

typedef enum
{
    OFF_LOCK = 0,
    ON_LOCK = 1
}ONOFFLOCK_TypeDef;

/****
*	各种控制参数和设定值
*	应对多种数据类型
***/
typedef struct
{
	double interTemper;		//温度设定值
	ONOFFLOCK_TypeDef onOffLock; 	//控制开关锁
	char getAddr[40];
	char getTime[25];
}setVal_Typedef;


#endif //__UART_APP_H
