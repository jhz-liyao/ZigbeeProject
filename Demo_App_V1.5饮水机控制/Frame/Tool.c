#include "Tool.h"

afAddrType_t Liyao_DstAddr;

const cId_t Liyao_ClusterList[LIYAO_MAX_CLUSTERS] =
{
  LIYAO_PERIODIC_CLUSTERID,
  LIYAO_FLASH_CLUSTERID
};

const SimpleDescriptionFormat_t Liyao_SimpleDesc =
{
  LIYAO_ENDPOINT,              //  int Endpoint;
  LIYAO_PROFID,                //  uint16 AppProfId[2];
  LIYAO_DEVICEID,              //  uint16 AppDeviceId[2];
  LIYAO_DEVICE_VERSION,        //  int   AppDevVer:4;
  LIYAO_FLAGS,                 //  int   AppFlags:4;
  LIYAO_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)Liyao_ClusterList,  //  uint8 *pAppInClusterList;
  LIYAO_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)Liyao_ClusterList   //  uint8 *pAppInClusterList;
};

__near_func int putchar(int c)
{
    HalUARTWrite(0,(unsigned char*)&c,1);
    return(c);
}

void Device_Info(void){
  Liyao_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  Liyao_DstAddr.endPoint = LIYAO_ENDPOINT;
  Liyao_DstAddr.addr.shortAddr = 0x0000;
  UART_Config_L();
  //uint16 short_Addr = NLME_GetShortAddr();
  //byte* shortAddr_byte = (byte*)&short_Addr;
  byte* longAddr = NLME_GetExtAddr();
  #if defined(ZDO_COORDINATOR)
    printf("协调器节点,长地址:%X %X %X %X %X %X %X %X \r\n",
           longAddr[0], longAddr[1], longAddr[2], longAddr[3], longAddr[4], longAddr[5], longAddr[6], longAddr[7] );
  #elif defined(RTR_NWK) 
      printf("路由器节点, 长地址:%X %X %X %X %X %X %X %X \r\n",
           longAddr[0], longAddr[1], longAddr[2], longAddr[3], longAddr[4], longAddr[5], longAddr[6], longAddr[7] );
  #else
      printf("终端节点, 长地址:%X %X %X %X %X %X %X %X \r\n",
           longAddr[0], longAddr[1], longAddr[2], longAddr[3], longAddr[4], longAddr[5], longAddr[6], longAddr[7] );
  #endif
}


void SendDataToShortAddr(uint8 TaskID,uint16 shortAddr,unsigned char* data, uint16 len){
  Liyao_DstAddr.addr.shortAddr = shortAddr;
  endPointDesc_t Liyao_epDesc;
  Liyao_epDesc.endPoint = LIYAO_ENDPOINT;
  Liyao_epDesc.task_id = &TaskID;
  Liyao_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&Liyao_SimpleDesc;
  Liyao_epDesc.latencyReq = noLatencyReqs; 
  if ( AF_DataRequest( &Liyao_DstAddr, &Liyao_epDesc,
                       LIYAO_FLASH_CLUSTERID,
                       len,
                       data,
                       (unsigned char*)&TaskID,
                       AF_ACK_REQUEST,
                       AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
  {
  }
}




/*
  功能：串口回环
  作者：李尧
  时间：2016年9月22日13:03:40
*/
void UART_CallBack( uint8 port, uint8 event ){
  uint16 i,cnt = 0;
  uint8 data[MT_UART_DEFAULT_MAX_RX_BUFF] = {0}; 
  cnt = Hal_UART_RxBufLen(port);
  cnt = HalUARTRead (port, data, cnt);
  for(i = 0;i < cnt; i++){
    HalUARTWrite(port, &data[i], 1); 
  }
//  HalUARTWrite(port, data, cnt);
}
void UART_Config_L(void){
  HalUARTInit();
  
  halUARTCfg_t uartConfig;
  /* UART Configuration */
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.flowControlThreshold = MT_UART_DEFAULT_THRESHOLD;
  uartConfig.rx.maxBufSize        = MT_UART_DEFAULT_MAX_RX_BUFF;
  uartConfig.tx.maxBufSize        = MT_UART_DEFAULT_MAX_TX_BUFF;
  uartConfig.idleTimeout          = MT_UART_DEFAULT_IDLE_TIMEOUT;
  uartConfig.intEnable            = TRUE; 
  uartConfig.callBackFunc         = UART_CallBack;
  HalUARTOpen (HAL_UART_PORT_0, &uartConfig);
}

void ZdoStateChange_printf(devStates_t devStates){
  switch((uint8)devStates){
    case (uint8)DEV_HOLD:
      printf("DEV_HOLD\r\n");
      break;
    case (uint8)DEV_INIT:
      printf("DEV_INIT\r\n"); 
      break;
    case (uint8)DEV_NWK_DISC:
      printf("DEV_NWK_DISC\r\n"); 
      break;
    case (uint8)DEV_NWK_JOINING:
      printf("DEV_NWK_JOINING\r\n"); 
      break;
    case (uint8)DEV_NWK_REJOIN:
      printf("DEV_NWK_REJOIN\r\n"); 
      break;
    case (uint8)DEV_END_DEVICE_UNAUTH:
      printf("DEV_END_DEVICE_UNAUTH 加入但尚未认证\r\n");  
      break;
    case (uint8)DEV_END_DEVICE:
      printf("DEV_END_DEVICE 设备认证完成 \r\n"); 
      break;
    case (uint8)DEV_ROUTER:
      printf("DEV_ROUTER 设备加入,进行验证,是一个路由器 \r\n");  
      break;
    case (uint8)DEV_COORD_STARTING: 
      printf("DEV_COORD_STARTING 协调器开始协调 \r\n");  
      break;
    case (uint8)DEV_ZB_COORD: 
      printf("DEV_ZB_COORD 协调器开始协调 \r\n");
      break; 
    case (uint8)DEV_NWK_ORPHAN:
      printf("DEV_NWK_ORPHAN 设备失去父节点连接. . \r\n");
      
      break;
  }
}


void Meter_Leave(void)
{
  NLME_LeaveReq_t leaveReq = {0};
    //osal_memset((uint8 *)&leaveReq,0,sizeof(NLME_LeaveReq_t));
  osal_memcpy(leaveReq.extAddr,NLME_GetExtAddr(),Z_EXTADDR_LEN);
  leaveReq.removeChildren = 1;
  leaveReq.rejoin = 0;
  leaveReq.silent = 0;
  NLME_LeaveReq( &leaveReq );
}

void Protocol_Printf(uint8 *data, uint8 len){
    uint8 i = 0;
    for(i = 0; i < len; i++)
      printf("%X ",data[i]);
    printf("\r\n");
}