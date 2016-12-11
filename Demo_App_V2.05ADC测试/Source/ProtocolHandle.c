#include "ProtocolHandle.h" 
#include "ModuleManager.h"
#include "ESP8266_Driver.h"
#include "SoilSensorProtocol.h"
/*���ص�Э��������Э��*/
void Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* Cmd_Protocol = pi->ParameterList;
    printf("����ָ��:%X\r\n", Cmd_Protocol->para1);
    switch(Cmd_Protocol->para1){
      case SOIL_SENSOR_STATE_GET:
        Log.info("�յ���ȡ����ʪ��ָ��\r\n");
        ModuleBoard_T* SoilSensorModule = getModuleByModuleID(SOIL_SENSOR_MODULE);
        if(SoilSensorModule->DataSize == 0)
          printf("��������δ����\r\n");
        Protocol_Send(SOIL_SENSOR_STATE_PROTOCOL, SoilSensorModule->DataBuff, SoilSensorModule->DataSize);
        break;
    }
}



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
    STATE_PROTOCOL_T* sp = pi->ParameterList;
    printf("�ͻ���״̬:%X\r\n", sp->para1);
}
 
void SolidSensor_State_P_Handle(Protocol_Info_T* pi){
  SolidSensor_State_P_T* SolidSensor_State = pi->ParameterList;
  
  ModuleBoard_T* SoilSensorModule = getModuleByModuleID(SOIL_SENSOR_MODULE); 
  if(SoilSensorModule != NULL){
    if(SoilSensorModule->DataBuff == NULL)
      SoilSensorModule->DataBuff = MALLOC(8);
      SoilSensorModule->DataSize = 8;
  }
  
  memcpy(SoilSensorModule->DataBuff, SolidSensor_State, 8);
  printf("%d, %d, %d, %d, %d, %d, %d, %d\r\n", SolidSensor_State->vcc
                                         , SolidSensor_State->para1
                                         , SolidSensor_State->para2
                                         , SolidSensor_State->para3
                                         , SolidSensor_State->para4
                                         , SolidSensor_State->para5
                                         , SolidSensor_State->para6
                                         , SolidSensor_State->para7);
}