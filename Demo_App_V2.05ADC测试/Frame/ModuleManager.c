#include "ModuleManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_defs.h"
#include "Log.h"
ModuleBoard_T ModuleList[] = {
  {"����", COORDINATOR_MODULE, OFFLINE ,0 ,0, NULL },
  {"��ˮ��", WATER_MODULE    , OFFLINE ,0 ,0 , NULL},
  {"�������", SOIL_SENSOR_MODULE    , OFFLINE ,0 ,0 , NULL},
  
};
uint8_t Module_Count = sizeof(ModuleList)/sizeof(ModuleBoard_T);

/****************************************************
	������:	getModuleByName
	����:	��ģ���б��л�ȡģ����Ϣ����ģ��������
	����:	ģ�������� 
        ����:	liyao 2016��10��19��14:38:12
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
	������:	getModuleByModuleID
	����:	��ģ���б��л�ȡģ����Ϣ����ģ��ID
	����:	ģ��ID
        ����:	liyao 2016��10��19��14:38:12
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
	������:	getModuleByModuleAddr
	����:	��ģ���б��л�ȡģ����Ϣ����ģ��̵�ַ
	����:	ģ��̵�ַ
        ����:	liyao 2016��10��19��14:38:12
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
	������:	ModuleHeartBeat_Check
	����:	�������ģ���Ƿ�����
	����:	ģ��̵�ַ
        ����:	liyao 2016��10��19��14:38:12
****************************************************/ 
void HeartBeat_Check(void){
    uint8_t i;
    for(i = 0; i < Module_Count; i++){ //λͼ�������2λ�ж���ʷ״̬
      if((ModuleList[i].HeartBeat_Flag & 0x03) == 0x02){
        printf("%s����\r\n",ModuleList[i].Name);
        //ModuleList[i].ShortAddr = 0; 
        ModuleList[i].ModuleState = OFFLINE; 
      }else if((ModuleList[i].HeartBeat_Flag & 0x03) == 0x01){
        printf("%s����\r\n",ModuleList[i].Name); 
        ModuleList[i].ModuleState = ONLINE; 
      }
      ModuleList[i].HeartBeat_Flag = (ModuleList[i].HeartBeat_Flag <<1) & 0xFE;
    }
}

/****************************************************
	������:	HeartBeat_Set
	����:	������������
	����:	ģ��ID
        ����:	liyao 2016��10��20��12:37:55
****************************************************/
void HeartBeat_Set(MODULE ModuleID){
    ModuleBoard_T *ModuleBoard = getModuleByModuleID(ModuleID);
    ModuleBoard->HeartBeat_Flag = ModuleBoard->HeartBeat_Flag | 0x01; 
}

/****************************************************
	������:	ModuleSaveData
	����:	����ģ���������
	����:	ģ��ID, ����, ����
  ����:	liyao 2016��12��12��22:56:07
****************************************************/
void ModuleSaveData(MODULE ModuleID, void* data,uint8_t len){
     ModuleBoard_T* Module = getModuleByModuleID(ModuleID);//�����ݻ��浽ģ�����ݴ洢�� 
    if(Module != NULL){
      if(Module->DataBuff == NULL){
        Module->DataBuff = MALLOC(len);
        Module->DataSize = len;
      }else{
        FREE(Module->DataBuff);
        Module->DataBuff = MALLOC( len);
        Module->DataSize = len;
      }
      
      memcpy(Module->DataBuff, data, len);
    }
}

/****************************************************
	������:	ModuleGetDataLen
	����:	��ȡģ�黺��������
	����:	ģ��ID
  ����:	liyao 2016��12��12��22:56:07
****************************************************/
uint8_t ModuleGetDataLen(MODULE ModuleID){
    ModuleBoard_T* Module = getModuleByModuleID(ModuleID);//�����ݻ��浽ģ�����ݴ洢�� 
    if(Module != NULL){
      if(Module->DataBuff != NULL && Module->DataSize > 0){
        return Module->DataSize;
      }
    }
    return 0;
}

/****************************************************
	������:	ModuleGetData
	����:	��ȡģ���������
	����:	ģ��ID
  ����:	liyao 2016��12��12��22:56:07
****************************************************/
uint8_t ModuleGetData(MODULE ModuleID, void* data,uint8_t len){
    ModuleBoard_T* Module = getModuleByModuleID(ModuleID);//�����ݻ��浽ģ�����ݴ洢�� 
    if(Module != NULL){
      if(Module->DataBuff != NULL){
        memcpy(data, Module->DataBuff, Module->DataSize);
        return Module->DataSize;
      }
    }
    return 0;
}
//ModuleBoard_T WaterMachine = {
//    "��ˮ��"
//};

//void Module_Init(void){
//    uint8_t i;
//    
//}