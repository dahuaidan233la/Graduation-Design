/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hal_key.h"
#include "gizwits_product.h"
#include "common.h"
#include "key.h"	 
#include "OLED_NEW.H"
DATA data;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int Temp;       //温度   数值
int Hum;        //湿度
int gz_value;        //光强
int MQ2_value;       //烟雾
int MQ135_value;     //空气质量
int ID=0;            //设备ID

uint8_t page=0;          //页面

uint16_t Send_time=0;   //发送计数
uint8_t oled_buff[20];     //OLED缓冲区

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void  Display(void);   //显示
//事件处理函数
void Modbus_Event(void);
/**
* key1 short press handle
* @param none
* @return none
*/
void key1ShortPress(void)
{
    //GIZWITS_LOG("KEY1 PRESS ,Production Mode\n");
    gizwitsSetMode(WIFI_PRODUCTION_TEST);
}

/**
* key1 long press handle
* @param none
* @return none
*/
void key1LongPress(void)
{
    //GIZWITS_LOG("KEY1 PRESS LONG ,Wifi Reset\n");
    gizwitsSetMode(WIFI_RESET_MODE);

}

/**
* key2 short press handle
* @param none
* @return none
*/
void key2ShortPress(void)
{
    //GIZWITS_LOG("KEY2 PRESS ,Soft AP mode\n");
    #if !MODULE_TYPE
    gizwitsSetMode(WIFI_SOFTAP_MODE);
    #endif
}

/**
* key2 long press handle
* @param none
* @return none
*/
void key2LongPress(void)
{
    //AirLink mode
    //GIZWITS_LOG("KEY2 PRESS LONG ,AirLink mode\n");
    #if !MODULE_TYPE
    gizwitsSetMode(WIFI_AIRLINK_MODE);
    #endif
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
uint8_t key=0;      //按键值
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_IWDG_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
    OLED_Init();		 //OLED初始化
    OLED_Clear();		 //OLED清屏
	
    Oled_ShowCHinese(20,2,(uint8_t *)"系统初始化");	
	
    timerInit();
    uartInit();
   
    userInit();
    gizwitsInit();
	HAL_TIM_Base_Start_IT(&htim4);   //使能定时器4   接收数据 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
		HAL_IWDG_Refresh (&hiwdg);       //喂狗
		key=KEY_Scan(0);   //0短按   1长按
		if(key==2) key2LongPress();   //一键配网
		else if(key==4)
		{
	    	page=~page;      //页面
			OLED_Clear();		 //OLED清屏
		}
		Display();   //显示	
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        userHandle();
		if(Send_time>=200) 
		{
		   gizwitsHandle((dataPoint_t *)&currentDataPoint);  //数据上传至机智云
			Send_time=0;   //清零
		}
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/* USER CODE BEGIN 4 */
void  Display(void)   //显示	
{
	sprintf ((char *)oled_buff,"     ");       //清该位置的屏
	Oled_ShowString(0,0,oled_buff);      //	
	Oled_ShowCHinese(40,0,(uint8_t *)"协调器");	
	sprintf ((char *)oled_buff,"     ");       //清该位置的屏
	Oled_ShowString(88,0,oled_buff);      //	
		
	Oled_ShowCHinese(0,2,(uint8_t *)"温度");
	sprintf ((char *)oled_buff,":%2d C",Temp);
	Oled_ShowString(32,2,oled_buff);      //	
	
	sprintf ((char *)oled_buff,"  ID:%2d",ID);       //设备ID
	Oled_ShowString(72,2,oled_buff);      //	 
	
	if(page==0)
	{	
		Oled_ShowCHinese(0,4,(uint8_t *)"湿度");
		sprintf ((char *)oled_buff,":%2d %%",Hum);
		Oled_ShowString(32,4,oled_buff);      //
		sprintf ((char *)oled_buff,"      "); //清该位置的屏
		Oled_ShowString(80,4,oled_buff);      //		
			
		Oled_ShowCHinese(0,6,(uint8_t *)"烟雾");
		sprintf ((char *)oled_buff,": %4d  ppm ",MQ2_value);
		Oled_ShowString(32,6,oled_buff);      //烟雾		
	}
	else 
	{
		Oled_ShowCHinese(0,4,(uint8_t *)"光照强度");
		sprintf ((char *)oled_buff,"%4dppm",gz_value);
		Oled_ShowString(66,4,oled_buff);      //光强
	
		Oled_ShowCHinese(0,6,(uint8_t *)"空气质量");
		sprintf ((char *)oled_buff,"%4dppm",MQ135_value);
		Oled_ShowString(66,6,oled_buff);      //空气质量
	}
}

//事件处理函数
void Modbus_Event(void)
{
  //数据处理
  if(sscanf((char *)data.rcbuf,"Zigbee:{\"Temp\":%d,\"Hum\":%d,\"gz_value\":%d,\"MQ2_value\":%d}",
   &Temp,&Hum,&gz_value,&MQ2_value)==4)    //获取数据
	{
	   currentDataPoint.valueTemp = Temp;//温度
	   currentDataPoint.valueHumi =Hum ;//湿度
	   currentDataPoint.valueLinht_value = gz_value;//光强
	   currentDataPoint.valueMQ2_value =MQ2_value ;//烟雾
	}
	else  if(sscanf((char *)data.rcbuf,"Zigbee:{\"MQ135_value\":%d,\"ID\":%d}",&MQ135_value,&ID)==2)    //获取数据
	{
	   currentDataPoint.valueMQ135_value = MQ135_value;//空气质量
	   currentDataPoint.valueID =ID ;// //设备ID
	}
	else printf("Zigbee_Error\r\n");
    data.recount = 0;//接收计数清零   
    data.reflag = 0; //接收标志清零
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim==&htim2)
  {
	keyHandle();
	gizTimerMs(); 
	Send_time++;    //发送计数 
  }
 if(htim->Instance==TIM4)
 {
	if(data.timrun != 0)//运行时间！=0表明
	 { 
		data.timout++;
		if(data.timout >=35) //大于1750us
		{
		  data.timrun = 0;
		  data.reflag = 1;    //接收数据完毕			 
		  data.timout = 0;    //超时时间 单位：ms
		  Modbus_Event();    //Zigbee数据处理
		}
	 }
 }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while(1) 
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
