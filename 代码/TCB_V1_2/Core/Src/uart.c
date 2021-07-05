/***
	************************************************************************************************************************************************************************************************
	*	@file  	uart.c
	*	@version V1.0
	*  	@date    2020-9-15
	*	@author  YHF
	*	@brief   DMA USART	
   **********************************************************************************************************************************************************************************************
   *  @description
   *	双缓冲DMA 串口
   *	这里用到了 USART1 和 USART3
   *	缓冲大小 1024B
	*
***/

#include "uart.h"
#include "stm32h7xx_hal.h"
#include "stdio.h"
#define RxBufSize   1024
UART_HandleTypeDef hUart1 = {0};
DMA_HandleTypeDef hDmaUart1Tx = {0};
DMA_HandleTypeDef hDmaUart1Rx = {0};

#define RxBufSize3   1024
UART_HandleTypeDef hUart3 = {0};
DMA_HandleTypeDef hDmaUart3Tx = {0};
DMA_HandleTypeDef hDmaUart3Rx = {0};

//uint16_t bytelen = 0 ;
//数组后边的那个限定跟你的内存分配有关 如果你的主RAM在512K的那个片内存就可以不加 这是AC6编译器用法
uint8_t RxBuf[2][RxBufSize]; //__attribute__((section (".RAM_D1")));
//数组后边的那个限定跟你的内存分配有关 如果你的主RAM在512K的那个片内存就可以不加 这是AC6编译器用法
uint8_t RxBuf3[2][RxBufSize3]; //__attribute__((section (".RAM_D1")));

void (*Uart1RxCompleteCallback)(uint8_t *pData,uint16_t *Count);
void (*Uart3RxCompleteCallback)(uint8_t *pData,uint16_t *Count);
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void Uart1Init(uint32_t BaudRate,void (*RxCompleteCallback)(uint8_t *pData,uint16_t *Count))
{
    hUart1.Instance = USART1;
    hUart1.Init.BaudRate = BaudRate;
    hUart1.Init.ClockPrescaler = UART_PRESCALER_DIV2;
    hUart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart1.Init.Mode = UART_MODE_TX_RX;
    hUart1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    hUart1.Init.OverSampling = UART_OVERSAMPLING_8;
    hUart1.Init.Parity = UART_PARITY_NONE;
    hUart1.Init.StopBits = UART_STOPBITS_1;
    hUart1.Init.WordLength = UART_WORDLENGTH_8B;
    hUart1.FifoMode = UART_FIFOMODE_DISABLE;

    if(HAL_UART_Init(&hUart1)!= HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
	  //__HAL_UART_ENABLE_IT(&hUart1, UART_IT_IDLE); //使能IDLE中断

    __HAL_RCC_DMA1_CLK_ENABLE();

    hDmaUart1Tx.Instance = DMA1_Stream0;
    hDmaUart1Tx.Init.Request = DMA_REQUEST_USART1_TX;
    hDmaUart1Tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hDmaUart1Tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hDmaUart1Tx.Init.MemInc = DMA_MINC_ENABLE;
    hDmaUart1Tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hDmaUart1Tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hDmaUart1Tx.Init.Mode = DMA_NORMAL;
    hDmaUart1Tx.Init.Priority = DMA_PRIORITY_MEDIUM;

    if(HAL_DMA_Init(&hDmaUart1Tx) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
    __HAL_LINKDMA(&hUart1,hdmatx,hDmaUart1Tx);

    hDmaUart1Rx.Instance = DMA1_Stream1;
    hDmaUart1Rx.Init.Request = DMA_REQUEST_USART1_RX;
    hDmaUart1Rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hDmaUart1Rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hDmaUart1Rx.Init.MemInc = DMA_MINC_ENABLE;
    hDmaUart1Rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hDmaUart1Rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hDmaUart1Rx.Init.Mode = DMA_NORMAL;
    hDmaUart1Rx.Init.Priority = DMA_PRIORITY_MEDIUM;

    if(HAL_DMA_Init(&hDmaUart1Rx) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
    __HAL_LINKDMA(&hUart1,hdmarx,hDmaUart1Rx);

    if(HAL_UART_Receive_DMA(&hUart1,RxBuf[0],RxBufSize) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
    __HAL_UART_ENABLE_IT(&hUart1,UART_IT_IDLE);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_SetPriority(USART1_IRQn,6,0);

    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);//Tx
    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn,8,0);

    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);//Rx
    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,7,0);

    Uart1RxCompleteCallback = RxCompleteCallback;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
 if(huart == &hUart1)//串口1
 {
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

  HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);
 }
 
 if(huart == &hUart3)//串口3
 {
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
	  while(1);
	}
  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_USART3_CLK_ENABLE();

  HAL_GPIO_Init(GPIOD,&GPIO_InitStruct);
 }
}

