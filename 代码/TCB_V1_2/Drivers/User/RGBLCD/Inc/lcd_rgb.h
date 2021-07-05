#ifndef  __LCD_RGB_H
#define	__LCD_RGB_H

#include "stm32h7xx_hal.h"

#define 	LCD_NUM_LAYERS  1	//	定义显示的层数，750可驱动两层显示


#define	ColorMode_0   LTDC_PIXEL_FORMAT_RGB565   		//定义 layer0 的颜色格式
						
/*---------------------------------------------------------- 函数声明 -------------------------------------------------------*/
/*---------------------------------------------------------- 函数声明 -------------------------------------------------------*/
	
void LCD_Init(void);

void  LCD_Clear(uint32_t color);			// 清屏函数

void  LCD_CopyBuffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height,uint32_t *color);	// 复制缓冲区到显示区
/*-------------------------------------------------------- LCD相关参数 -------------------------------------------------------*/

// #define HBP  43	// 根据屏幕的手册进行设置
// #define VBP  12
// #define HSW  1
// #define VSW  1
// #define HFP  8
// #define VFP  8

#define LCD_Width     	480				//	LCD的像素长度
#define LCD_Height    	272				//	LCD的像素宽度
#define LCD_MemoryAdd   0x24000000 		//	显存的起始地址  

// layer0 每个像素所占字节

#if ( ColorMode_0 == LTDC_PIXEL_FORMAT_RGB565 || ColorMode_0 == LTDC_PIXEL_FORMAT_ARGB1555 || ColorMode_0 ==LTDC_PIXEL_FORMAT_ARGB4444 )
	#define BytesPerPixel_0		2		//16位色模式每个像素占2字节
#elif ColorMode_0 == LTDC_PIXEL_FORMAT_RGB888
	#define BytesPerPixel_0		3		//24位色模式每个像素占3字节
#else
	#define BytesPerPixel_0		4		//32位色模式每个像素占4字节
#endif	

// layer1 每个像素所占字节 

#if LCD_NUM_LAYERS == 2

	#if ( ColorMode_1 == LTDC_PIXEL_FORMAT_RGB565 || ColorMode_1 == LTDC_PIXEL_FORMAT_ARGB1555 || LTDC_PIXEL_FORMAT_ARGB4444)
		#define BytesPerPixel_1		2		//16位色模式每个像素占2字节
	#endif	
	
	#define LCD_MemoryAdd_OFFSET   LCD_Width * LCD_Height * BytesPerPixel_0 	 //第二层的显存的偏移地址 

#endif

/*-------------------------------------------------------- LCD背光引脚 -------------------------------------------------------*/

// #define  LCD_Backlight_PIN								GPIO_PIN_15								
// #define	LCD_Backlight_PORT							GPIOD									
// #define 	GPIO_LDC_Backlight_CLK_ENABLE        	__HAL_RCC_GPIOD_CLK_ENABLE()	 	





#endif //__LCD_RGB_H

