#ifndef __SS_PROTOCOL__H__
#define __SS_PROTOCOL__H__
#include "app_types.h" 

//#ifndef ZDO_COORDINATOR 
//    #define RECV_PROTOCOL_NUM 3				//接收协议实体数量
//    #define SEND_PROTOCOL_NUM 4				//发送协议实体数量
//    #define TRANSPOND_PROTOCOL_NUM 1	                //转发协议实体数量 
//#endif
/****************************************************
    结构体名:	SolidSensor_State_P_T
    功能:	状态上报
    作者：2016年12月2日
    例:		
****************************************************/
typedef struct{
    uint8_t vcc;
    uint8_t para1;
    uint8_t para2;
    uint8_t para3;
    uint8_t para4;
    uint8_t para5;
    uint8_t para6;
    uint8_t para7;
}SolidSensor_State_P_T;


#endif

