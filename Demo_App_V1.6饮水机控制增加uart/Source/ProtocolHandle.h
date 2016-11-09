#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include <stdio.h>
#include <string.h>
#include "ProtocolFrame.h"
//¿Í»§¶Ë
extern void Cmd_P_Handle(PROTOCOL_INFO_T* pi);
extern void StateGet_P_Handle(PROTOCOL_INFO_T* pi); 

//Ö÷¿Ø
extern void HearBeat_P_Handle(PROTOCOL_INFO_T* pi);
extern void State_P_Handle(PROTOCOL_INFO_T* pi);
extern void Ack_P_Handle(PROTOCOL_INFO_T* pi);
extern void AddrReport_P_Handle(PROTOCOL_INFO_T* pi); 
#endif

