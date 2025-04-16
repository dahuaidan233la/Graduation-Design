#include <stdio.h>
#include <string.h>

#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "aps_groups.h"
#include "ZDApp.h"

#include "SampleApp.h"
#include "SampleAppHw.h"

#include "OnBoard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"

#include "DHT11.h"

#define data_size   200  //��������С

#define LED1 P1_0	//����LED1ΪP10�ڿ���

//�ն˵����ݴ洢��SendBuf
char SendBuf[8]={0x24, 0x40, 0x30, 0x01, 0x02, 0x31, 0x0d, 0x0a}; //0X00 not use
bool end1Online = false;
bool end2Online = false;
bool end3Online = false;
bool end4Online = false;
bool end5Online = false;
int end1Time = 0;
int end2Time = 0;
int end3Time = 0;
int end4Time = 0;
int end5Time = 0;

#if defined(ZDO_COORDINATOR)

#else
    #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])
#endif

//---------------------------------------------------------------------
//��׼�治ͬ���ն���Ҫ�޸Ĵ�ID,����ʶ��Э���������������ݣ�ID��ͬ����
//רҵ���Զ���Flash��õ�ַ�������ն˹̼���ͬ���ʺ�����
static uint16 EndDeviceID = 0x3001; //�ն�ID���ǳ���Ҫ
//---------------------------------------------------------------------


//�ݶ�6·�ն�
//�����Э���������ڱ��������ն˵�����
//������ն�,endDevInfo[0]�����Լ�������
EndDeviceDataInfo  endDevInfo[6];

#if defined(ZDO_COORDINATOR)

#else
    static void SerialApp_SendData_Lamp(uint8 fc);
   
    static uint8 GetHuman( void );
#endif
    


/*********************************************************************
 * GLOBAL VARIABLES
 */

// This list should be filled with Application specific Cluster IDs.
const cId_t SampleApp_ClusterList[SAMPLEAPP_MAX_CLUSTERS] =
{
  COOR_RF_PERIODIC_CLUSTERID,
  END_RF_PERIODIC_CLUSTERID
};

const SimpleDescriptionFormat_t SampleApp_SimpleDesc =
{
  SAMPLEAPP_ENDPOINT,              //  int Endpoint;
  SAMPLEAPP_PROFID,                //  uint16 AppProfId[2];
  SAMPLEAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  SAMPLEAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  SAMPLEAPP_FLAGS,                 //  int   AppFlags:4;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList,  //  uint8 *pAppInClusterList;
  SAMPLEAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)SampleApp_ClusterList   //  uint8 *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in SampleApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t SampleApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 SampleApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // SampleApp_Init() is called.
devStates_t SampleApp_NwkState;

uint8 SampleApp_TransID;  // This is the unique message ID (counter)

static afAddrType_t Broadcast_DstAddr;
static afAddrType_t SampleApp_TxAddr;

aps_Group_t SampleApp_Group;

uint8 SampleAppPeriodicCounter = 0;
uint8 SampleAppFlashCounter = 0;

char HttpUart_RX[data_size];

int httpCon1 = 0;
int httpSendOK = 0;

char *did = "10001";
char *key = "aaaaaa";
int httpt = 1;

uint8 zigbee_buff_rx[data_size];   //���ͻ���
uint16 len_buff_rx=0;       //����
uint8 zigbee_id_tx[10];     //���ͻ���

//Э���������ռ�5���ն����ݵ����飬ÿ���ն�8λ
uint8 sensorData[41]={0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
                      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
                      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 
                      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
                      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30};



