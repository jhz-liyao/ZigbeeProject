#include "ProtocolHandle.h" 
#include "ModuleManager.h"
#include "ESP8266_Driver.h"
//void Cmd_P_Handle(Protocol_Info_T* pi){
//    printf("����ָ��:%X\r\n", pi->protocol.Cmd_P.para1);
//}



void StateGet_P_Handle(Protocol_Info_T* pi){
    //printf("��ȡ״̬:%X\r\n", pi->protocol.StateGet_P.para1);
} 



void HearBeat_P_Handle(Protocol_Info_T* pi){
  HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
    //printf("����:%X\r\n", pi->protocol.Heartbeat_P.para1);
    ModuleBoard_T *ModuleBoard = getModuleByModuleID((MODULE)(pi->Module >> 4));
    if(ModuleBoard == NULL){printf("�յ����������޷�ƥ�䵽ģ��\r\n");return;} 
    ModuleBoard->ShortAddr = Heartbeat_P->para2 << 8 | Heartbeat_P->para3;
    ModuleBoard->ModuleState = ONLINE;
    ModuleBoard->HeartBeat_Flag = ModuleBoard->HeartBeat_Flag | 0x01;
    printf("�յ�%sģ����������ַΪ%X,%X,������:%d\r\n", ModuleBoard->Name,ModuleBoard->ShortAddr>> 8, ModuleBoard->ShortAddr & 0xff,*(uint8_t*)pi->ParameterList); 
}
void State_P_Handle(Protocol_Info_T* pi){
    //printf("�ͻ���״̬:%X\r\n", pi->protocol.State_P.para1);
    ESP8266_Protocol_Send(pi);
}
void Ack_P_Handle(Protocol_Info_T* pi){
    //printf("�ͻ���Ӧ��:%X\r\n", pi->protocol.Ack_P.para1);
}
void AddrReport_P_Handle(Protocol_Info_T* pi){
    //printf("�ͻ����ϱ���ַ:%X,%X\r\n", pi->protocol.AddrReport_P.para1, pi->protocol.AddrReport_P.para2);
     
} 
