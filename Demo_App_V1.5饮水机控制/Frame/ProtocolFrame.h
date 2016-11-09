#ifndef __PROTOCOLFRAME__H
#define __PROTOCOLFRAME__H 
#include "Queue.h" 
#include "Protocol.h"
#include "app_types.h"  
/*-----字符转义-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/




 

/*协议类型  发送   接收  转发*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
  



//---------------------------框架区------------------------------------- 
typedef struct _PROTOCOL_INFO_T PROTOCOL_INFO_T ; 
typedef struct _PROTOCOL_DESC_T PROTOCOL_DESC_T ; 
typedef struct _Protocol_Resolver_T Protocol_Resolver_T;
/****************************************************
	结构体名:	PROTOCOL_DESC_T
	功能: 协议描述
	作者：liyao 2015年9月8日14:10:51
****************************************************/
struct _PROTOCOL_DESC_T{ 
	MODULE_ACTION ModuleAction;
	MODULE TargetModule;
	MODULE SrcModule;
	u8 		ProtocolSize;	//参数结构体大小
	u8		Serial;
	void (*send)(uint8_t* data, uint8_t len);
	void (*handle)(PROTOCOL_INFO_T*);
	int8_t (*check)(void*);
};

/****************************************************
	结构体名:	PROTOCOL_INFO_T
	功能: 协议信息描述
	作者：liyao 2015年9月8日14:10:51
****************************************************/
struct _PROTOCOL_INFO_T{
	u8 head;		//帧头
	u8 standby1;//预留
	u8 plen;		//帧长
	u8 module; //目标模块
	u8 serial;	//序号
	u8 action;	//指令码 
	PROTOCOL_T protocol;//参数
	u8 checksum;//校验和 
	u8 tail;		//帧尾 
	u8 paralen;	//参数长度
	u8 alen;		//总长度
	PROTOCOL_DESC_T* Desc_T;//协议描述
	void (*handle)(PROTOCOL_INFO_T*);
	int8_t (*check)(void*);
};


/****************************************************
	结构体名:	Protocol_Resolver_T
	功能: 协议解析器
	作者：liyao 2016年9月7日17:55:45
****************************************************/
struct _Protocol_Resolver_T{ 
	QUEUE_T  *Protocol_Queue;
	uint16_t RPQueue_Size;
	PROTOCOL_INFO_T pi;   
	uint8_t  cnt; 				//参数个数计数
	uint8_t  index;
	int8_t 	 Is_FE;
	uint16_t CheckSum;
	uint8_t  Recv_State;
	int8_t (*Protocol_Put)(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len);
	void (*Fetch_Protocol)(Protocol_Resolver_T* pr);
};



//###################################声明区################################### 
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
