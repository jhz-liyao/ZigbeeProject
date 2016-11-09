#ifndef __PROTOCOLFRAME__H
#define __PROTOCOLFRAME__H 
#include "Queue.h" 
#include "Protocol.h"
#include "app_types.h"  
/*-----�ַ�ת��-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/




 

/*Э������  ����   ����  ת��*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
  



//---------------------------�����------------------------------------- 
typedef struct _PROTOCOL_INFO_T PROTOCOL_INFO_T ; 
typedef struct _PROTOCOL_DESC_T PROTOCOL_DESC_T ; 
typedef struct _Protocol_Resolver_T Protocol_Resolver_T;
/****************************************************
	�ṹ����:	PROTOCOL_DESC_T
	����: Э������
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
struct _PROTOCOL_DESC_T{ 
	MODULE_ACTION ModuleAction;
	MODULE TargetModule;
	MODULE SrcModule;
	u8 		ProtocolSize;	//�����ṹ���С
	u8		Serial;
	void (*send)(uint8_t* data, uint8_t len);
	void (*handle)(PROTOCOL_INFO_T*);
	int8_t (*check)(void*);
};

/****************************************************
	�ṹ����:	PROTOCOL_INFO_T
	����: Э����Ϣ����
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
struct _PROTOCOL_INFO_T{
	u8 head;		//֡ͷ
	u8 standby1;//Ԥ��
	u8 plen;		//֡��
	u8 module; //Ŀ��ģ��
	u8 serial;	//���
	u8 action;	//ָ���� 
	PROTOCOL_T protocol;//����
	u8 checksum;//У��� 
	u8 tail;		//֡β 
	u8 paralen;	//��������
	u8 alen;		//�ܳ���
	PROTOCOL_DESC_T* Desc_T;//Э������
	void (*handle)(PROTOCOL_INFO_T*);
	int8_t (*check)(void*);
};


/****************************************************
	�ṹ����:	Protocol_Resolver_T
	����: Э�������
	���ߣ�liyao 2016��9��7��17:55:45
****************************************************/
struct _Protocol_Resolver_T{ 
	QUEUE_T  *Protocol_Queue;
	uint16_t RPQueue_Size;
	PROTOCOL_INFO_T pi;   
	uint8_t  cnt; 				//������������
	uint8_t  index;
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

extern void ProtocolFrame_Init(SEND_INTERFACE SendHandle);
//extern PROTOCOL_INFO_T Create_Protocol_Info(int8_t len,SEND_ACTION type,void (*handle)(PROTOCOL_INFO_T*),int8_t (*check)(void*)); 
//int8_t Send_To_Uart(PROTOCOL_INFO_T* protocol_info);
//int8_t Send_To_Buff(PROTOCOL_INFO_T* protocol_info);
extern void Protocol_Send(MODULE_ACTION Module_Action,PROTOCOL_T* Protocol_t,u8 Len);
extern void Protocol_Send_Transpond(PROTOCOL_INFO_T* pi);
extern void FetchProtocols(void);
uint16_t char_special(uint8_t num);
extern int8_t Protocol_Register(PROTOCOL_DESC_T* Desc_T,PROTOCOL_TYPE Protocol_Type);
#endif
