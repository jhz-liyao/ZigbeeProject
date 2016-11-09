#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "DemoApp.h"
#include "DebugTrace.h"

#if !defined( WIN32 ) || defined( ZBIT )
  #include "OnBoard.h"
#endif
 
/* HAL */
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h" 
#include "Tool.h"  
#include "Protocol.h"
#include "ProtocolFrame.h"
#include "LOG.h"
#include "ModuleManager.h"
#include "WaterMachineDriver.h"

#define APP_SEND_MSG_TIMEOUT   1000     
#define HEARTBEAT_CHECK_TIMEOUT 3000
#define HEARTBEAT_SEND_TIMEOUT  1000
#define APP_SEND_MSG_EVT                     (BV(0))
#define WATER_MACHINE_STATE_CHECK_EVT             (BV(1))
#define HEARTBEAT_CHECK_EVT                       (BV(2))
#define HEARTBEAT_SEND_EVT                       (BV(3))

/*********************************************************************
 * GLOBAL VARIABLES
 */ 
const cId_t App_ClusterList[APP_MAX_CLUSTERS] = { APP_CLUSTERID };//簇列表

const SimpleDescriptionFormat_t App_SimpleDesc =       //简单描述符
{
  APP_ENDPOINT,              //  int Endpoint;
  APP_PROFID,                //  uint16 AppProfId[2];
  APP_DEVICEID,              //  uint16 AppDeviceId[2];
  APP_DEVICE_VERSION,        //  int   AppDevVer:4;
  APP_FLAGS,                 //  int   AppFlags:4;
  APP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)App_ClusterList,  //  byte *pAppInClusterList;
  APP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)App_ClusterList   //  byte *pAppInClusterList;
};

endPointDesc_t App_epDesc; //端点描述符
/*********************************************************************
 * 本地变量
 */
byte App_TaskId; 
devStates_t App_NwkState; 
byte App_TransID; 
afAddrType_t App_DstAddr; 
 
PROTOCOL_T protocol = {0}; 
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void App_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void App_HandleKeys( byte shift, byte keys );
static void App_ReceiveHandle( afIncomingMSGPacket_t *pckt );
static void App_SendHandle( void );

 
void SendHandle(uint8_t* data, uint8_t len){
  
  App_DstAddr.addr.shortAddr = 0x00;
  //Protocol_Printf(data, len);
  if ( AF_DataRequest( &App_DstAddr, &App_epDesc,
                       APP_CLUSTERID,
                       len,
                       (byte *)data,
                       &App_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );
}
/****************************************************
*      函数名: App_Init
*        功能: 应用初始化
*        参数: 应用层任务ID 
****************************************************/
void App_Init( uint8 task_id ){
  
  App_TaskId = task_id;
  App_NwkState = DEV_INIT;
  App_TransID = 0; 

  App_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  App_DstAddr.endPoint = APP_ENDPOINT;
  App_DstAddr.addr.shortAddr = 0;
 
  App_epDesc.endPoint = APP_ENDPOINT;
  App_epDesc.task_id = &App_TaskId;
  App_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&App_SimpleDesc;
  App_epDesc.latencyReq = noLatencyReqs;
 
  afRegister( &App_epDesc );
 
  RegisterForKeys( App_TaskId );
 
  Device_Info();
 

//  ZDO_RegisterForZDOMsg( App_TaskId, End_Device_Bind_rsp );
//  ZDO_RegisterForZDOMsg( App_TaskId, Match_Desc_req );
//  ZDO_RegisterForZDOMsg( App_TaskId, Match_Desc_rsp );
 
  Log_Init();
  ProtocolFrame_Init();
  WaterMachine_Init();
  osal_start_timerEx( App_TaskId, WATER_MACHINE_STATE_CHECK_EVT, 50 );
  
  
}


/****************************************************
*      函数名: App_GetTaskID
*      功能: 获得APP层的ID
****************************************************/
uint8* App_GetTaskID(){
    return (uint8*)&App_TaskId;
}

/****************************************************
*      函数名: App_GetepDesc
*      功能: 获得APP层的简单描述符
****************************************************/
endPointDesc_t App_GetepDesc(){
    return App_epDesc;
}


/****************************************************
*      函数名: App_HandleKeys
*        功能: 按键事件处理函数
****************************************************/
void App_HandleKeys( uint8 shift, uint8 keys )
{ 
  if ( shift ){
    if ( keys & HAL_KEY_SW_1 ) {}
    if ( keys & HAL_KEY_SW_2 ) {}
    if ( keys & HAL_KEY_SW_3 ) {}
    if ( keys & HAL_KEY_SW_4 ) {}
  }else{
    if ( keys & HAL_KEY_SW_1 ){ 
      printf("HAL_KEY_SW_1\r\n"); 
    }

    if ( keys & HAL_KEY_SW_2 ){
      printf("HAL_KEY_SW_2\r\n"); 
    } 
    if ( keys & HAL_KEY_SW_3 ){ } 
    if ( keys & HAL_KEY_SW_4 ){ }
  }
}

/****************************************************
*      函数名: App_ReceiveHandle
*        功能: 数据接收处理函数
****************************************************/ 
static void App_ReceiveHandle( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId ){
    case APP_CLUSTERID:  
      printf("收到数据:");
      Protocol_Printf(pkt->cmd.Data, pkt->cmd.DataLength);
      if(pkt->cmd.DataLength > 1)
        UART1_Resolver->Protocol_Put(UART1_Resolver,pkt->cmd.Data, pkt->cmd.DataLength);
      HeartBeat_Set(COORDINATOR_MODULE);
      break;
  }
}

