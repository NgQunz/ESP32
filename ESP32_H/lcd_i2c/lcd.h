/*
  ******************************************************************************
  * @file		stm32_gpio.h                                                              *
  * @author	Nguyen Minh Quan                                                *
  * @date		27/03/2024        
	* @ver 1.0                                                       *
  ******************************************************************************
*/
	
#ifndef __LCD_H__
#define __LCD_H__

#ifdef __cplusplus
 extern "C" {
#endif

// #include "stm32f10x.h"
// #include "stm32f10x_gpio.h"
// #include "stm32f10x_rcc.h"


void GPIO_Config(void);
void led_Blink(void);
#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/