/*********************************************************************
 * LOCAL FUNCTIONS
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
void COOR_GW_PeriodicMessage( void );


static void SerialApp_ProcessMSGCmd( afIncomingMSGPacket_t *pkt );
static void PrintAddrInfo(uint16 shortAddr, uint8 *pIeeeAddr);
static void GetIeeeAddr(uint8 * pIeeeAddr, uint8 *pStr);
static uint8 GetDataLen(uint8 fc);

static void AfSendAddrInfo(void);

#if defined(ZDO_COORDINATOR)

#else
void END_SendPeriodicMessage( void );
 static uint8 zigbee_send(void); //�������� 
#endif

static uint8 XorCheckSum(uint8 * pBuf, uint8 len);
static uint8 SendData(uint8 addr, uint8 FC);

static void FS_Uart_CallBack(uint8 port,uint8 event);

/*********************************************************************
 * @fn      SampleApp_Init
 *
 * @brief   Initialization function for the Generic App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void SampleApp_Init( uint8 task_id )
{
  SampleApp_TaskID = task_id;
  SampleApp_NwkState = DEV_INIT;
  SampleApp_TransID = 0;
  
  halUARTCfg_t uartConfig;
  
  uartConfig.configured           = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.flowControlThreshold = 32; // 2x30 don't care - see uart driver.
  uartConfig.rx.maxBufSize        = 32;  // 2x30 don't care - see uart driver.
  uartConfig.tx.maxBufSize        = 32;  // 2x30 don't care - see uart driver.
  uartConfig.idleTimeout             = 6;   // 2x30 don't care - see uart driver.
  uartConfig.intEnable                = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.callBackFunc          = FS_Uart_CallBack;  //ָ�����ڻص�����
  HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
  HalUARTOpen (HAL_UART_PORT_1, &uartConfig);
  

  //HalUARTWrite(HAL_UART_PORT_1,"UartIni04 RR\n", sizeof("UartInitr OK\n"));
  
  
  //------------------------���ô���---------------------------------
  //MT_UartInit();                    //���ڳ�ʼ��
  //MT_UartRegisterTaskID(task_id);   //ע�ᴮ������
//  #if ( HAL_UART_DMA == 1 )
//  HalUARTWrite(HAL_UART_PORT_0,"UartIniVV d1\n", sizeof("UartInitr OK\n"));
//  #endif
//    #if ( HAL_UART_DMA == 2 )
//  HalUARTWrite(HAL_UART_PORT_0,"UartIniVV d2\n", sizeof("UartInitr OK\n"));
//  #endif
//    #if ( HAL_UART_ISR == 1 )
//  HalUARTWrite(HAL_UART_PORT_0,"UartIniVV S1\n", sizeof("UartInitr OK\n"));
//  #endif
//    #if ( HAL_UART_ISR == 2 )
//  HalUARTWrite(HAL_UART_PORT_0,"UartIniVV S2\n", sizeof("UartInitr OK\n"));
//  #endif

  
  //HalUARTWrite(HAL_UART_PORT_0, "EFFFf11", strlen("EFFFf11")); 
  

    P1DIR |= 0x01;  //P1_0����Ϊ���
    LED1 = 1;       //LED1��Ϩ��     
   

        
  Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  Broadcast_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
  Broadcast_DstAddr.addr.shortAddr = 0xFFFF;
        
  SampleApp_epDesc.endPoint = SAMPLEAPP_ENDPOINT;
  SampleApp_epDesc.task_id = &SampleApp_TaskID;
  SampleApp_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&SampleApp_SimpleDesc;
  SampleApp_epDesc.latencyReq = noLatencyReqs;
  
  afRegister( &SampleApp_epDesc );

  RegisterForKeys( SampleApp_TaskID );

  // By default, all devices start out in Group 1
  SampleApp_Group.ID = 0x0001;
  osal_memcpy( SampleApp_Group.name, "Group 1", 7  );
  aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
  

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

 #if defined ( BUILD_ALL_DEVICES )
  // The "Demo" target is setup to have BUILD_ALL_DEVICES and HOLD_AUTO_START
  // We are looking at a jumper (defined in SampleAppHw.c) to be jumpered
  // together - if they are - we will start up a coordinator. Otherwise,
  // the device will start as a router.
  if ( readCoordinatorJumper() )
    zgDeviceLogicalType = ZG_DEVICETYPE_COORDINATOR;
  else
    zgDeviceLogicalType = ZG_DEVICETYPE_ROUTER;
#endif // BUILD_ALL_DEVICES

#if defined ( HOLD_AUTO_START )
  // HOLD_AUTO_START is a compile option that will surpress ZDApp
  //  from starting the device and wait for the application to
  //  start the device.
  ZDOInitDevice(0);
#endif

#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "SampleApp", HAL_LCD_LINE_1 );
#endif
}

static void FS_Uart_CallBack(uint8 port,uint8 event)   //���ڻص�����
{   
   if(( event & ( HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT ) ) ) 
    { 
      if(port == HAL_UART_PORT_1)
      {
          uint16 len = Hal_UART_RxBufLen(HAL_UART_PORT_1); //ȡ�����ν��յ����ַ�����              
          HalUARTRead(HAL_UART_PORT_1, HttpUart_RX, len); 
          #if defined(ZDO_COORDINATOR)   //�ж��Ƿ�ΪЭ����
          
          #else                
                len_buff_rx=len;   //����                               
                memcpy(zigbee_buff_rx,HttpUart_RX,len);      //����  
                //zigbee_send();       //��������           
                sprintf((char*)&zigbee_id_tx[0],"EndDeviceID:%x\r\n",EndDeviceID);    //EndDeviceID = 0x3001; //�ն�ID
                HalUARTWrite(HAL_UART_PORT_1, zigbee_id_tx, strlen(zigbee_id_tx));                         
          #endif        
      }
    }
}

/*********************************************************************
 * @fn      SampleApp_ProcessEvent
 *
 * @brief   Generic Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        // Received when a key is pressed
        case KEY_CHANGE:
          SampleApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        // Received when a messages is received (OTA) for this endpoint
        case AF_INCOMING_MSG_CMD:
          SampleApp_MessageMSGCB( MSGpkt );
          break;

        // Received whenever the device changes state in the network
        case ZDO_STATE_CHANGE:
          SampleApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (SampleApp_NwkState == DEV_ZB_COORD)
              || (SampleApp_NwkState == DEV_ROUTER)
              || (SampleApp_NwkState == DEV_END_DEVICE) )
          {
            
            
             #if defined(ZDO_COORDINATOR) //Э����ͨ�������������̵�ַ��IEEE  
                  Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
                  Broadcast_DstAddr.endPoint = SAMPLEAPP_ENDPOINT;
                  Broadcast_DstAddr.addr.shortAddr = 0xFFFF;
                  #if UART_DEBUG           
                      PrintAddrInfo( NLME_GetShortAddr(), aExtendedAddress + Z_EXTADDR_LEN - 1);
                  #endif 
                  HalUARTWrite(HAL_UART_PORT_0,"UartIniCOOOK33\n", sizeof("UartIniCOOOK33\n"));
                
                  osal_start_timerEx( SampleApp_TaskID, COOR_GW_PERIODIC_MSG_EVT,
                  ( COOR_GW_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );				
              #else  
                  
                  AfSendAddrInfo();
                  //�����ɹ������������ϱ��¼�
                  osal_start_timerEx( SampleApp_TaskID, END_RF_PERIODIC_MSG_EVT,
                      ( END_RF_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );
              #endif
          }
          else
          {
            // Device is no longer in the network
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next - if one is available
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( SampleApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
  
  #if defined(ZDO_COORDINATOR)
  if ( events & COOR_GW_PERIODIC_MSG_EVT )
  {
      COOR_GW_PeriodicMessage();
    // ���Ͷ�ʱ���������ض�ʱ�����ն����ݵ�������
    osal_start_timerEx( SampleApp_TaskID, COOR_GW_PERIODIC_MSG_EVT,
        ( COOR_GW_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    return (events ^ COOR_GW_PERIODIC_MSG_EVT);
  }
  
  #else

  
  // END_RF_PERIODIC_MSG_EVT���ն���Ӧ�¼�
  if ( events & END_RF_PERIODIC_MSG_EVT )
  {
    // �ն˶�ʱ��������ʱ��Э������������
    END_SendPeriodicMessage();
    // osal_start_timerEx��Э��ջ�Ķ�ʱ��������ʱʱ����END_RF_PERIODIC_MSG_TIMEOUT
    // ��ͨ���ڲ��������Ҽ�Go to definition of END_RF_PERIODIC_MSG_TIMEOUT �޸�
    osal_start_timerEx( SampleApp_TaskID, END_RF_PERIODIC_MSG_EVT,
        ( END_RF_PERIODIC_MSG_TIMEOUT + (osal_rand() & 0x00FF)) );

    return (events ^ END_RF_PERIODIC_MSG_EVT);
  }
  #endif
  
  // Discard unknown events
  return 0;
}

void AfSendAddrInfo(void)
{
    uint16 shortAddr;
    uint8 strBuf[11]={0};  
    
    SampleApp_TxAddr.addrMode = (afAddrMode_t)Addr16Bit;
    SampleApp_TxAddr.endPoint = SAMPLEAPP_ENDPOINT;
    SampleApp_TxAddr.addr.shortAddr = 0x00;   
    
    shortAddr=NLME_GetShortAddr();
    
    strBuf[0] = 0x3B;                          //���͵�ַ��Э���� �����ڵ㲥
    strBuf[1] = HI_UINT16( shortAddr );        //��Ŷ̵�ַ��8λ
    strBuf[2] = LO_UINT16( shortAddr );        //��Ŷ̵�ַ��8λ
    
    osal_memcpy(&strBuf[3], NLME_GetExtAddr(), 8);
        
   if ( AF_DataRequest( &SampleApp_TxAddr, (endPointDesc_t *)&SampleApp_epDesc,
                       END_RF_PERIODIC_CLUSTERID,
                       11,
                       strBuf,
                       &SampleApp_TransID, 
                       0, 
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
     
  {
  }
  else
  {
    // Error occurred in request to send.
  }   
}

/*********************************************************************
 * Event Generation Functions
 */
