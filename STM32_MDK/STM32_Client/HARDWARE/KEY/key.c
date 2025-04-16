#include "stm32f1xx.h"
#include "key.h"	    

//����������
uint8_t KEY_Scan(uint8_t mode)   //0�̰�   1����
{	 
	static uint8_t key_up=0;  //�������ɿ���־
	
	if(mode==1)  key_up=0; //����
	
	if(key_up==0&&(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0||HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==0||
	HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==0||HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==0))
	{
		HAL_Delay(5);//ȥ���� 	
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==GPIO_PIN_RESET)      { key_up=1; return 1; } 
		else if(HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==GPIO_PIN_RESET) { key_up=1; return 2; }
		else if(HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==GPIO_PIN_RESET) { key_up=1; return 3; }
		else if(HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==GPIO_PIN_RESET) { key_up=1; return 4; }	
	}
	else if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==1&&HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)==1&&
		    HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)==1&&HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)==1)  key_up=0;
 		
	return 0;// �ް�������   
}