void SendHearBeat(void){
  static uint8_t cnt = 0;  
  PROTOCOL_T protocol = {0}; 
  
  uint16 ShortAddr = NLME_GetShortAddr(); 
  protocol.Heartbeat_P.para1 = cnt++;
  protocol.Heartbeat_P.para2 = ShortAddr >> 8;
  protocol.Heartbeat_P.para3 = ShortAddr & 0xff;
  Protocol_Send(HEARTBEAT_PROTOCOL, &protocol, sizeof(protocol.Heartbeat_P));
}

/****************************************************
*      函数名: App_SendHandle
*        功能: 数据发送函数
****************************************************/ 
static void App_SendHandle( void )
{
  if(App_NwkState != DEV_END_DEVICE)
    return; 
  static uint8_t cnt = 0;  
  uint16 Short_Addr = NLME_GetShortAddr();
  protocol.Heartbeat_P.para1 = cnt++;
  protocol.Heartbeat_P.para2 = Short_Addr >> 8;
  protocol.Heartbeat_P.para3 = Short_Addr & 0xff;
  Protocol_Send(HEARTBEAT_PROTOCOL, &protocol, sizeof(protocol.Heartbeat_P));
  printf("本机地址：%X,%X\r\n",Short_Addr >> 8, Short_Addr & 0x00ff);
//  if ( AF_DataRequest( &App_DstAddr, &App_epDesc, APP_CLUSTERID,
//                       10, (byte *)&shortaddr,
//                       &App_TransID, AF_ACK_REQUEST, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );
  
  
//  Protocol_Send(HEARTBEAT_PROTOCOL, &protocol, sizeof(protocol.Heartbeat_P)); 
//  Protocol_Send(STATE_PROTOCOL, &protocol, sizeof(protocol.State_P));
//  Protocol_Send(ACK_PROTOCOL, &protocol, sizeof(protocol.Ack_P));  
    
//  LED_Flag = ~LED_Flag;
//  P1_0 = LED_Flag;
//  P1_1 = LED_Flag;
//  P1_4 = LED_Flag;
//  //P0_4 = LED_Flag;
//  P0_6 = LED_Flag;
  printf("温控输入:%d\r\n", P1_3); 
}