/*********************************************************************
 * @fn      SampleApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void SampleApp_HandleKeys( uint8 shift, uint8 keys )
{
  (void)shift;  // Intentionally unreferenced parameter
  
  if ( keys & HAL_KEY_SW_1 )
  {
    
  }

  if ( keys & HAL_KEY_SW_2 )
  {
    /* The Flashr Command is sent to Group 1.
     * This key toggles this device in and out of group 1.
     * If this device doesn't belong to group 1, this application
     * will not receive the Flash command sent to group 1.
     */
    aps_Group_t *grp;
    grp = aps_FindGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    if ( grp )
    {
      // Remove from the group
      aps_RemoveGroup( SAMPLEAPP_ENDPOINT, SAMPLEAPP_FLASH_GROUP );
    }
    else
    {
      // Add to the flash group
      aps_AddGroup( SAMPLEAPP_ENDPOINT, &SampleApp_Group );
    }
  }
}

/*********************************************************************
 * @fn      SampleApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
void SampleApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
    uint16 shortAddr;
    uint8 *pIeeeAddr; 
    uint8 delay;
    uint8 afRxData[data_size]={0};
    uint8 afDataLen=0; 	
    
    osal_memcpy(afRxData, pkt->cmd.Data, pkt->cmd.DataLength);
    afDataLen=pkt->cmd.DataLength;
    
//    if(afDataLen>=8 && afRxData[0]=='$' && afRxData[1]=='@')
//    {
//        //HalUARTWrite(UART0, afRxData, afDataLen);
//        //parseRfData(afRxData, afDataLen);
//    }

      HalUARTWrite(HAL_UART_PORT_1, afRxData,afDataLen );   //���ڷ���   
      LED1 = !LED1;           // LED1����һ��
}

//���ض�ʱ��������ʱ�������ݵ�������
void COOR_GW_PeriodicMessage( void )
{
    //�ж��ն�1�Ƿ����ߣ�0x30��ʾ���ߣ�0x31��ʾ���ߣ����������ն��ж�ͬ��
    sensorData[ 0 ] = 0x30;
    if ( end1Online && end1Time < 4 ){
        sensorData[ 0 ] = 0x31;
    }
    end1Time = end1Time + 1;
    if ( end1Time >= 4 ){
        end1Online = false;
    }
    end1Time = end1Time > 100 ? 4 : end1Time;
    
    sensorData[ 8 ] = 0x30;
    if ( end2Online && end2Time < 4 ){
        sensorData[ 8 ] = 0x31;
    }
    end2Time = end2Time + 1;
    if ( end2Time >= 4 ){
        end2Online = false;
    }
    end2Time = end2Time > 100 ? 4 : end2Time;
    
    sensorData[ 16 ] = 0x30;
    if ( end3Online && end3Time < 4 ){
        sensorData[ 16 ] = 0x31;
    }
    end3Time = end3Time + 1;
    if ( end3Time >= 4 ){
        end3Online = false;
    }
    end3Time = end3Time > 100 ? 4 : end3Time;
    
    sensorData[ 24 ] = 0x30;
    if ( end4Online && end4Time < 4 ){
        sensorData[ 24 ] = 0x31;
    }
    end4Time = end4Time + 1;
    if ( end4Time >= 4 ){
        end4Online = false;
    }
    end4Time = end4Time > 100 ? 4 : end4Time;
    
    sensorData[ 32 ] = 0x30;
    if ( end5Online && end5Time < 4 ){
        sensorData[ 32 ] = 0x31;
    }
    end5Time = end5Time + 1;
    if ( end5Time >= 4 ){
        end5Online = false;
    }
    end5Time = end5Time > 100 ? 4 : end5Time;
    
   
    httpt += 1;
    if ( httpt == 5 )
    {
        httpt = 1;
    }
    
    if ( httpSendOK == 1 )
    {
//        HalUARTWrite(HAL_UART_PORT_0, Http_TX, strlen( Http_TX ));
//        HalUARTWrite(HAL_UART_PORT_1, Http_TX, strlen( Http_TX ));
    }
}

//ͨ����������̵�ַ IEEE
void PrintAddrInfo(uint16 shortAddr, uint8 *pIeeeAddr)
{
    uint8 strIeeeAddr[17] = {0};
    char  buff[30] = {0};    
    
    //��ö̵�ַ   
    sprintf(buff, "shortAddr:%04X   IEEE:", shortAddr);  
 
    //���IEEE��ַ
    GetIeeeAddr(pIeeeAddr, strIeeeAddr);

    //HalUARTWrite (HAL_UART_PORT_0, (uint8 *)buff, strlen(buff));
    Delay_ms(10);
    //HalUARTWrite (HAL_UART_PORT_0, strIeeeAddr, 16); 
    //HalUARTWrite (HAL_UART_PORT_0, "\n", 1);
}

void GetIeeeAddr(uint8 * pIeeeAddr, uint8 *pStr)
{
    uint8 i;
    uint8 *xad = pIeeeAddr;
    
    for (i = 0; i < Z_EXTADDR_LEN*2; xad--)
    {
        uint8 ch;
        ch = (*xad >> 4) & 0x0F;
        *pStr++ = ch + (( ch < 10 ) ? '0' : '7');
        i++;
        ch = *xad & 0x0F;
        *pStr++ = ch + (( ch < 10 ) ? '0' : '7');
        i++;
    }
}

uint8 XorCheckSum(uint8 * pBuf, uint8 len)
{
    uint8 i;
    uint8 byRet=0;
    if(len == 0)
    {
        return byRet;
    }
            
    byRet = pBuf[0];
    for(i = 1; i < len; i ++)
    {
        byRet = byRet ^ pBuf[i];
    }
    return byRet;
}

uint8 GetDataLen(uint8 fc)
{
    uint8 len=0;
    switch(fc)
    {
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        len = 1;
        break;
    }
    
    return len;
}

#if defined(ZDO_COORDINATOR)

#else

void END_SendPeriodicMessage( void )  //��ʱ����
{ 
   zigbee_send();     //��������
   LED1 = !LED1;      // LED1����һ��
}
uint8 zigbee_send(void)  //��������
{ 
    //�������ݸ�Э����
    if ( AF_DataRequest( &SampleApp_TxAddr, &SampleApp_epDesc,
                       END_RF_PERIODIC_CLUSTERID,
                       len_buff_rx,
                        zigbee_buff_rx,
                       &SampleApp_TransID,
                       0, 
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
    {
    // Successfully requested to be sent.
    }
  
   //return len_buff_rx;
}

#endif