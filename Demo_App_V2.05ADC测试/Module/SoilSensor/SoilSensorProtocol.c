#include "ProtocolFrame.h"
#include "SoilSensorProtocol.h"
#include "protocol.h"
#include "ProtocolHandle.h"

#include "af.h"
#include "DemoApp.h"
#include "tool.h"
#include "ModuleManager.h"

void ProtocolSend(uint8_t* data, uint8_t len){
  static byte TransID = 0;
//  printf("准备发送:%d\r\n",len);
//  Protocol_Printf(data, len); 
  ModuleBoard_T *module = getModuleByModuleID((MODULE)(data[3] & 0x0f));
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


void Protocol_Init(){ 
	Protocol_Desc_T pdt; 
  //心跳协议
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = SOIL_SENSOR_HEARTBEAT_PROTOCOL; 
  pdt.Send = ProtocolSend; 
	Protocol_Register(&pdt, SEND); 
  //上报协议
  pdt.ProtocolSize = sizeof(SolidSensor_State_P_T);
	pdt.ModuleAction = SOIL_SENSOR_STATE_PROTOCOL; 
  pdt.Send = ProtocolSend; 
	Protocol_Register(&pdt, SEND); 

}

