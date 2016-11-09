 
#include "Protocol.h"
#include "ProtocolHandle.h"
#include "WaterMachineProtocol.h"


void Protocol_Init(SEND_INTERFACE Send_Handle){ 
	PROTOCOL_DESC_T pdt; 

	pdt.ProtocolSize = sizeof(HEARTBEAT_PROTOCOL_T);
	pdt.ModuleAction = HEARTBEAT_PROTOCOL;  
	pdt.handle = HearBeat_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
	
        pdt.ProtocolSize = sizeof(STATE_PROTOCOL_T);
	pdt.ModuleAction = STATE_PROTOCOL;  
	pdt.handle = State_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
        
	pdt.ProtocolSize = sizeof(ACK_PROTOCOL_T);
	pdt.ModuleAction = ACK_PROTOCOL;  
	pdt.handle = Ack_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
        	
        pdt.ProtocolSize = sizeof(ADDRREPORT_PROTOCOL_T);
	pdt.ModuleAction = ADDRREPORT_PROTOCOL;  
        pdt.handle = AddrReport_P_Handle;
	Protocol_Register(&pdt,RECEIVE);
        
        
        
        pdt.ProtocolSize = sizeof(CMD_PROTOCOL_T);
	pdt.ModuleAction = CMD_PROTOCOL; 
        pdt.send = Send_Handle; 
	Protocol_Register(&pdt,SEND);
	
        pdt.ProtocolSize = sizeof(STATEGET_PROTOCOL_T);
	pdt.ModuleAction = STATEGET_PROTOCOL; 
        pdt.send = Send_Handle; 
	Protocol_Register(&pdt,SEND);
        
	

}

