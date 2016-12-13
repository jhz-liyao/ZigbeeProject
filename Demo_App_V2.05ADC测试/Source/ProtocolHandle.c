#include "ProtocolHandle.h" 
#include "ModuleManager.h"
#include "ESP8266_Driver.h"
#include "SoilSensorProtocol.h"
#include "protocol.h"
/*���ص�Э��������Э��*/
void Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* Cmd_Protocol = pi->ParameterList;
    printf("����ָ��:%X\r\n", Cmd_Protocol->para1);
    switch(Cmd_Protocol->para1){
      case WATER_STATE_GET:{
          STATEGET_PROTOCOL_T StateGet_Protocol = {0};
          Protocol_Send(WATER_STATEGET_PROTOCOL, &StateGet_Protocol, sizeof(STATEGET_PROTOCOL_T));
          break;
      }case SOIL_SENSOR_STATE_GET:{
        Log.info("�յ���ȡ����ʪ��ָ��\r\n");
        ModuleBoard_T* SoilSensorModule = getModuleByModuleID(SOIL_SENSOR_MODULE);
        if(SoilSensorModule->DataSize == 0)
          printf("��������δ����\r\n");
        Protocol_Send(SOIL_SENSOR_STATE_PROTOCOL, SoilSensorModule->DataBuff, SoilSensorModule->DataSize);
        break;
      }
    }
}

/*���ض������ն������Ĵ���*/
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

/*��ˮ��״̬����Э��*/
void Water_State_P_Handle(Protocol_Info_T* pi){
  STATE_PROTOCOL_T* sp = pi->ParameterList;//��ģ�����ݻ��浽ģ��洢��
  ModuleSaveData(WATER_MODULE, sp, sizeof(STATE_PROTOCOL_T));
  printf("��ˮ��״̬:%X\r\n", sp->para1);
}

/*��ˮ������Э��*/
void Water_Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* Cmd_Protocol = pi->ParameterList;
  switch(Cmd_Protocol->para1){
    case 0:{
      printf("ָ��:��ˮ���ر�\r\n");
      break;
    }case 1:{
      printf("ָ��:��ˮ������\r\n");
      break;
    }
  }
}
/*�������״̬����Э��*/
void SolidSensor_State_P_Handle(Protocol_Info_T* pi){
  SolidSensor_State_P_T* SolidSensor_State = pi->ParameterList;
  ModuleSaveData(SOIL_SENSOR_MODULE, SolidSensor_State, sizeof(SolidSensor_State_P_T)); 
  char *p = (char*)SolidSensor_State;
  printf("����������ݣ�");
  for(uint8_t i = 0; i < 8; i++)
    printf("%d ", *(p+i)); 
  printf("\r\n");

}