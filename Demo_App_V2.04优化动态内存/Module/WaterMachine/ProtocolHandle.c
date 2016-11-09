#include "ProtocolHandle.h"
#include "WaterMachineDriver.h"
#include "ModuleManager.h"
//void Cmd_P_Handle(Protocol_Info_T* pi){
//    printf("����ָ��:%X\r\n", pi->protocol.Cmd_P.para1);
//}
void ReportState(void){
    STATE_PROTOCOL_T spt = {0};
    spt.para1 = WaterM.State;
    Protocol_Send(STATE_PROTOCOL, &spt, sizeof(STATE_PROTOCOL_T)); 
}


void StateGet_P_Handle(Protocol_Info_T* pi){ 
    STATEGET_PROTOCOL_T* p = pi->ParameterList;
    printf("��ȡ״̬:%X\r\n", p->para1); 
    ReportState(); 
} 


void HearBeat_P_Handle(Protocol_Info_T* pi){
    HEARTBEAT_PROTOCOL_T* hp = pi->ParameterList;
    //printf("����:%X\r\n", pi->protocol.Heartbeat_P.para1);
    ModuleBoard_T *ModuleBoard = getModuleByModuleID((MODULE)(pi->Module >> 4));
    if(ModuleBoard == NULL){printf("�յ����������޷�ƥ�䵽ģ��\r\n");return;} 
    ModuleBoard->ShortAddr = hp->para2 << 8 | hp->para3;
    ModuleBoard->ModuleState = ONLINE;
    printf("�յ�%sģ����������ַΪ%X,%X\r\n", ModuleBoard->Name,ModuleBoard->ShortAddr>> 8, ModuleBoard->ShortAddr & 0xff); 
}
