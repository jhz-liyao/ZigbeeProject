#ifndef __WMPROTOCOLHANDLE_H__
#define __WMPROTOCOLHANDLE_H__ 
#include <stdio.h>
#include <string.h>
#include "ProtocolFrame.h"
//�ͻ���
extern void Cmd_P_Handle(Protocol_Info_T* pi);
extern void StateGet_P_Handle(Protocol_Info_T* pi); 

extern void ReportState(void);
//����
extern void HearBeat_P_Handle(Protocol_Info_T* pi);
extern void State_P_Handle(Protocol_Info_T* pi);
extern void Ack_P_Handle(Protocol_Info_T* pi);
extern void AddrReport_P_Handle(Protocol_Info_T* pi); 
#endif

