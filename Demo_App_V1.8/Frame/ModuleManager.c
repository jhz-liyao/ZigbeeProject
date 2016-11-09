#include "ModuleManager.h"
#include <stdio.h>
#include <string.h>
#include "hal_defs.h"
ModuleBoard_T ModuleList[] = {
  {"����", COORDINATOR_MODULE, OFFLINE ,0 ,0 },
  {"��ˮ��", WATER_MODULE    , OFFLINE ,0 ,0 },
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
        ModuleList[i].ShortAddr = 0; 
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
//ModuleBoard_T WaterMachine = {
//    "��ˮ��"
//};

//void Module_Init(void){
//    uint8_t i;
//    
//}