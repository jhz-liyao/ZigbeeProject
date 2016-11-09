#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "KeyTestApp.h"
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
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

uint8 rcv_cnt = 0;
uint8 rcv_data = 0;
static void sync_cnt(void){ 
  printf("本机:%d\t远程:%d\r\n", rcv_cnt++, rcv_data);
}



/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
const cId_t KeyTestApp_ClusterList[KEYTESTAPP_MAX_CLUSTERS] =
{
  KEYTESTAPP_CLUSTERID
};

const SimpleDescriptionFormat_t KeyTestApp_SimpleDesc =
{
  KEYTESTAPP_ENDPOINT,              //  int Endpoint;
  KEYTESTAPP_PROFID,                //  uint16 AppProfId[2];
  KEYTESTAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  KEYTESTAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  KEYTESTAPP_FLAGS,                 //  int   AppFlags:4;
  KEYTESTAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)KeyTestApp_ClusterList,  //  byte *pAppInClusterList;
  KEYTESTAPP_MAX_CLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)KeyTestApp_ClusterList   //  byte *pAppInClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in KeyTestApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t KeyTestApp_epDesc;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
byte KeyTestApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // KeyTestApp_Init() is called.

devStates_t KeyTestApp_NwkState;

byte KeyTestApp_TransID;  // This is the unique message ID (counter)

afAddrType_t KeyTestApp_DstAddr;

// Number of recieved messages
static uint16 rxMsgCount;

// Time interval between sending messages
static uint32 txMsgDelay = KEYTESTAPP_SEND_MSG_TIMEOUT;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void KeyTestApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void KeyTestApp_HandleKeys( byte shift, byte keys );
static void KeyTestApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
static void KeyTestApp_SendTheMessage( void );

#if defined( IAR_ARMCM3_LM )
static void KeyTestApp_ProcessRtosMessage( void );
#endif

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      KeyTestApp_Init
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
void KeyTestApp_Init( uint8 task_id )
{
  KeyTestApp_TaskID = task_id;
  KeyTestApp_NwkState = DEV_INIT;
  KeyTestApp_TransID = 0;

  // Device hardware initialization can be added here or in main() (Zmain.c).
  // If the hardware is application specific - add it here.
  // If the hardware is other parts of the device add it in main().

  KeyTestApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  KeyTestApp_DstAddr.endPoint = KEYTESTAPP_ENDPOINT;
  KeyTestApp_DstAddr.addr.shortAddr = 0;

  // Fill out the endpoint description.
  KeyTestApp_epDesc.endPoint = KEYTESTAPP_ENDPOINT;
  KeyTestApp_epDesc.task_id = &KeyTestApp_TaskID;
  KeyTestApp_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&KeyTestApp_SimpleDesc;
  KeyTestApp_epDesc.latencyReq = noLatencyReqs;

  // Register the endpoint description with the AF
  afRegister( &KeyTestApp_epDesc );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( KeyTestApp_TaskID );

  UART_Config_L();
  LiyaoApp_Init();
  // Update the display 

  ZDO_RegisterForZDOMsg( KeyTestApp_TaskID, End_Device_Bind_rsp );
  ZDO_RegisterForZDOMsg( KeyTestApp_TaskID, Match_Desc_rsp );

#if defined( IAR_ARMCM3_LM )
  // Register this task with RTOS task initiator
  RTOS_RegisterApp( task_id, KEYTESTAPP_RTOS_MSG_EVT );
#endif
  
  
}

