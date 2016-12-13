#include "ProtocolHandle.h" 
#include "ModuleManager.h"
#include "ESP8266_Driver.h"
#include "SoilSensorProtocol.h"
#include "protocol.h"
/*网关到协调器控制协议*/
void Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* Cmd_Protocol = pi->ParameterList;
    printf("控制指令:%X\r\n", Cmd_Protocol->para1);
    switch(Cmd_Protocol->para1){
      case WATER_STATE_GET:{
          STATEGET_PROTOCOL_T StateGet_Protocol = {0};
          Protocol_Send(WATER_STATEGET_PROTOCOL, &StateGet_Protocol, sizeof(STATEGET_PROTOCOL_T));
          break;
      }case SOIL_SENSOR_STATE_GET:{
        Log.info("收到获取土壤湿度指令\r\n");
        ModuleBoard_T* SoilSensorModule = getModuleByModuleID(SOIL_SENSOR_MODULE);
        if(SoilSensorModule->DataSize == 0)
          printf("土壤监测板未连接\r\n");
        Protocol_Send(SOIL_SENSOR_STATE_PROTOCOL, SoilSensorModule->DataBuff, SoilSensorModule->DataSize);
        break;
      }
    }
}

/*主控对所有终端心跳的处理*/
void HearBeat_P_Handle(Protocol_Info_T* pi){
    HEARTBEAT_PROTOCOL_T* Heartbeat_P = pi->ParameterList;
    //printf("心跳:%X\r\n", pi->protocol.Heartbeat_P.para1);
    ModuleBoard_T *ModuleBoard = getModuleByModuleID((MODULE)(pi->Module >> 4));
    if(ModuleBoard == NULL){printf("收到心跳但是无法匹配到模块\r\n");return;} 
    ModuleBoard->ShortAddr = Heartbeat_P->para2 << 8 | Heartbeat_P->para3;
    ModuleBoard->ModuleState = ONLINE;
    ModuleBoard->HeartBeat_Flag = ModuleBoard->HeartBeat_Flag | 0x01;
    printf("收到%s模块心跳，地址为%X,%X,心跳号:%d\r\n", ModuleBoard->Name,ModuleBoard->ShortAddr>> 8, ModuleBoard->ShortAddr & 0xff,*(uint8_t*)pi->ParameterList); 
}

/*饮水机状态处理协议*/
void Water_State_P_Handle(Protocol_Info_T* pi){
  STATE_PROTOCOL_T* sp = pi->ParameterList;//将模块数据缓存到模块存储区
  ModuleSaveData(WATER_MODULE, sp, sizeof(STATE_PROTOCOL_T));
  printf("饮水机状态:%X\r\n", sp->para1);
}

/*饮水机命令协议*/
void Water_Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* Cmd_Protocol = pi->ParameterList;
  switch(Cmd_Protocol->para1){
    case 0:{
      printf("指令:饮水机关闭\r\n");
      break;
    }case 1:{
      printf("指令:饮水机开启\r\n");
      break;
    }
  }
}
/*土壤监测状态处理协议*/
void SolidSensor_State_P_Handle(Protocol_Info_T* pi){
  SolidSensor_State_P_T* SolidSensor_State = pi->ParameterList;
  ModuleSaveData(SOIL_SENSOR_MODULE, SolidSensor_State, sizeof(SolidSensor_State_P_T)); 
  char *p = (char*)SolidSensor_State;
  printf("土壤监测数据：");
  for(uint8_t i = 0; i < 8; i++)
    printf("%d ", *(p+i)); 
  printf("\r\n");

}