#ifndef __DHT11_PROTOCOL__H__
#define __DHT11_PROTOCOL__H__
#include "app_types.h"  
/****************************************************
    结构体名:	DHT11_State_P_T
    功能:	状态上报
    作者：2016年12月20日
    例:		
****************************************************/
typedef struct{ 
    uint8_t para1;
    uint8_t para2; 
}DHT11_State_P_T; 
#endif

