 
#include "Protocol.h"
#include "ProtocolHandle.h"
#include "WaterMachineProtocol.h"

#include "af.h"
#include "DemoApp.h"
#include "tool.h"
#include "ModuleManager.h"

//接收来自wifi的数据转发给终端节点
void TranspondHandle(Protocol_Info_T* pi){
  Protocol_Send_Transpond(pi);
  printf("收到wifi协议\r\n");
}

void ProtocolSend(uint8_t* data, uint8_t len){
  static byte TransID = 0;
//  printf("准备发送:%d\r\n",len);
  Protocol_Printf(data, len); 
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
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = WATER_HEARTBEAT_PROTOCOL;  
	pdt.Handle = HearBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
  
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = SOIL_SENSOR_HEARTBEAT_PROTOCOL;  
	pdt.Handle = HearBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
	
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
  pdt.ProtocolSize = sizeof(STATE_PROTOCOL_T);
	pdt.ModuleAction = STATE_PROTOCOL;  
	pdt.Handle = State_P_Handle; 
	Protocol_Register(&pdt,RECEIVE);
        
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
	pdt.ProtocolSize = sizeof(ACK_PROTOCOL_T);
	pdt.ModuleAction = ACK_PROTOCOL;  
	pdt.Handle = Ack_P_Handle; 
	Protocol_Register(&pdt,RECEIVE);
        	
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
  pdt.ProtocolSize = sizeof(ADDRREPORT_PROTOCOL_T);
	pdt.ModuleAction = ADDRREPORT_PROTOCOL;  
  pdt.Handle = AddrReport_P_Handle; 
	Protocol_Register(&pdt,RECEIVE);
        
        
        
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
  pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_PROTOCOL; 
  pdt.Handle = TranspondHandle;
  pdt.Send = ProtocolSend; 
	Protocol_Register(&pdt,SEND);
	
  memset(&pdt, 0, sizeof(Protocol_Desc_T));
  pdt.ProtocolSize = sizeof(STATEGET_PROTOCOL_T);
	pdt.ModuleAction = STATEGET_PROTOCOL; 
  pdt.Handle = TranspondHandle;
  pdt.Send = ProtocolSend; 
	Protocol_Register(&pdt,SEND);
        
	

}

