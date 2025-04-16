
#ifndef SAMPLEAPP_H
#define SAMPLEAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define SAMPLEAPP_ENDPOINT           20

#define SAMPLEAPP_PROFID             0x0F08
#define SAMPLEAPP_DEVICEID           0x0001
#define SAMPLEAPP_DEVICE_VERSION     0
#define SAMPLEAPP_FLAGS              0

#define SAMPLEAPP_MAX_CLUSTERS       2
#define SAMPLEAPP_PERIODIC_CLUSTERID 1
#define END_RF_PERIODIC_CLUSTERID    2
#define COOR_RF_PERIODIC_CLUSTERID   3
  
  
  //终端数
#define MAX_DEVICE  		6
  
  //功能码定义
#define ZIGBEE_FUN_CODE_CHECK_LAMP				0x01	//查询/响应单个终端上灯的状态
#define ZIGBEE_FUN_CODE_CTRL_LAMP				0x02	//控制/响应单个终端上灯的状态
#define ZIGBEE_FUN_CODE_CHECK_Smoke				0x03	//查询/响应单个终端上烟雾传感器
#define ZIGBEE_FUN_CODE_CHECK_HUMAN				0x04	//查询/响应单个终端上人体感应模块
#define ZIGBEE_FUN_CODE_CHECK_LIGHT				0x05	//查询/响应单个终端上光敏传感器
#define ZIGBEE_FUN_CODE_CHECK_WindowCurtains		0x06      //查询/响应单个终端上电动窗帘
#define ZIGBEE_FUN_CODE_CTRL_WindowCurtains		0x07	//控制/响应单个终端上电动窗帘
#define ZIGBEE_FUN_CODE_CHECK_RFID				0x08	//查询/响应RFID上报
#define ZIGBEE_FUN_CODE_CHECK_TEMP_HUM			0x09	//查询/响应单个终端上温度和湿度
#define ZIGBEE_FUN_CODE_CHECK_Flame				0x0a	//查询单个终端上火焰传感器状态
#define ZIGBEE_FUN_CODE_STEP				0x0b	//控制/响应终端上的步进电机
#define ZIGBEE_FUN_CODE_RFID_FINDCARD			0x0c   //寻卡/响应终端上的RFID
#define ZIGBEE_FUN_CODE_RFID_Conflict			0x0d   //防冲突/响应终端上的RFID
  
  
  //每一个终端上的设备数据结构
typedef struct _end_device_data_info_
{
	int addr;		//终端端地址
	byte LampState;//灯的状态  TRUE:开灯
	bool Smoke;   //烟雾传感器TRUE:有烟雾
	bool HumanState; //人体感应的状态  true:有人
	bool LightSensor; //光每传感器 true:有强光感应
	int WindowCurtains;//电动窗状态,分为10级0~10
	byte rfid[4];			//rfid上报数据
	byte Temperature;//	温度
	byte Humidity;//湿度
	bool Flame;  //火焰传感器
}EndDeviceDataInfo;

// Send Message Timeout
#define COOR_GW_PERIODIC_MSG_TIMEOUT   5000     // Every 5 seconds
#define END_RF_PERIODIC_MSG_TIMEOUT   1000     // Every 5 seconds

// Application Events (OSAL) - These are bit weighted definitions.
#define COOR_GW_PERIODIC_MSG_EVT       0x0001
#define END_RF_PERIODIC_MSG_EVT       0x0002
  
// Group ID for Flash Command
#define SAMPLEAPP_FLASH_GROUP                  0x0001
  
// Flash Command Duration - in milliseconds
#define SAMPLEAPP_FLASH_DURATION               1000

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Generic Application
 */
extern void SampleApp_Init( uint8 task_id );

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 SampleApp_ProcessEvent( uint8 task_id, uint16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SAMPLEAPP_H */
