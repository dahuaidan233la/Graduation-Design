#include "stm32f1xx.h"
#include "key.h"	    

//按键处理函数
uint8_t KEY_Scan(uint8_t mode)   //0短按   1长按
{	 
	static uint8_t key_up=0;  //按键按松开标志
	
	if(mode==1)  key_up=0; //长按
	
	if(key_up==0&&(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0||HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0||
	HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==0||HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==0))
	{
		HAL_Delay(5);//去抖动 	
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET)      { key_up=1; return 1; } 
		else if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET) { key_up=1; return 2; }
		else if(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==GPIO_PIN_RESET) { key_up=1; return 3; }
		else if(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==GPIO_PIN_RESET) { key_up=1; return 4; }	
	}
	else if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==1&&HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==1&&
		    HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==1&&HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==1)  key_up=0;
 		
	return 0;// 无按键按下   
}
