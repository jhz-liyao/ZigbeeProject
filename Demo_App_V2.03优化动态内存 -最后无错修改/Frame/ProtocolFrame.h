#ifndef __PROTOCOLFRAME__H
#define __PROTOCOLFRAME__H 
#include "Queue.h" 
#include "Protocol.h"
#include "app_types.h"  
/*-----字符转义-----
FD->FE 7D
F8->FE 78
FE->FE 7E		*/

#define PROTOCOL_SINGLE_BUFF 100
 
/*协议类型  发送   接收  转发*/
typedef enum{ SEND, RECEIVE, TRANSPOND }PROTOCOL_TYPE;
  



//---------------------------框架区------------------------------------- 
typedef struct _Protocol_Info_T Protocol_Info_T ; 
typedef struct _Protocol_Desc_T Protocol_Desc_T ; 
typedef struct _Protocol_Resolver_T Protocol_Resolver_T;
/****************************************************
	结构体名:	Protocol_Desc_T
	功能: 协议描述
	作者：liyao 2015年9月8日14:10:51
****************************************************/
struct _Protocol_Desc_T{ 
	MODULE_ACTION ModuleAction;
	MODULE TargetModule;
	MODULE SrcModule;
	u8 		ProtocolSize;	//参数结构体大小
	u8		Serial;
	void (*Send)(uint8_t* data, uint8_t len);
	void (*Handle)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};

/****************************************************
	结构体名:	Protocol_Info_T
	功能: 协议信息描述
	作者：liyao 2015年9月8日14:10:51
****************************************************/
struct _Protocol_Info_T{
	u8 Head;		//帧头
	u8 Standby1;//预留
	u8 Plen;		//帧长
	u8 Module; //目标模块
	u8 Serial;	//序号
	u8 Action;	//指令码 
	//PROTOCOL_T protocol;//参数
	void* ParameterList;//参数
	u8 CheckSum;//校验和 
	u8 Tail;		//帧尾 
	u8 ParaLen;	//参数长度
	u8 AllLen;		//总长度
	Protocol_Desc_T* ProtocolDesc;//协议描述
	void (*Handle)(Protocol_Info_T*);
	int8_t (*Check)(void*);
};


/****************************************************
	结构体名:	Protocol_Resolver_T
	功能: 协议解析器
	作者：liyao 2016年9月7日17:55:45
****************************************************/
struct _Protocol_Resolver_T{ 
	Queue_Head_T  *Protocol_Queue; 
	uint16_t RPQueue_Size;
	Protocol_Info_T pi;   
	uint8_t  Cnt; 		
        uint8_t  ParaData[PROTOCOL_SINGLE_BUFF];		//参数个数计数
	uint8_t  Index;
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
