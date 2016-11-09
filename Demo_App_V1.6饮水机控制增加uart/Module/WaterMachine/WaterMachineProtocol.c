#include "ProtocolFrame.h"
#include "WaterMachineProtocol.h"
#include "protocol.h"
#include "ProtocolHandle.h"
 

void Protocol_Init(SEND_INTERFACE SendHandle){ 
	PROTOCOL_DESC_T pdt; 
        //公共协议初始化
	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = HEARTBEAT_PROTOCOL; 
        pdt.send = SendHandle; 
	Protocol_Register(&pdt, SEND);
	
        pdt.ProtocolSize = sizeof(STATE_PROTOCOL_T);
	pdt.ModuleAction = STATE_PROTOCOL; 
        pdt.send = SendHandle; 
	Protocol_Register(&pdt, SEND);
        
	pdt.ProtocolSize = sizeof(ACK_PROTOCOL_T);
	pdt.ModuleAction = ACK_PROTOCOL; 
        pdt.send = SendHandle; 
	Protocol_Register(&pdt, SEND);
        
	pdt.ProtocolSize = sizeof(ADDRREPORT_PROTOCOL_T);
	pdt.ModuleAction = ADDRREPORT_PROTOCOL; 
        pdt.send = SendHandle; 
	Protocol_Register(&pdt, SEND);

        
        pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_PROTOCOL;  
	pdt.handle = Cmd_P_Handle;
	Protocol_Register(&pdt, RECEIVE);
	
        pdt.ProtocolSize = sizeof(STATEGET_PROTOCOL_T);
	pdt.ModuleAction = STATEGET_PROTOCOL;  
	pdt.handle = StateGet_P_Handle;
	Protocol_Register(&pdt, RECEIVE);
        
        //特有协议初始化	

}

