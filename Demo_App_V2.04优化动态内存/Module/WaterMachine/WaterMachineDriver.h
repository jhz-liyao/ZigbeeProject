#ifndef __WATERMACHINEDRIVER_H__
#define __WATERMACHINEDRIVER_H__ 
#include "ioCC2530.h"
#include "hal_defs.h"
#include "Protocol.h"
#include "ProtocolFrame.h"
#define JDQ_ENABLE      P0_4 = 1        //���ü̵�����
#define JDQ_DISABLE     P0_4 = 0        //���ü̵�����
#define PWRLED_ENABLE   P0_6 = 1        //���õ�Դָʾ�ƿ�
#define PWRLED_DISABLE  P0_6 = 0        //���õ�Դָʾ�ƹ�
#define TEMPCTRL_READ   P1_3            //��ȡ�¿�״̬
#define TEMPCTRL_ENABLE 1               //�¿�������Ҫ��ˮ
#define TEMPCTRL_DISABLE 0              //�¿ضϿ�ˮ���տ�

typedef enum{ WATERM_CLOSE = 0x00, WATERM_OPEN = 0x01 }CMD_WATERMACHINE;

typedef enum{   WATERM_AWAIT = 0x00, WATERM_RUN = 0x11, WATERM_FINISH = 0x10}RUN_STATE;
typedef struct {
    uint16_t Short_Addr;
    RUN_STATE State;
}WaterM_Info_T;

extern WaterM_Info_T WaterM ;
extern void WaterMachine_Init(void);
extern void State_Check(void);
extern void Cmd_P_Handle(Protocol_Info_T* pi);
#endif
