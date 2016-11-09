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

#define DEMO_SEND_MSG_TIMEOUT   1000     
#define HEARTBEAT_CHECK_TIMEOUT 3000
#define HEARTBEAT_SEND_TIMEOUT  1000
#define DEMO_SEND_MSG_EVT               (BV(0)) 
#define HEARTBEAT_CHECK_EVT             (BV(1))
#define HEARTBEAT_SEND_EVT              (BV(2))
//#define INFRARED_POLL_EVT       (BV(2))   
static void Demo_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
/*********************************************************************
 * GLOBAL VARIABLES
 */ 
const cId_t Demo_ClusterList[DEMO_MAX_CLUSTERS] = { DEMO_CLUSTERID };//簇列表

const SimpleDescriptionFormat_t Demo_SimpleDesc =       //简单描述符
{
  DEMO_ENDPOINT,              //  int Endpoint;
  DEMO_PROFID,                //  uint16 AppProfId[2];
  DEMO_DEVICEID,              //  uint16 AppDeviceId[2];
  DEMO_DEVICE_VERSION,        //  int   AppDevVer:4;
  DEMO_FLAGS,                 //  int   AppFlags:4;
  DEMO_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)Demo_ClusterList,  //  byte *pAppInClusterList;
  DEMO_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)Demo_ClusterList   //  byte *pAppInClusterList;
};


/*********************************************************************
 * 本地变量
 */
byte Demo_TaskID; 
devStates_t Demo_NwkState; 
byte Demo_TransID; 
afAddrType_t Demo_DstAddr;  //地址描述符
endPointDesc_t Demo_epDesc; //端点描述符

PROTOCOL_T protocol = {0}; 


/*********************************************************************
 * LOCAL FUNCTIONS
 */ 
void Demo_ReceiveHandle( afIncomingMSGPacket_t *pckt );
void Demo_SendHandle( void );
  

void SendHandle(uint8_t* data, uint8_t len){
//  printf("准备发送:%d\r\n",len);
  Protocol_Printf(data, len);
  if ( AF_DataRequest( &Demo_DstAddr, &Demo_epDesc,
                       DEMO_CLUSTERID,
                       len,
                       (byte *)data,
                       &Demo_TransID,
                       AF_ACK_REQUEST, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );
}
/****************************************************
*      函数名: Demo_Init
*        功能: 应用初始化
*        参数: 应用层任务ID 
****************************************************/
void Demo_Init( uint8 task_id ){
  Log_Init();
  ProtocolFrame_Init(SendHandle);
  
  Demo_TaskID = task_id;
  Demo_NwkState = DEV_INIT;
  Demo_TransID = 0; 

  Demo_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  Demo_DstAddr.endPoint = DEMO_ENDPOINT;
  Demo_DstAddr.addr.shortAddr = 0;
 
  Demo_epDesc.endPoint = DEMO_ENDPOINT;
  Demo_epDesc.task_id = &Demo_TaskID;
  Demo_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&Demo_SimpleDesc;
  Demo_epDesc.latencyReq = noLatencyReqs;
 
  afRegister( &Demo_epDesc );
 
  RegisterForKeys( Demo_TaskID );
 
  Device_Info(); 

//  ZDO_RegisterForZDOMsg( Demo_TaskID, End_Device_Bind_rsp );
//  ZDO_RegisterForZDOMsg( Demo_TaskID, Match_Desc_req );
//  ZDO_RegisterForZDOMsg( Demo_TaskID, Match_Desc_rsp );
  //afSetMatch(DEMO_ENDPOINT, false);  
}



 
/****************************************************
*      函数名: Demo_HandleKeys
*        功能: 按键事件处理函数
****************************************************/
void Demo_HandleKeys( uint8 shift, uint8 keys )
{ 
  if ( shift ){
    if ( keys & HAL_KEY_SW_1 ) {}
    if ( keys & HAL_KEY_SW_2 ) {}
    if ( keys & HAL_KEY_SW_3 ) {}
    if ( keys & HAL_KEY_SW_4 ) {}
  }else{
    ModuleBoard_T *Water_Board = getModuleByModuleID(WATER_MODULE);
    if ( keys & HAL_KEY_SW_1 ){
      printf("HAL_KEY_SW_1\r\n"); 
      if(Water_Board->ModuleState == OFFLINE)
        printf("未收到客户端地址\r\n");
      else{
        protocol.Cmd_P.para1 = (uint8_t)WATERM_OPEN;
        Demo_DstAddr.addr.shortAddr = Water_Board->ShortAddr;
        printf("ShortAddr:%x,%x\r\n",Water_Board->ShortAddr>>8, Water_Board->ShortAddr&0xff);
        Protocol_Send(CMD_PROTOCOL, &protocol, sizeof(protocol.Cmd_P));
      } 
    }

    if ( keys & HAL_KEY_SW_2 ){ 
      printf("HAL_KEY_SW_2\r\n"); 
      if(Water_Board->ModuleState == OFFLINE)
        printf("未收到客户端地址\r\n");
      else{
        protocol.Cmd_P.para1 = (uint8_t)WATERM_CLOSE;
        Demo_DstAddr.addr.shortAddr = Water_Board->ShortAddr;
        Protocol_Send(CMD_PROTOCOL, &protocol, sizeof(protocol.Cmd_P));
      } 
    }

    if ( keys & HAL_KEY_SW_3 ){ }

    if ( keys & HAL_KEY_SW_4 ){ }
  }
}

/****************************************************
*      函数名: Demo_ReceiveHandle
*        功能: 数据接收处理函数
****************************************************/ 
void Demo_ReceiveHandle( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId ){
    case DEMO_CLUSTERID: 
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_BLINK );  // Blink an LED
  
      //uint16 shortAddr = pkt->srcAddr.addr.shortAddr; //remoteADDR 
      //printf("收到%X,%X发来的协议",shortAddr >> 8,shortAddr & 0x00ff);
      //Protocol_Printf(pkt->cmd.Data, pkt->cmd.DataLength);
      UART1_Resolver->Protocol_Put(UART1_Resolver,pkt->cmd.Data, pkt->cmd.DataLength); 
      break;
  }
}

