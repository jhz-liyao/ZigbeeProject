 
#include "Protocol.h"
#include "ProtocolHandle.h"
#include "WaterMachineProtocol.h"
#include "ProtocolFrame.h"
#include "af.h"
#include "DemoApp.h"
#include "tool.h"
#include "ModuleManager.h"
#include "SoilSensorProtocol.h"
#include "Queue.h"
#include "LOG.h"
#include "DHT11_Protocol.h"

void SendToEndDev(uint8_t* data, uint8_t len);


/*从网关接收到队列*/
void WifiToUart_Handle(void){
  uint16 len = 0;
  uint8* data = NULL;
  if((len = Queue_Link_OutSize(WifiToUart_Queue)) > 0){
    data = MALLOC(len);
    MALLOC_CHECK(data, "");
    Queue_Link_Get(WifiToUart_Queue, data);
//    for(uint8_t i = 0; i < len; i++){
//      printf("%x ", data[i]);
//    }
//    printf("\r\nlen:%d\r\n", len); 
    Protocol_Resolver_1->Protocol_Put(Protocol_Resolver_1,data, len);
    printf("WifiToUart_Handle_len:%d\r\n", len);
    FREE(data);
  }
}


/*从队列发送到网关*/
void UartToWifi_Handle(void){
  uint16 len = 0;
  uint8* data = NULL;
  if((len = Queue_Link_OutSize(UartToWifi_Queue)) > 0){
    data = MALLOC(len);
    MALLOC_CHECK(data, "");
    Queue_Link_Get(UartToWifi_Queue, data);
    HalUARTWrite( HAL_UART_PORT_0, data, len );
    HalUARTWrite( HAL_UART_PORT_0, "\n", 1 );
    printf("UartToWifi_Handle_len:%d\r\n", len);
    FREE(data);
  }
}

/*接收来自终端的数据队列*/
void DeviceRecv_Handle(void){
  uint16 len = 0;
  uint8* data = NULL;
  if((len = Queue_Link_OutSize(DeviceRecv_Queue)) > 0){
    data = MALLOC(len);
    MALLOC_CHECK(data, "");
    Queue_Link_Get(DeviceRecv_Queue, data);
    Protocol_Resolver_1->Protocol_Put(Protocol_Resolver_1, data, len);
    printf("DeviceRecv_Handle_len:%d\r\n", len);
    FREE(data);
  }
}

/*协议发送回调函数----发送到网关*/
void SendToWIFI(uint8_t* data, uint8_t len){ 
  Queue_Link_Put(UartToWifi_Queue, data, len);
}

/*协议发送回调函数----将数据发送至终端*/
void SendToEndDev(uint8_t* data, uint8_t len){
  static byte TransID = 0; 
  Protocol_Printf(data, len); 
  ModuleBoard_T *module = getModuleByModuleID((MODULE)(data[3] & 0x0f)); 
  if(module == NULL){
    Log.info("SendToEndDev 查找的module不存在\r\n");
    return;
  }else if(module->ShortAddr == 0){
    Log.info("模块不在线 放弃发送\r\n");
    return;
  } 
  
  afAddrType_t Protocol_DstAddr;  //地址描述符
  endPointDesc_t Protocol_epDesc; //端点描述符 
  
  Protocol_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  Protocol_DstAddr.endPoint = APP_ENDPOINT;
  Protocol_DstAddr.addr.shortAddr = module->ShortAddr;
 
  Protocol_epDesc = App_GetepDesc();
  TransID++;
  if ( AF_DataRequest( &Protocol_DstAddr, &Protocol_epDesc,
                       APP_CLUSTERID,
                       len,
                       (byte *)data,
                       &TransID,
                       AF_ACK_REQUEST, AF_DEFAULT_RADIUS ) == afStatus_SUCCESS );

}
///*接收来自wifi的数据转发给终端节点*/
//void TranspondHandle(Protocol_Info_T* pi){
//  uint8_t data[100] = {0};
//  uint8_t len = Protocol_Serialization(pi, data, 100); 
//  SendToEndDev(data, len); 
//}



void Protocol_Init(){ 
  
	Protocol_Desc_T pdt; 
  
  /*网关通信协议*/
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T)); //接收网关控制指令
	pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_PROTOCOL;  
	pdt.Handle = Cmd_P_Handle;
	Protocol_Register(&pdt,RECEIVE);  
  
  /*通用终端协议*/
  
  /*饮水机协议*/  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);//饮水机心跳
	pdt.ModuleAction = WATER_HEARTBEAT_PROTOCOL;  
	pdt.Handle = HearBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);  
	
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
  pdt.ProtocolSize = sizeof(STATE_PROTOCOL_T);  //上报饮水机状态
	pdt.ModuleAction = WATER_STATE_PROTOCOL;  
	pdt.Handle = Water_State_P_Handle; 
	Protocol_Register(&pdt,RECEIVE);  

  memset(&pdt, 0, sizeof(Protocol_Desc_T)); //饮水机控制协议
  pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = WATER_CMD_PROTOCOL;
  pdt.TranspondHandle = Protocol_Send_Transpond; 
  pdt.Handle = Water_Cmd_P_Handle;
  pdt.Send = SendToEndDev; 
	Protocol_Register(&pdt,SEND);
	
  memset(&pdt, 0, sizeof(Protocol_Desc_T));//获取饮水机状态(通过网关命令调用通用控制指令)
  pdt.ProtocolSize = sizeof(STATEGET_PROTOCOL_T);
	pdt.ModuleAction = WATER_STATEGET_PROTOCOL; 
  pdt.Handle = NULL;
  pdt.TranspondHandle = Protocol_Send_Transpond;
  pdt.Send = SendToEndDev; 
	Protocol_Register(&pdt,SEND);
   
  /*土壤传感板协议*/  
  memset(&pdt, 0, sizeof(Protocol_Desc_T)); //接收土壤监测板心跳
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = SOIL_SENSOR_HEARTBEAT_PROTOCOL;  
	pdt.Handle = HearBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
        
	  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));//接收土壤检测板监测数据
  pdt.ProtocolSize = sizeof(SolidSensor_State_P_T);
	pdt.ModuleAction = SOIL_SENSOR_STATE_PROTOCOL;  
  pdt.Handle = SolidSensor_State_P_Handle; 
  pdt.Send = SendToWIFI;
	Protocol_Register(&pdt,RECEIVE);
  
  /*DHT11 温湿度协议*/
  memset(&pdt, 0, sizeof(Protocol_Desc_T));//向网关发送协调器温湿度信息
  pdt.ProtocolSize = sizeof(DHT11_State_P_T);
	pdt.ModuleAction = DHT11_PROTOCOL;
  pdt.Send = SendToWIFI; 
	Protocol_Register(&pdt,SEND);

}