/*********************************************************************
 * @fn      KeyTestApp_ProcessEvent
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
uint16 KeyTestApp_ProcessEvent( uint8 task_id, uint16 events )
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
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( KeyTestApp_TaskID );
    while ( MSGpkt )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZDO_CB_MSG:
          KeyTestApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          KeyTestApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
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
          KeyTestApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
          KeyTestApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if ( (KeyTestApp_NwkState == DEV_ZB_COORD) ||
               (KeyTestApp_NwkState == DEV_ROUTER) ||
               (KeyTestApp_NwkState == DEV_END_DEVICE) )
          {
            // Start sending "the" message in a regular interval.
            osal_start_timerEx( KeyTestApp_TaskID,
                                KEYTESTAPP_SEND_MSG_EVT,
                                txMsgDelay );
          }
          ZdoStateChange_printf(KeyTestApp_NwkState);
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( KeyTestApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & RCV_CNT_EVT ){
    sync_cnt();
    osal_start_timerEx( KeyTestApp_TaskID, RCV_CNT_EVT, 1000 );
    return (events ^ RCV_CNT_EVT);
  }
  // Send a message out - This event is generated by a timer
  //  (setup in KeyTestApp_Init()).
  if ( events & KEYTESTAPP_SEND_MSG_EVT )
  {
    // Send "the" message
    KeyTestApp_SendTheMessage();

    // Setup to send message again
    osal_start_timerEx( KeyTestApp_TaskID,
                        KEYTESTAPP_SEND_MSG_EVT,
                        txMsgDelay );

    // return unprocessed events
    return (events ^ KEYTESTAPP_SEND_MSG_EVT);
  }

#if defined( IAR_ARMCM3_LM )
  // Receive a message from the RTOS queue
  if ( events & KEYTESTAPP_RTOS_MSG_EVT )
  {
    // Process message from RTOS queue
    KeyTestApp_ProcessRtosMessage();

    // return unprocessed events
    return (events ^ KEYTESTAPP_RTOS_MSG_EVT);
  }
#endif

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * Event Generation Functions
 */

/*********************************************************************
 * @fn      KeyTestApp_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   none
 *
 * @return  none
 */
static void KeyTestApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
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
            KeyTestApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            KeyTestApp_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            KeyTestApp_DstAddr.endPoint = pRsp->epList[0];

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
 * @fn      KeyTestApp_HandleKeys
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
static void KeyTestApp_HandleKeys( uint8 shift, uint8 keys )
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
//                            KeyTestApp_epDesc.endPoint,
//                            KEYTESTAPP_PROFID,
//                            KEYTESTAPP_MAX_CLUSTERS, (cId_t *)KeyTestApp_ClusterList,
//                            KEYTESTAPP_MAX_CLUSTERS, (cId_t *)KeyTestApp_ClusterList,
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
//                        KEYTESTAPP_PROFID,
//                        KEYTESTAPP_MAX_CLUSTERS, (cId_t *)KeyTestApp_ClusterList,
//                        KEYTESTAPP_MAX_CLUSTERS, (cId_t *)KeyTestApp_ClusterList,
//                        FALSE );
    }
  }
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      KeyTestApp_MessageMSGCB
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
static void KeyTestApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case KEYTESTAPP_CLUSTERID:
      rxMsgCount += 1;  // Count this message
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_BLINK );  // Blink an LED
#if defined( LCD_SUPPORTED )
      HalLcdWriteString( (char*)pkt->cmd.Data, HAL_LCD_LINE_1 );
      HalLcdWriteStringValue( "Rcvd:", rxMsgCount, 10, HAL_LCD_LINE_2 );
#elif defined( WIN32 )
      WPRINTSTR( pkt->cmd.Data );
#endif
    #ifdef COO
      rcv_data = *(pkt->cmd.Data);
      if(flag){
        sync_cnt();
        osal_start_timerEx( KeyTestApp_TaskID, RCV_CNT_EVT, 1000 );
        flag = 0;
      }
    #endif
      break;
  }
}

/*********************************************************************
 * @fn      KeyTestApp_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
uint8 send_cnt = 0;
static void KeyTestApp_SendTheMessage( void )
{
#ifdef EDV  
//  char theMessageData[] = "Hello World";
//
  if(KeyTestApp_NwkState != DEV_END_DEVICE)
    return;
  if ( AF_DataRequest( &KeyTestApp_DstAddr, &KeyTestApp_epDesc,
                       KEYTESTAPP_CLUSTERID,
                       1,
                       (byte *)&send_cnt,
                       &KeyTestApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
    // Successfully requested to be sent.
  }
  else
  {
    // Error occurred in request to send.
  }
  
  printf("数据已发送 %d\r\n",send_cnt);
  send_cnt++;
#endif
}



#if defined( IAR_ARMCM3_LM )
/*********************************************************************
 * @fn      KeyTestApp_ProcessRtosMessage
 *
 * @brief   Receive message from RTOS queue, send response back.
 *
 * @param   none
 *
 * @return  none
 */
static void KeyTestApp_ProcessRtosMessage( void )
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