/****************************************************
*      函数名: Demo_SendHandle
*        功能: 数据发送函数
****************************************************/ 
static void Demo_SendHandle( void )
{   
  if(Demo_NwkState != DEV_ZB_COORD)
    return;
  FetchProtocols(); 
}

void BroadcastHearBeat(void){
  static uint8_t cnt = 0; 
  afAddrType_t HearBeat_DstAddr;  //地址描述符
  endPointDesc_t HearBeat_epDesc; //端点描述符 
  
  HearBeat_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
  HearBeat_DstAddr.endPoint = DEMO_ENDPOINT;
  HearBeat_DstAddr.addr.shortAddr = 0xFFFF;
 
  HearBeat_epDesc.endPoint = DEMO_ENDPOINT;
  HearBeat_epDesc.task_id = &Demo_TaskID;
  HearBeat_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)&Demo_SimpleDesc;
  HearBeat_epDesc.latencyReq = noLatencyReqs;
  cnt++;
  if ( AF_DataRequest( &HearBeat_DstAddr, &HearBeat_epDesc, DEMO_CLUSTERID, 1, (byte *)&cnt, &Demo_TransID, AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );
}



/****************************************************
*      函数名: Demo_ProcessEvent
*        功能: 应用初事件处理函数
*        参数: 应用层任务ID， 事件源
****************************************************/
uint16 Demo_ProcessEvent( uint8 task_id, uint16 events ){
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;
 
  if ( events & SYS_EVENT_MSG ){
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( Demo_TaskID );
    while ( MSGpkt ){
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          Demo_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          Demo_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
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
          Demo_ReceiveHandle( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          Demo_NwkState = (devStates_t)(MSGpkt->hdr.status);
          ZdoStateChange_printf(Demo_NwkState); 
          if ( (Demo_NwkState == DEV_ZB_COORD) || (Demo_NwkState == DEV_ROUTER) || (Demo_NwkState == DEV_END_DEVICE) ){ 
            //osal_start_timerEx( Demo_TaskID, DEMO_SEND_MSG_EVT, DEMO_SEND_MSG_TIMEOUT );
            osal_start_timerEx( Demo_TaskID, HEARTBEAT_SEND_EVT , HEARTBEAT_SEND_TIMEOUT ); //发送心跳
            osal_start_timerEx( Demo_TaskID, HEARTBEAT_CHECK_EVT, HEARTBEAT_CHECK_TIMEOUT );//启动心跳检查
          }
          
          break; 
        default:
          break;
      } 
      osal_msg_deallocate( (uint8 *)MSGpkt ); 
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( Demo_TaskID );
    } 
    return (events ^ SYS_EVENT_MSG);
  }
  if ( events & DEMO_SEND_MSG_EVT ){ 
    Demo_SendHandle(); 
    osal_start_timerEx( Demo_TaskID, DEMO_SEND_MSG_EVT, DEMO_SEND_MSG_TIMEOUT ); 
    return (events ^ DEMO_SEND_MSG_EVT);
  }
  
  if ( events & HEARTBEAT_CHECK_EVT ){ 
    HeartBeat_Check(); 
    osal_start_timerEx( Demo_TaskID, HEARTBEAT_CHECK_EVT, HEARTBEAT_CHECK_TIMEOUT ); 
    return (events ^ HEARTBEAT_CHECK_EVT);
  }
  
  if ( events & HEARTBEAT_SEND_EVT ){ 
    BroadcastHearBeat(); 
    osal_start_timerEx( Demo_TaskID, HEARTBEAT_SEND_EVT, HEARTBEAT_SEND_TIMEOUT ); 
    return (events ^ HEARTBEAT_SEND_EVT);
  }
  return 0;
}


/****************************************************
*      函数名: Demo_ProcessZDOMsgs
*        功能: ZDO设备对象层消息处理 
****************************************************/
static void Demo_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg ){
  switch ( inMsg->clusterID ){
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess ){
        // Light LED
        //HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp ){
          if ( pRsp->status == ZSuccess && pRsp->cnt ){
            Demo_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            Demo_DstAddr.addr.shortAddr = pRsp->nwkAddr; 
            Demo_DstAddr.endPoint = pRsp->epList[0];
 
            printf("Match_Desc_rsp,addr:%X,%X\r\n", pRsp->nwkAddr >> 8, pRsp->nwkAddr & 0x00FF);
          }
          osal_mem_free( pRsp );
        }
      }
      break;
    case Match_Desc_req:
      printf("Match_Desc_rsp\r\n");
      break;
  }
}
