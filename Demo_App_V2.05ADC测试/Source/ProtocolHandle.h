#ifndef __PROTOCOLHANDLE_H__
#define __PROTOCOLHANDLE_H__ 
#include <stdio.h>
#include <string.h>
#include "ProtocolFrame.h"
//����->����
extern void Cmd_P_Handle(Protocol_Info_T* pi);
extern void Water_Cmd_P_Handle(Protocol_Info_T* pi);
//�ն�->����
extern void HearBeat_P_Handle(Protocol_Info_T* pi);
extern void Water_State_P_Handle(Protocol_Info_T* pi);
extern void SolidSensor_State_P_Handle(Protocol_Info_T* pi); 
 
#endif

