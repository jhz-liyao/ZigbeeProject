#include "ProtocolHandle.h" 
#include "ModuleManager.h"
//void Cmd_P_Handle(PROTOCOL_INFO_T* pi){
//    printf("控制指令:%X\r\n", pi->protocol.Cmd_P.para1);
//}

void StateGet_P_Handle(PROTOCOL_INFO_T* pi){
    printf("获取状态:%X\r\n", pi->protocol.StateGet_P.para1);
} 



void HearBeat_P_Handle(PROTOCOL_INFO_T* pi){
    //printf("心跳:%X\r\n", pi->protocol.Heartbeat_P.para1);
    ModuleBoard_T *ModuleBoard = getModuleByModuleID((MODULE)(pi->module >> 4));
    if(ModuleBoard == NULL){printf("收到心跳但是无法匹配到模块\r\n");return;} 
    ModuleBoard->ShortAddr = pi->protocol.Heartbeat_P.para2 << 8 | pi->protocol.Heartbeat_P.para3;
    ModuleBoard->ModuleState = ONLINE;
    ModuleBoard->HeartBeat_Flag = ModuleBoard->HeartBeat_Flag | 0x01;
    printf("收到%s模块心跳，地址为%X,%X,心跳号:%d\r\n", ModuleBoard->Name,ModuleBoard->ShortAddr>> 8, ModuleBoard->ShortAddr & 0xff,pi->protocol.Heartbeat_P.para1); 
}
void State_P_Handle(PROTOCOL_INFO_T* pi){
    printf("客户端状态:%X\r\n", pi->protocol.State_P.para1);
}
void Ack_P_Handle(PROTOCOL_INFO_T* pi){
    printf("客户端应答:%X\r\n", pi->protocol.Ack_P.para1);
}
void AddrReport_P_Handle(PROTOCOL_INFO_T* pi){
    printf("客户端上报地址:%X,%X\r\n", pi->protocol.AddrReport_P.para1, pi->protocol.AddrReport_P.para2);
     
} 