void Uart1TxData(uint8_t *pData,uint16_t Count)
{
 if(Count)
 {
    if(HAL_UART_Transmit_DMA(&hUart1,pData,Count) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      //while(1);
    }
 }

}

void USART1_IRQHandler(void)
{
  
 if(__HAL_UART_GET_FLAG(&hUart1,UART_FLAG_IDLE))
 {
  __HAL_UART_CLEAR_IDLEFLAG(&hUart1);
  if(Uart1RxCompleteCallback)
  {
   hUart1.RxState = HAL_UART_STATE_READY;
   hDmaUart1Rx.State = HAL_DMA_STATE_READY;
   HAL_UART_RxCpltCallback(&hUart1);
  }
 }
 else
 {
	HAL_UART_IRQHandler(&hUart1); 
 }
}
//extern uint8_t sschstr[20];
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint16_t count;
	if(huart == &hUart1)
	{
	   //HAL_UART_Receive_IT(&hUart1,sschstr,5);
		HAL_Delay(50);
		count = RxBufSize - __HAL_DMA_GET_COUNTER(&hDmaUart1Rx);
		//bytelen = 0;
		if(count == 0)return;
		hDmaUart1Rx.Lock = HAL_UNLOCKED;
		if(huart->pRxBuffPtr < RxBuf[1])
		{
			Uart1RxCompleteCallback(RxBuf[0],&count);
			HAL_UART_Receive_DMA(&hUart1,RxBuf[1],RxBufSize);
		}
		else
		{
			Uart1RxCompleteCallback(RxBuf[1],&count);
			HAL_UART_Receive_DMA(&hUart1,RxBuf[0],RxBufSize);
		}
		hDmaUart1Rx.Lock = HAL_LOCKED;

	}
	if(huart == &hUart3)
	{
		HAL_Delay(50);
		count = RxBufSize3 - __HAL_DMA_GET_COUNTER(&hDmaUart3Rx);
		if(count == 0)return;
		hDmaUart3Rx.Lock = HAL_UNLOCKED;
		if(huart->pRxBuffPtr < RxBuf3[1])
		{
			Uart3RxCompleteCallback(RxBuf3[0],&count);
			HAL_UART_Receive_DMA(&hUart3,RxBuf3[1],RxBufSize3);
		}
		else
		{
			Uart3RxCompleteCallback(RxBuf3[1],&count);
			HAL_UART_Receive_DMA(&hUart3,RxBuf3[0],RxBufSize3);
		}
		hDmaUart3Rx.Lock = HAL_LOCKED;

	}
}

void DMA1_Stream0_IRQHandler(void)
{
 HAL_DMA_IRQHandler(&hDmaUart1Tx);
}

void DMA1_Stream1_IRQHandler(void)
{
 HAL_DMA_IRQHandler(&hDmaUart1Rx);
}












