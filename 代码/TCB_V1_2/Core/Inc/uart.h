#ifndef __Uart_H_
#define __Uart_H_

#include "stdint.h"

void Uart1Init(uint32_t BaudRate,void (*RxCompleteCallback)(uint8_t *pData,uint16_t *Count));
void Uart1TxData(uint8_t *pData,uint16_t Count);

void Uart3Init(uint32_t BaudRate,void (*RxCompleteCallback)(uint8_t *pData,uint16_t *Count));
void Uart3TxData(uint8_t *pData,uint16_t Count);
#endif /* End __Uart_H_ */
