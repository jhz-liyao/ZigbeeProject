#include "ProtocolHandle.h"
#include "WaterMachineDriver.h"
#include "ModuleManager.h"
//void Cmd_P_Handle(Protocol_Info_T* pi){
//    printf("控制指令:%X\r\n", pi->protocol.Cmd_P.para1);
//}

void StateGet_P_Handle(Protocol_Info_T* pi){
    printf("获取状态:%X\r\n", pi->protocol.StateGet_P.para1);
    PROTOCOL_T protocol = {0};  
    protocol.State_P.para1 = WaterM.State;
    Protocol_Send(STATE_PROTOCOL, &protocol, sizeof(protocol.State_P)); 
} 

void ReportState(void){
    PROTOCOL_T protocol = {0};  
    protocol.State_P.para1 = WaterM.State;
    Protocol_Send(STATE_PROTOCOL, &protocol, sizeof(protocol.State_P));
}

void HearBeat_P_Handle(Protocol_Info_T* pi){
    //printf("心跳:%X\r\n", pi->protocol.Heartbeat_P.para1);
    ModuleBoard_T *ModuleBoard = getModuleByModuleID((MODULE)(pi->Module >> 4));
    if(ModuleBoard == NULL){printf("收到心跳但是无法匹配到模块\r\n");return;} 
    ModuleBoard->ShortAddr = pi->protocol.Heartbeat_P.para2 << 8 | pi->protocol.Heartbeat_P.para3;
    ModuleBoard->ModuleState = ONLINE;
    printf("收到%s模块心跳，地址为%X,%X\r\n", ModuleBoard->Name,ModuleBoard->ShortAddr>> 8, ModuleBoard->ShortAddr & 0xff); 
}
void State_P_Handle(Protocol_Info_T* pi){
    printf("客户端状态:%X\r\n", pi->protocol.State_P.para1);
}
void Ack_P_Handle(Protocol_Info_T* pi){
    printf("客户端应答:%X\r\n", pi->protocol.Ack_P.para1);
}
void AddrReport_P_Handle(Protocol_Info_T* pi){
    printf("客户端上报地址:%X,%X\r\n", pi->protocol.AddrReport_P.para1, pi->protocol.AddrReport_P.para2);
    if(pi->Module >> 4 == WATER_MODULE){
        WaterM.Short_Addr = pi->protocol.AddrReport_P.para1 << 8 | pi->protocol.AddrReport_P.para2;
    }
} 
