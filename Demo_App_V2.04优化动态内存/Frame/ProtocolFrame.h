#ifndef __PROTOCOLFRAME__H
#define __PROTOCOLFRAME__H 
#include "Queue.h" 
#include "Protocol.h"
#include "app_types.h"  
/*-----�ַ�ת��-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/

#define PROTOCOL_SINGLE_BUFF 100
 
/*Э������  ����   ����  ת��*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
  



//---------------------------�����------------------------------------- 
typedef struct _Protocol_Info_T Protocol_Info_T ; 
typedef struct _Protocol_Desc_T Protocol_Desc_T ; 
typedef struct _Protocol_Resolver_T Protocol_Resolver_T;
/****************************************************
	�ṹ����:	Protocol_Desc_T
	����: Э������
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
struct _Protocol_Desc_T{ 
	MODULE_ACTION ModuleAction;
	MODULE TargetModule;
	MODULE SrcModule;
	u8 		ProtocolSize;	//�����ṹ���С
	u8		Serial;
	void (*Send)(uint8_t* data, uint8_t len);
	void (*Handle)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};

/****************************************************
	�ṹ����:	Protocol_Info_T
	����: Э����Ϣ����
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
struct _Protocol_Info_T{
	u8 Head;		//֡ͷ
	u8 Standby1;//Ԥ��
	u8 Plen;		//֡��
	u8 Module; //Ŀ��ģ��
	u8 Serial;	//���
	u8 Action;	//ָ���� 
	//PROTOCOL_T protocol;//����
	void* ParameterList;//����
	u8 CheckSum;//У��� 
	u8 Tail;		//֡β 
	u8 ParaLen;	//��������
	u8 AllLen;		//�ܳ���
	Protocol_Desc_T* ProtocolDesc;//Э������
	void (*Handle)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};


/****************************************************
	�ṹ����:	Protocol_Resolver_T
	����: Э�������
	���ߣ�liyao 2016��9��7��17:55:45
****************************************************/
struct _Protocol_Resolver_T{ 
	Queue_Head_T  *Protocol_Queue; 
	uint16_t RPQueue_Size;
	Protocol_Info_T pi;   
	uint8_t  Cnt; 		
        uint8_t  ParaData[PROTOCOL_SINGLE_BUFF];		//������������
	uint8_t  Index;
	int8_t 	 Is_FE;
	uint16_t CheckSum;
	uint8_t  Recv_State;
	int8_t (*Protocol_Put)(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
	void (*Fetch_Protocol)(Protocol_Resolver_T* pr);
};



//###################################������################################### 
#if UART1_PROTOCOL_RESOLVER
	extern Protocol_Resolver_T *UART1_Resolver; 
#endif
#if UART2_PROTOCOL_RESOLVER
	extern Protocol_Resolver_T *UART2_Resolver; 
#endif
#if UART3_PROTOCOL_RESOLVER
	extern Protocol_Resolver_T *UART3_Resolver; 
#endif
#if UART4_PROTOCOL_RESOLVER
	extern Protocol_Resolver_T *UART4_Resolver; 
#endif

extern void ProtocolFrame_Init();
//extern Protocol_Info_T Create_Protocol_Info(int8_t len,SEND_ACTION type,void (*handle)(Protocol_Info_T*),int8_t (*check)(void*)); 
//int8_t Send_To_Uart(Protocol_Info_T* protocol_info);
//int8_t Send_To_Buff(Protocol_Info_T* protocol_info); 
extern void Protocol_Send(MODULE_ACTION Module_Action,void* data,u8 Len);
extern void Protocol_Send_Transpond(Protocol_Info_T* pi);
extern void FetchProtocols(void);
uint16_t char_special(uint8_t num);
extern int8_t Protocol_Register(Protocol_Desc_T* ProtocolDesc,PROTOCOL_TYPE Protocol_Type);
#endif
