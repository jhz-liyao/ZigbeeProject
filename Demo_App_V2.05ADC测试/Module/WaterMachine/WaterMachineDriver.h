#ifndef __WATERMACHINEDRIVER_H__
#define __WATERMACHINEDRIVER_H__ 
#include "ioCC2530.h"
#include "hal_defs.h"
#include "Protocol.h"
#include "ProtocolFrame.h"
#define JDQ_ENABLE      P0_4 = 1        //设置继电器开
#define JDQ_DISABLE     P0_4 = 0        //设置继电器关
#define PWRLED_ENABLE   P0_6 = 1        //设置电源指示灯开
#define PWRLED_DISABLE  P0_6 = 0        //设置电源指示灯关
#define TEMPCTRL_READ   P1_3            //读取温控状态
#define TEMPCTRL_ENABLE 1               //温控启动需要烧水
#define TEMPCTRL_DISABLE 0              //温控断开水已烧开

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
