
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
  
  
  //�ն���
#define MAX_DEVICE  		6
  
  //�����붨��
#define ZIGBEE_FUN_CODE_CHECK_LAMP				0x01	//��ѯ/��Ӧ�����ն��ϵƵ�״̬
#define ZIGBEE_FUN_CODE_CTRL_LAMP				0x02	//����/��Ӧ�����ն��ϵƵ�״̬
#define ZIGBEE_FUN_CODE_CHECK_Smoke				0x03	//��ѯ/��Ӧ�����ն�����������
#define ZIGBEE_FUN_CODE_CHECK_HUMAN				0x04	//��ѯ/��Ӧ�����ն��������Ӧģ��
#define ZIGBEE_FUN_CODE_CHECK_LIGHT				0x05	//��ѯ/��Ӧ�����ն��Ϲ���������
#define ZIGBEE_FUN_CODE_CHECK_WindowCurtains		0x06      //��ѯ/��Ӧ�����ն��ϵ綯����
#define ZIGBEE_FUN_CODE_CTRL_WindowCurtains		0x07	//����/��Ӧ�����ն��ϵ綯����
#define ZIGBEE_FUN_CODE_CHECK_RFID				0x08	//��ѯ/��ӦRFID�ϱ�
#define ZIGBEE_FUN_CODE_CHECK_TEMP_HUM			0x09	//��ѯ/��Ӧ�����ն����¶Ⱥ�ʪ��
#define ZIGBEE_FUN_CODE_CHECK_Flame				0x0a	//��ѯ�����ն��ϻ��洫����״̬
#define ZIGBEE_FUN_CODE_STEP				0x0b	//����/��Ӧ�ն��ϵĲ������
#define ZIGBEE_FUN_CODE_RFID_FINDCARD			0x0c   //Ѱ��/��Ӧ�ն��ϵ�RFID
#define ZIGBEE_FUN_CODE_RFID_Conflict			0x0d   //����ͻ/��Ӧ�ն��ϵ�RFID
  
  
  //ÿһ���ն��ϵ��豸���ݽṹ
typedef struct _end_device_data_info_
{
	int addr;		//�ն˶˵�ַ
	byte LampState;//�Ƶ�״̬  TRUE:����
	bool Smoke;   //��������TRUE:������
	bool HumanState; //�����Ӧ��״̬  true:����
	bool LightSensor; //��ÿ������ true:��ǿ���Ӧ
	int WindowCurtains;//�綯��״̬,��Ϊ10��0~10
	byte rfid[4];			//rfid�ϱ�����
	byte Temperature;//	�¶�
	byte Humidity;//ʪ��
	bool Flame;  //���洫����
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
