#include "ModuleManager.h"
#include <stdio.h>
#include <string.h>
#include "hal_defs.h"
ModuleBoard_T ModuleList[] = {
  {"主控", COORDINATOR_MODULE, OFFLINE ,0 ,0 },
  {"饮水机", WATER_MODULE    , OFFLINE ,0 ,0 },
};
uint8_t Module_Count = sizeof(ModuleList)/sizeof(ModuleBoard_T);

/****************************************************
	函数名:	getModuleByName
	功能:	从模块列表中获取模块信息根据模块中文名
	参数:	模块中文名 
        作者:	liyao 2016年10月19日14:38:12
****************************************************/
ModuleBoard_T* getModuleByName(uint8_t *name){
    uint8_t i;
    for(i = 0; i < Module_Count; i++){ 
      if(strcmp((char const*)name, (char const*)(&ModuleList[i].Name)) == 0){
        return &ModuleList[i];
      }
    }
    return NULL;
}

/****************************************************
	函数名:	getModuleByModuleID
	功能:	从模块列表中获取模块信息根据模块ID
	参数:	模块ID
        作者:	liyao 2016年10月19日14:38:12
****************************************************/
ModuleBoard_T* getModuleByModuleID(MODULE ModuleID){
    uint8_t i;
    for(i = 0; i < Module_Count; i++){ 
      if(ModuleID == ModuleList[i].ModuleID){
        return &ModuleList[i];
      }
    }
    return NULL;
}

/****************************************************
	函数名:	getModuleByModuleAddr
	功能:	从模块列表中获取模块信息根据模块短地址
	参数:	模块短地址
        作者:	liyao 2016年10月19日14:38:12
****************************************************/
ModuleBoard_T* getModuleByModuleAddr(uint16_t ShortAddr){
    uint8_t i;
    for(i = 0; i < Module_Count; i++){ 
      if(ShortAddr == ModuleList[i].ShortAddr){
        return &ModuleList[i];
      }
    }
    return NULL;
}

/****************************************************
	函数名:	ModuleHeartBeat_Check
	功能:	检查所有模块是否在线
	参数:	模块短地址
        作者:	liyao 2016年10月19日14:38:12
****************************************************/ 
void HeartBeat_Check(void){
    uint8_t i;
    for(i = 0; i < Module_Count; i++){ //位图操作最后2位判断历史状态
      if((ModuleList[i].HeartBeat_Flag & 0x03) == 0x02){
        printf("%s离线\r\n",ModuleList[i].Name);
        ModuleList[i].ShortAddr = 0; 
        ModuleList[i].ModuleState = OFFLINE; 
      }else if((ModuleList[i].HeartBeat_Flag & 0x03) == 0x01){
        printf("%s上线\r\n",ModuleList[i].Name); 
        ModuleList[i].ModuleState = ONLINE; 
      }
      ModuleList[i].HeartBeat_Flag = (ModuleList[i].HeartBeat_Flag <<1) & 0xFE;
    }
}

/****************************************************
	函数名:	HeartBeat_Set
	功能:	设置心跳存在
	参数:	模块ID
        作者:	liyao 2016年10月20日12:37:55
****************************************************/
void HeartBeat_Set(MODULE ModuleID){
    ModuleBoard_T *ModuleBoard = getModuleByModuleID(ModuleID);
    ModuleBoard->HeartBeat_Flag = ModuleBoard->HeartBeat_Flag | 0x01; 
}
//ModuleBoard_T WaterMachine = {
//    "饮水机"
//};

//void Module_Init(void){
//    uint8_t i;
//    
//}