void BrodcastData(uint8* data,uint8 len){
  static uint8_t cnt = 0; 
  afAddrType_t Broadcast_DstAddr;  //地址描述符
  endPointDesc_t Broadcast_epDesc; //端点描述符 
  
  Broadcast_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  Broadcast_DstAddr.endPoint = APP_ENDPOINT;
  Broadcast_DstAddr.addr.shortAddr = 0xFFFF;
 
  Broadcast_epDesc.endPoint = APP_ENDPOINT;
  Broadcast_epDesc.task_id = &App_TaskId;
  Broadcast_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&App_SimpleDesc;
  Broadcast_epDesc.latencyReq = noLatencyReqs;
  cnt++;
  if ( AF_DataRequest( &Broadcast_DstAddr, &Broadcast_epDesc, APP_CLUSTERID, len, (byte *)data, &App_TransID, AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );

}
 

/****************************************************
*      函数名: App_ProcessEvent
*        功能: 应用初事件处理函数
*        参数: 应用层任务ID， 事件源
****************************************************/
uint16 App_ProcessEvent( uint8 task_id, uint16 events ){
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;
 
  if ( events & SYS_EVENT_MSG ){
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( App_TaskId );
    while ( MSGpkt ){
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          App_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          App_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_DATA_CONFIRM_CMD:
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
//          byte sentEP = afDataConfirm->endpoint; (void)sentEP;
//          byte sentTransID = afDataConfirm->transID;(void)sentTransID;  
          if ( afDataConfirm->hdr.status != ZSuccess ){
            printf("发送失败\r\n");
          }
          break; 
        case AF_INCOMING_MSG_CMD:
          App_ReceiveHandle( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          App_NwkState = (devStates_t)(MSGpkt->hdr.status);
          ZdoStateChange_printf(App_NwkState); 
          if ( (App_NwkState == DEV_ZB_COORD) || (App_NwkState == DEV_ROUTER) || (App_NwkState == DEV_END_DEVICE) ){ 
            //osal_start_timerEx( App_TaskId, APP_SEND_MSG_EVT, APP_SEND_MSG_TIMEOUT ); 
            
            osal_start_timerEx( App_TaskId, HEARTBEAT_SEND_EVT , HEARTBEAT_SEND_TIMEOUT ); //发送心跳
            osal_start_timerEx( App_TaskId, HEARTBEAT_CHECK_EVT, HEARTBEAT_CHECK_TIMEOUT );//启动心跳检查
          }
          break; 
        default:
          break;
      } 
      osal_msg_deallocate( (uint8 *)MSGpkt ); 
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( App_TaskId );
    } 
    return (events ^ SYS_EVENT_MSG);
  }
  
  if ( events & APP_SEND_MSG_EVT ){ 
    App_SendHandle(); 
    osal_start_timerEx( App_TaskId, APP_SEND_MSG_EVT, APP_SEND_MSG_TIMEOUT ); 
    return (events ^ APP_SEND_MSG_EVT);
  }
  
  if( events & WATER_MACHINE_STATE_CHECK_EVT){
    State_Check();
    osal_start_timerEx( App_TaskId, WATER_MACHINE_STATE_CHECK_EVT, 50 );
    return (events ^ WATER_MACHINE_STATE_CHECK_EVT);
  }
  
  if ( events & HEARTBEAT_CHECK_EVT ){ 
    HeartBeat_Check(); 
    osal_start_timerEx( App_TaskId, HEARTBEAT_CHECK_EVT, HEARTBEAT_CHECK_TIMEOUT ); 
    return (events ^ HEARTBEAT_CHECK_EVT);
  }
  
  if ( events & HEARTBEAT_SEND_EVT ){ 
    SendHearBeat(); 
    osal_start_timerEx( App_TaskId, HEARTBEAT_SEND_EVT, HEARTBEAT_SEND_TIMEOUT ); 
    return (events ^ HEARTBEAT_SEND_EVT);
  }
  
  return 0;
}

/****************************************************
*      函数名: App_ProcessZDOMsgs
*        功能: ZDO设备对象层消息处理 
****************************************************/
static void App_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg ){
  switch ( inMsg->clusterID ){
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess ){
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp ){
          if ( pRsp->status == ZSuccess && pRsp->cnt ){
            App_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            App_DstAddr.addr.shortAddr = pRsp->nwkAddr; 
            App_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          printf("Match_Desc_rsp,addr:%X,%X\r\n", pRsp->nwkAddr >> 8, pRsp->nwkAddr & 0x00FF);
          osal_mem_free( pRsp );
        }
      }
      break;
    case Match_Desc_req:
      printf("Match_Desc_rsp\r\n");
      break;
  }
}
