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
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"

/* RTOS */
#if defined( IAR_ARMCM3_LM )
#include "RTOS_App.h"
#endif

#include "Tool.h" 

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

endPointDesc_t Demo_epDesc; //端点描述符
/*********************************************************************
 * 本地变量
 */
byte Demo_TaskID; 
devStates_t Demo_NwkState; 
byte Demo_TransID; 
afAddrType_t Demo_DstAddr;
  
static uint32 txMsgDelay = DEMO_SEND_MSG_TIMEOUT;



/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Demo_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void Demo_HandleKeys( byte shift, byte keys );
static void Demo_ReceiveHandle( afIncomingMSGPacket_t *pckt );
static void Demo_SendHandle( void );

#if defined( IAR_ARMCM3_LM )
static void Demo_ProcessRtosMessage( void );
#endif
 
//
//uint16 remot_ShortAddr = 0;
//uint8 rcv_cnt = 0;
//uint8 rcv_data = 0;
//static void sync_cnt(void){ 
//  printf("本机:%d\t远程:%d\r\n", rcv_cnt++, rcv_data);
//}
/****************************************************
*      函数名: Demo_Init
*        功能: 应用初始化
*        参数: 应用层任务ID 
****************************************************/
void Demo_Init( uint8 task_id ){
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
  // Update the display
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "Demo", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( Demo_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( Demo_TaskID, Match_Desc_req );
  ZDO_RegisterForZDOMsg( Demo_TaskID, Match_Desc_rsp );
  //afSetMatch(DEMO_ENDPOINT, false);

#if defined( IAR_ARMCM3_LM ) 
  RTOS_RegisterApp( task_id, DEMO_RTOS_MSG_EVT );
#endif  
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
          if ( (Demo_NwkState == DEV_ZB_COORD) || (Demo_NwkState == DEV_ROUTER) || (Demo_NwkState == DEV_END_DEVICE) ){ 
            osal_start_timerEx( Demo_TaskID, DEMO_SEND_MSG_EVT, txMsgDelay );
          }
          ZdoStateChange_printf(Demo_NwkState); 
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
    osal_start_timerEx( Demo_TaskID, DEMO_SEND_MSG_EVT, txMsgDelay ); 
    return (events ^ DEMO_SEND_MSG_EVT);
  }
    
 
    #if defined( IAR_ARMCM3_LM ) 
      if ( events & DEMO_RTOS_MSG_EVT ){ 
        Demo_ProcessRtosMessage(); 
        return (events ^ DEMO_RTOS_MSG_EVT);
      }
    #endif 
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
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp ){
          if ( pRsp->status == ZSuccess && pRsp->cnt ){
            //Demo_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            Demo_DstAddr.addr.shortAddr = pRsp->nwkAddr; 
            Demo_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
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
 
/****************************************************
*      函数名: Demo_HandleKeys
*        功能: 按键事件处理函数
****************************************************/
void Demo_HandleKeys( uint8 shift, uint8 keys )
{
  zAddrType_t dstAddr; 
  if ( shift ){
    if ( keys & HAL_KEY_SW_1 ) {}
    if ( keys & HAL_KEY_SW_2 ) {}
    if ( keys & HAL_KEY_SW_3 ) {}
    if ( keys & HAL_KEY_SW_4 ) {}
  }else{
    if ( keys & HAL_KEY_SW_1 ){
//      if(Demo_NwkState == DEV_HOLD){
//        ZDOInitDevice(0);
//      }else{
//        Meter_Leave();
//      }
      dstAddr.addrMode = AddrBroadcast;
      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
      ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                        DEMO_PROFID,
                        DEMO_MAX_CLUSTERS, (cId_t *)Demo_ClusterList,
                        DEMO_MAX_CLUSTERS, (cId_t *)Demo_ClusterList,
                        FALSE );      
      printf("HAL_KEY_SW_1\r\n");
#if defined( SWITCH1_BIND )
      keys |= HAL_KEY_SW_2;
#elif defined( SWITCH1_MATCH )
      keys |= HAL_KEY_SW_4;
#else
#endif
    }

    if ( keys & HAL_KEY_SW_2 ){
      printf("HAL_KEY_SW_2\r\n");
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate an End Device Bind Request for the mandatory endpoint
//      dstAddr.addrMode = Addr16Bit;
//      dstAddr.addr.shortAddr = 0x0000; // Coordinator
//      ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
//                            Demo_epDesc.endPoint,
//                            DEMO_PROFID,
//                            DEMO_MAX_CLUSTERS, (cId_t *)Demo_ClusterList,
//                            DEMO_MAX_CLUSTERS, (cId_t *)Demo_ClusterList,
//                            FALSE );
    }

    if ( keys & HAL_KEY_SW_3 ){}

    if ( keys & HAL_KEY_SW_4 ){
//      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
//      // Initiate a Match Description Request (Service Discovery)
//      dstAddr.addrMode = AddrBroadcast;
//      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
//      ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
//                        DEMO_PROFID,
//                        DEMO_MAX_CLUSTERS, (cId_t *)Demo_ClusterList,
//                        DEMO_MAX_CLUSTERS, (cId_t *)Demo_ClusterList,
//                        FALSE );
    }
  }
}

/****************************************************
*      函数名: Demo_ReceiveHandle
*        功能: 数据接收处理函数
****************************************************/
uint8 flag = 1;
static void Demo_ReceiveHandle( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId ){
    case DEMO_CLUSTERID: 
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_BLINK );  // Blink an LED
#if defined( LCD_SUPPORTED )
      HalLcdWriteString( (char*)pkt->cmd.Data, HAL_LCD_LINE_1 );
      HalLcdWriteStringValue( "Rcvd:", rxMsgCount, 10, HAL_LCD_LINE_2 );
#elif defined( WIN32 )
      WPRINTSTR( pkt->cmd.Data );
#endif 
      uint16 shortAddr = pkt->srcAddr.addr.shortAddr;
      printf("srcAddr:%X,%X\r\n",shortAddr >> 8,shortAddr & 0x00ff);  
//      Demo_DstAddr.addr.shortAddr = shortAddr;
//      Demo_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
//      if ( AF_DataRequest( &Demo_DstAddr, &Demo_epDesc,
//                       DEMO_CLUSTERID,
//                       2,
//                       (byte *)&shortAddr,
//                       &Demo_TransID,
//                       AF_ACK_REQUEST, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );
      break;
  }
}

