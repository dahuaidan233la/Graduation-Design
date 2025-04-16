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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "OLED_NEW.H"
#include "key.h"
#include "string.h"
#include "key.h"
MODBUS modbus;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint16_t Temp;       //温度   数值
uint16_t Hum;        //湿度
uint16_t gz_value;        //光强
uint16_t MQ2_value;       //烟雾
uint16_t MQ135_value;     //空气质量
uint8_t oled_buff[20];     //OLED缓冲区
uint8_t page=0;          //页面
uint8_t Tx_Buf[1024];     //发送
uint8_t RS485_data[8]={0x01,0x04,0x00,0x00,0x00,0x02,0x71,0xCB};   //采集485
uint16_t time2=0;        //定时器2计数
uint16_t ID=0;           //设备ID

const char* func1="Temp";     //数据的名称
const char* func2="Hum";
const char* func3="gz_value";
const char* func4="MQ2_value";
const char* func5="MQ135_value";
const char* func6="ID";

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void Display(void);  //显示
void send_buff(void);  //数据发送
void adc(void);  //采集ADC
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// Modbus事件处理函数
void Modbus_Event(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Modbus初始化函数
void Modbus_Init(void)
{
 modbus.myadd = 0x01;  // 0x01 从机设备地址为1
 modbus.timrun = 0;     //modbus定时器停止计算
 modbus.reflag = 0;    //无数据包处理
 modbus.recount = 0;   //接收到的字节数
 modbus.timout = 0;    //超时时间 单位：ms
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
 Modbus_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);   //定时器使能
  HAL_TIM_Base_Start_IT(&htim4);   //使能定时器4   接收数据 
  OLED_Init();		 //OLED初始化
  OLED_Clear();		 //OLED清屏
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Display();  //显示
	  
	  	key=KEY_Scan(0);   //0短按   1长按
	  	if(key==4)
		{
	    	page=~page;      //页面
			OLED_Clear();		 //OLED清屏
		}
		
       if(time2>=2)       //定时器2计数 2秒
	   {     		   
		  HAL_UART_Transmit_DMA(&huart1, RS485_data, sizeof(RS485_data));  //采集RS485  温湿度
		  adc();   //采集ADC 
		  send_buff();  //数据发送 
	      time2=0;
	   }
	  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint16_t ADC_Read(uint32_t Channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = Channel;                                         /* ?? */
	sConfig.Rank = ADC_REGULAR_RANK_1;                              
	sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;                  /* ???? */
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)             
	{
		Error_Handler();
	}
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	return (uint16_t)HAL_ADC_GetValue(&hadc1);
}
void adc(void)  //采集ADC
{	
   gz_value=4096-ADC_Read(ADC_CHANNEL_4);        //采集光强
   MQ2_value=ADC_Read(ADC_CHANNEL_5);        //采集烟雾
   MQ135_value=ADC_Read(ADC_CHANNEL_6);      //采集空气质量
}

void Display(void)  //显示
{
	Oled_ShowCHinese(28,0,(uint8_t *)"终端");
	
		sprintf ((char *)oled_buff,"ID:%d",ID);       //设备ID
	Oled_ShowString(75,0,oled_buff);      //	
	
	
	Oled_ShowCHinese(0,2,(uint8_t *)"温度");
	sprintf ((char *)oled_buff,": %d C ",Temp);
	Oled_ShowString(32,2,oled_buff);      //	 
	
	if(page==0)
	{	
		Oled_ShowCHinese(0,4,(uint8_t *)"湿度");
		sprintf ((char *)oled_buff,": %d %% ",Hum);
		Oled_ShowString(32,4,oled_buff);      //		
			
		Oled_ShowCHinese(0,6,(uint8_t *)"烟雾");
		sprintf ((char *)oled_buff,":%4dppm",MQ2_value);
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

void send_buff(void)  //数据发送
{
   static uint8_t send_key=0;
   if(ID!=0)	//ID不为0  发生数据
   {	
	   if(send_key==0)
	   {
		  sprintf((char*)&Tx_Buf[0],"Zigbee:{\"%s\":%d,\"%s\":%d,\"%s\":%d,\"%s\":%d}\r\n",
		  func1,Temp,func2,Hum,func3,gz_value,func4,MQ2_value);
		  HAL_UART_Transmit_DMA(&huart3, &Tx_Buf[0],strlen((char*)Tx_Buf));   //发送
		  send_key=1;
       }
	   else
	   {
		  send_key=0;
		  sprintf((char*)&Tx_Buf[0],"Zigbee:{\"%s\":%d,\"%s\":%d}\r\n",func5,MQ135_value,func6,ID);
		  HAL_UART_Transmit_DMA(&huart3, &Tx_Buf[0],strlen((char*)Tx_Buf));   //发送
	   }
    }
   else 	
   { 
	  sprintf((char*)&Tx_Buf[0],"ID:\r\n");    //获取ID
	  HAL_UART_Transmit_DMA(&huart3, &Tx_Buf[0],strlen((char*)Tx_Buf));   //发送 
  }
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
 if(htim->Instance==TIM2)
 {
    time2++;       //定时器2计数
 }
 if(htim->Instance==TIM4)
 {
	if(modbus.timrun != 0)//运行时间！=0表明
	 {
		modbus.timout++;
		if(modbus.timout >=35)  //大于1750us
		{
		  modbus.timrun = 0;
		  modbus.reflag = 1;     //接收数据完毕			 
		  modbus.timout = 0;     //超时时间 单位：ms
		  Modbus_Event();        //本机作为从机使用时	 
		}	
	 }
	 
 }
}

//事件处理函数
void Modbus_Event(void)
{
	if(modbus.rcbuf[1]==1&&modbus.rcbuf[2]==4&&modbus.rcbuf[3]==4)
	{
      Temp=(modbus.rcbuf[4]*256+modbus.rcbuf[5])*0.1; //对寄存器一次写入   //温度
      Hum=(modbus.rcbuf[6]*256+modbus.rcbuf[7])*0.1; //对寄存器一次写入    //湿度 
	}

   modbus.recount = 0;//接收计数清零   
   modbus.reflag = 0; //接收标志清零
}

//int mCRC16(uint8_t *data, uint16_t len) // CRC校验
//{
//    uint8_t i    = 0;
//    uint16_t crc = UINT16_MAX;

//    while (len--)
//        for (crc ^= *(data++), i = 0; i < 8; i++) crc = (crc & 0x0001) ? (crc >> 1) ^ 0xA001 : crc >> 1;
//    return crc;
//}
		
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
