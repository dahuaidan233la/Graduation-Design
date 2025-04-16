
#ifndef __OLED_NEW_H__
#define __OLED_NEW_H__

#include "main.h"


/* 宏定义 -------------------------------------------------------------------*/
/***********************   OLED 连接引脚定义  **************************/

#define scl(a) (a?HAL_GPIO_WritePin(OLED_SCL_GPIO_Port,OLED_SCL_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(OLED_SCL_GPIO_Port,OLED_SCL_Pin,GPIO_PIN_RESET))
#define sda(a) (a?HAL_GPIO_WritePin(OLED_SDA_GPIO_Port,OLED_SDA_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(OLED_SDA_GPIO_Port,OLED_SDA_Pin,GPIO_PIN_RESET))

void OLED_Clear(void) ; 
void OLED_Set_Pos(unsigned char x, unsigned char y); 
void OLED_Init(void); 
void OLED_Clear2(void) ;
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char *p);
void fill_picture(unsigned char fill_Data);
void point(unsigned char x,unsigned char y);
void Oled_ShowCHinese(unsigned char x,unsigned char y,unsigned char *p);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2);
void Oled_ShowString(unsigned char x,unsigned char y,unsigned char *chr);
void OLED_Drwa_QRCode(void);
void OLED_Drwa_NUM_Plus(unsigned char x,unsigned char num);//数字显示函数（64X64）
#endif
