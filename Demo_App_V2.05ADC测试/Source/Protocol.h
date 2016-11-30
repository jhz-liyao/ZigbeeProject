#ifndef __PROTOCOL__H
#define __PROTOCOL__H
#include "app_types.h" 
 

#define PROTOCOL_NUM 10
#if defined(ZDO_COORDINATOR)
    #define RECV_PROTOCOL_NUM 4				//接收协议实体数量
    #define SEND_PROTOCOL_NUM 3				//发送协议实体数量
    #define TRANSPOND_PROTOCOL_NUM 1	                //转发协议实体数量 
#endif

//###################################公共发送协议类###################################

/****************************************************
    结构体名:	HEARTBEAT_PROTOCOL_T
    功能:	心跳协议实体
    作者：      2016年10月18日17:03:41
    例:		
****************************************************/
typedef struct{
    uint8_t para1;//心跳号
    uint8_t para2;//地址高8位
    uint8_t para3;//地址低8位
}HEARTBEAT_PROTOCOL_T;
 
/****************************************************
    结构体名:	STATE_PROTOCOL_T
    功能:	状态上报
    作者：      2016年10月18日17:50:45
    例:		
****************************************************/
typedef struct{
    uint8_t para1;//状态上报
}STATE_PROTOCOL_T;

/****************************************************
	结构体名:ASK_PROTOCOL_T
	功能: 应答协议实体
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
    uint8_t para1;//接收序号
}ACK_PROTOCOL_T;

/****************************************************
	结构体名:ADDRREPOT_PROTOCOL_T
	功能: 客户端上报地址
	作者：liyao 2015年9月8日14:10:51
****************************************************/
typedef struct{
    uint8_t para1;//地址高8位
    uint8_t para2;//地址低8位
}ADDRREPORT_PROTOCOL_T;



//###################################公共接收协议类###################################

/****************************************************
    结构体名:	CMD_PROTOCOL_T
    功能:	控制指令
    作者：      2016年10月18日17:50:45
    例:		
****************************************************/
typedef struct{
    uint8_t para1;//控制指令
    uint8_t para2;//余留
}CMD_PROTOCOL_T;

/****************************************************
    结构体名:	STATEGET_PROTOCOL_T
    功能:	状态获取
    作者：      2016年10月18日17:50:45
    例:		
****************************************************/
typedef struct{
    uint8_t para1;//状态获取
    uint8_t para2;//余留
}STATEGET_PROTOCOL_T;

//###################################主控发送协议类###################################


//###################################主控接收协议类###################################








//----------------------联合体--------------------------------
//typedef union 
//{
//    HEARTBEAT_PROTOCOL_T    Heartbeat_P;
//    STATE_PROTOCOL_T        State_P;
//    ACK_PROTOCOL_T          Ack_P;
//    ADDRREPORT_PROTOCOL_T    AddrReport_P;
//    
//    CMD_PROTOCOL_T          Cmd_P;
//    STATEGET_PROTOCOL_T     StateGet_P;
//}PROTOCOL_T;
 
/*模块编号*/
typedef enum{ 
	COORDINATOR_MODULE	 = 0x00,
	WATER_MODULE         = 0x01, 
  SOIL_SENSOR_MODULE   = 0x02,
}MODULE;
/*协议目标板及动作*/
typedef enum { 	
	WATER_HEARTBEAT_PROTOCOL  = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0x31,
	SOIL_SENSOR_HEARTBEAT_PROTOCOL  = (SOIL_SENSOR_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0x31, 
  
	STATE_PROTOCOL      = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0x20,
        ACK_PROTOCOL        = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0xA1,
        ADDRREPORT_PROTOCOL = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0xB1,
        
        CMD_PROTOCOL        = (COORDINATOR_MODULE << 4 | WATER_MODULE) <<8 | 0x01,
        STATEGET_PROTOCOL   = (COORDINATOR_MODULE << 4 | WATER_MODULE) <<8 | 0x02,
}MODULE_ACTION;

typedef void(*SEND_INTERFACE)(uint8_t* data, uint8_t len);
extern void Protocol_Init(void);
































#endif