/****************************************************
*      函数名: Demo_SendHandle
*        功能: 数据发送函数
****************************************************/ 
static void Demo_SendHandle( void )
{   
  //uint16* shortaddr = (uint16*)&send_cnt[1]; 
  //*shortaddr = NLME_GetShortAddr();
  uint16 shortaddr = NLME_GetShortAddr();
  if(Demo_NwkState != DEV_ROUTER)
    return;
  //Demo_DstAddr.addr.shortAddr = 0xc645;
  Demo_DstAddr.addrMode = (afAddrMode_t)AddrNotPresent;
  printf("本机地址：%X,%X\r\n",shortaddr >> 8, shortaddr & 0x00ff);
  if ( AF_DataRequest( &Demo_DstAddr, &Demo_epDesc, DEMO_CLUSTERID,
                       2, (byte *)&shortaddr,
                       &Demo_TransID, AF_ACK_REQUEST, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );
}



#if defined( IAR_ARMCM3_LM )
/*********************************************************************
 * @fn      Demo_ProcessRtosMessage
 *
 * @brief   Receive message from RTOS queue, send response back.
 *
 * @param   none
 *
 * @return  none
 */
static void Demo_ProcessRtosMessage( void )
{
  osalQueue_t inMsg;

  if ( osal_queue_receive( OsalQueue, &inMsg, 0 ) == pdPASS )
  {
    uint8 cmndId = inMsg.cmnd;
    uint32 counter = osal_build_uint32( inMsg.cbuf, 4 );

    switch ( cmndId )
    {
      case CMD_INCR:
        counter += 1;  /* Increment the incoming counter */
                       /* Intentionally fall through next case */

      case CMD_ECHO:
      {
        userQueue_t outMsg;

        outMsg.resp = RSP_CODE | cmndId;  /* Response ID */
        osal_buffer_uint32( outMsg.rbuf, counter );    /* Increment counter */
        osal_queue_send( UserQueue1, &outMsg, 0 );  /* Send back to UserTask */
        break;
      }

      default:
        break;  /* Ignore unknown command */
    }
  }
}
#endif

/*********************************************************************
 */