void Uart3Init(uint32_t BaudRate,void (*RxCompleteCallback)(uint8_t *pData,uint16_t *Count))
{
    hUart3.Instance = USART3;
    hUart3.Init.BaudRate = BaudRate;
    hUart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    hUart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hUart3.Init.Mode = UART_MODE_TX_RX;
    hUart3.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    hUart3.Init.OverSampling = UART_OVERSAMPLING_16;
    hUart3.Init.Parity = UART_PARITY_NONE;
    hUart3.Init.StopBits = UART_STOPBITS_1;
    hUart3.Init.WordLength = UART_WORDLENGTH_8B;
    hUart3.FifoMode = UART_FIFOMODE_DISABLE;

    if(HAL_UART_Init(&hUart3)!= HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
	  //__HAL_UART_ENABLE_IT(&hUart1, UART_IT_IDLE); //使能IDLE中断

    __HAL_RCC_DMA1_CLK_ENABLE();

    hDmaUart3Tx.Instance = DMA1_Stream2;
    hDmaUart3Tx.Init.Request = DMA_REQUEST_USART3_TX;
    hDmaUart3Tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hDmaUart3Tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hDmaUart3Tx.Init.MemInc = DMA_MINC_ENABLE;
    hDmaUart3Tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hDmaUart3Tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hDmaUart3Tx.Init.Mode = DMA_NORMAL;
    hDmaUart3Tx.Init.Priority = DMA_PRIORITY_MEDIUM;

    if(HAL_DMA_Init(&hDmaUart3Tx) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
    __HAL_LINKDMA(&hUart3,hdmatx,hDmaUart3Tx);

    hDmaUart3Rx.Instance = DMA1_Stream3;
    hDmaUart3Rx.Init.Request = DMA_REQUEST_USART3_RX;
    hDmaUart3Rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hDmaUart3Rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hDmaUart3Rx.Init.MemInc = DMA_MINC_ENABLE;
    hDmaUart3Rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hDmaUart3Rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hDmaUart3Rx.Init.Mode = DMA_NORMAL;
    hDmaUart3Rx.Init.Priority = DMA_PRIORITY_MEDIUM;

    if(HAL_DMA_Init(&hDmaUart3Rx) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
    __HAL_LINKDMA(&hUart3,hdmarx,hDmaUart3Rx);

    if(HAL_UART_Receive_DMA(&hUart3,RxBuf3[0],RxBufSize3) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      while(1);
    }
    __HAL_UART_ENABLE_IT(&hUart3,UART_IT_IDLE);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    HAL_NVIC_SetPriority(USART3_IRQn,6,0);

    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);//Tx
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn,8,0);

    HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);//Rx
    HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,7,0);

    Uart3RxCompleteCallback = RxCompleteCallback;
}


void Uart3TxData(uint8_t *pData,uint16_t Count)
{
 if(Count)
 {
    if(HAL_UART_Transmit_DMA(&hUart3,pData,Count) != HAL_OK)
    {

      //Error_Handler(__FILE__, __LINE__);         
      //while(1);
    }
 }

}

void USART3_IRQHandler(void)
{

 if(__HAL_UART_GET_FLAG(&hUart3,UART_FLAG_IDLE))
 {
  //static uint16_t count;
  __HAL_UART_CLEAR_IDLEFLAG(&hUart3);
  if(Uart3RxCompleteCallback)
  {
   hUart3.RxState = HAL_UART_STATE_READY;
   hDmaUart3Rx.State = HAL_DMA_STATE_READY;
   HAL_UART_RxCpltCallback(&hUart3);
  }
 }
 else
  HAL_UART_IRQHandler(&hUart3); 
}


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&hUart1, (uint8_t *)&ch, 1, 0xFFFF);
 
  return ch;
}






void DMA1_Stream2_IRQHandler(void)
{
 HAL_DMA_IRQHandler(&hDmaUart3Tx);
}

void DMA1_Stream3_IRQHandler(void)
{
 HAL_DMA_IRQHandler(&hDmaUart3Rx);
}




