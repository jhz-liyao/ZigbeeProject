#ifndef __MODULEMANAGER_H__
#define __MODULEMANAGER_H__
#include "app_types.h"
#include "Protocol.h"

//#define MODULE_COUNT 3
typedef enum{ONLINE, OFFLINE}ModuleState;
typedef struct {
    uint8_t Name[8];            //模块中文名
    MODULE ModuleID;            //模块编号
    ModuleState ModuleState;    //模块是否在线
    int8_t HeartBeat_Flag;      //模块心跳
    uint16_t ShortAddr;         //模块短地址
}ModuleBoard_T;

//extern ModuleBoard_T ModuleList[];
//extern uint8_t Module_Count;


extern ModuleBoard_T* getModuleByName(uint8_t *name);
extern ModuleBoard_T* getModuleByModuleID(MODULE ModuleID);
extern ModuleBoard_T* getModuleByModuleAddr(uint16_t ShortAddr);
extern void HeartBeat_Check(void);
void HeartBeat_Set(MODULE ModuleID);
#endif