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

#include "hal_infrared.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
const cId_t Demo_ClusterList[DEMO_MAX_CLUSTERS] =
{
  DEMO_CLUSTERID
};

const SimpleDescriptionFormat_t Demo_SimpleDesc =
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

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in Demo_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t Demo_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte Demo_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // Demo_Init() is called.

devStates_t Demo_NwkState;

byte Demo_TransID;  // This is the unique message ID (counter)

afAddrType_t Demo_DstAddr;

// Number of recieved messages
static uint16 rxMsgCount;

// Time interval between sending messages
static uint32 txMsgDelay = DEMO_SEND_MSG_TIMEOUT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void Demo_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void Demo_HandleKeys( byte shift, byte keys );
static void Demo_MessageMSGCB( afIncomingMSGPacket_t *pckt );
static void Demo_SendTheMessage( void );

#if defined( IAR_ARMCM3_LM )
static void Demo_ProcessRtosMessage( void );
#endif

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Demo_Init
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
void Demo_Init( uint8 task_id )
{
  Demo_TaskID = task_id;
  Demo_NwkState = DEV_INIT;
  Demo_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  Demo_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  Demo_DstAddr.endPoint = DEMO_ENDPOINT;
  Demo_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  Demo_epDesc.endPoint = DEMO_ENDPOINT;
  Demo_epDesc.task_id = &Demo_TaskID;
  Demo_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&Demo_SimpleDesc;
  Demo_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &Demo_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( Demo_TaskID );

  UART_Config_L();
  LiyaoApp_Init();
  // Update the display
#if defined ( LCD_SUPPORTED )
  HalLcdWriteString( "Demo", HAL_LCD_LINE_1 );
#endif

  ZDO_RegisterForZDOMsg( Demo_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( Demo_TaskID, Match_Desc_rsp );

#if defined( IAR_ARMCM3_LM )
  // Register this task with RTOS task initiator
  RTOS_RegisterApp( task_id, DEMO_RTOS_MSG_EVT );
#endif
  osal_start_timerEx( Demo_TaskID, INFRARED_POLL_EVT, 500 ); 
  
}

/*********************************************************************
 * @fn      Demo_ProcessEvent
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
uint16 Demo_ProcessEvent( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;

  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent
  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( Demo_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          Demo_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          Demo_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;

          sentEP = afDataConfirm->endpoint;
          (void)sentEP;  // This info not used now
          sentTransID = afDataConfirm->transID;
          (void)sentTransID;  // This info not used now

          sentStatus = afDataConfirm->hdr.status;
          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            printf("发送失败\r\n");
            // The data wasn't delivered -- Do something
          }
          break;

        case AF_INCOMING_MSG_CMD:
          Demo_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          Demo_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (Demo_NwkState == DEV_ZB_COORD) || (Demo_NwkState == DEV_ROUTER) || (Demo_NwkState == DEV_END_DEVICE) )
          { 
            osal_start_timerEx( Demo_TaskID, DEMO_SEND_MSG_EVT, txMsgDelay );
          }
          ZdoStateChange_printf(Demo_NwkState);
          break; 
        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( Demo_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }
 
  // Send a message out - This event is generated by a timer
  //  (setup in Demo_Init()).
  if ( events & DEMO_SEND_MSG_EVT )
  { 
    Demo_SendTheMessage(); 
    osal_start_timerEx( Demo_TaskID, DEMO_SEND_MSG_EVT, txMsgDelay ); 
    return (events ^ DEMO_SEND_MSG_EVT);
  }
    
  if(events & INFRARED_POLL_EVT){ 
    hal_infrared_poll();
    osal_start_timerEx( Demo_TaskID, INFRARED_POLL_EVT, 500 ); 
    return (events ^ INFRARED_POLL_EVT);
  }
#if defined( IAR_ARMCM3_LM )
  // Receive a message from the RTOS queue
  if ( events & DEMO_RTOS_MSG_EVT )
  {
    // Process message from RTOS queue
    Demo_ProcessRtosMessage();

    // return unprocessed events
    return (events ^ DEMO_RTOS_MSG_EVT);
  }
#endif

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */

/*********************************************************************
 * @fn      Demo_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
static void Demo_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined( BLINK_LEDS )
      else
      {
        // Flash LED to show failure
        HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            Demo_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            Demo_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            Demo_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}

/*********************************************************************
 * @fn      Demo_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void Demo_HandleKeys( uint8 shift, uint8 keys )
{
  //zAddrType_t dstAddr;

  // Shift is used to make each button/switch dual purpose.
  if ( shift )
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
      printf("HAL_KEY_SW_1\r\n");
#if defined( SWITCH1_BIND )
      // We can use SW1 to simulate SW2 for devices that only have one switch,
      keys |= HAL_KEY_SW_2;
#elif defined( SWITCH1_MATCH )
      // or use SW1 to simulate SW4 for devices that only have one switch
      keys |= HAL_KEY_SW_4;
#else
      // Normally, SW1 changes the rate that messages are sent
#endif
    }

    if ( keys & HAL_KEY_SW_2 )
    {
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

    if ( keys & HAL_KEY_SW_3 )
    {
    }

    if ( keys & HAL_KEY_SW_4 )
    {
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

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      Demo_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */
uint8 flag = 1;
static void Demo_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case DEMO_CLUSTERID:
      rxMsgCount += 1;  // Count this message
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_BLINK );  // Blink an LED
#if defined( LCD_SUPPORTED )
      HalLcdWriteString( (char*)pkt->cmd.Data, HAL_LCD_LINE_1 );
      HalLcdWriteStringValue( "Rcvd:", rxMsgCount, 10, HAL_LCD_LINE_2 );
#elif defined( WIN32 )
      WPRINTSTR( pkt->cmd.Data );
#endif 
      printf("收到:%d\r\n", *pkt->cmd.Data);
      break;
  }
}

/*********************************************************************
 * @fn      Demo_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
byte send_cnt[3] = {0};
static void Demo_SendTheMessage( void )
{  
//  char theMessageData[] = "Hello World";
  uint16* shortaddr = (uint16*)&send_cnt[1]; 
  *shortaddr = NLME_GetShortAddr();
  if(Demo_NwkState != DEV_END_DEVICE)
    return;
  if ( AF_DataRequest( &Demo_DstAddr, &Demo_epDesc,
                       DEMO_CLUSTERID,
                       3,
                       (byte *)send_cnt,
                       &Demo_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    // Error occurred in request to send.
  } 
  printf("数据已发送 %d,%X %X\r\n",send_cnt[0],send_cnt[1],send_cnt[2]);
  send_cnt[0]++;  
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
