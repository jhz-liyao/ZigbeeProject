#include "WaterMachineDriver.h"

#include "WaterMachineProtocol.h"
#include "ProtocolHandle.h"

WaterM_Info_T WaterM;
/****************************************************
	函数名:	WaterMachine_Init
	功能:	饮水机IO相关初始化  
        作者:	2016年10月18日20:44:34
****************************************************/
void WaterMachine_Init(void){ 
  WaterM.State = WATERM_FINISH;
  WaterM.Short_Addr = 0;
  
  P0SEL &= ~BV(4);//继电器 工作灯
  P0SEL &= ~BV(6);//指示灯
  P1SEL &= ~BV(0);//LED1 
  P1SEL &= ~BV(1);//LED2
  P1SEL &= ~BV(4);//LED3
  P1SEL &= ~BV(3);//温控输入
   
  P0DIR |= BV(4);//继电器 工作灯
  P0DIR |= BV(6);//指示灯
  P1DIR |= BV(0); 
  P1DIR |= BV(1);
  P1DIR |= BV(4); 
  P1DIR &= ~BV(3);//温控输入 
  
  P1_0 = 1;
  P1_1 = 1;
  P1_4 = 1;
  
  JDQ_ENABLE;
  PWRLED_ENABLE; 
  
}

/****************************************************
	函数名:	WaterMachine_Init
	功能:	饮水机IO相关初始化  
        作者:	2016年10月18日20:44:34
****************************************************/
void State_Check(void){
  RUN_STATE oldstate = WaterM.State;
  if(WaterM.State != WATERM_AWAIT){//如果没有关闭电源智控 
    PWRLED_ENABLE;
    if(TEMPCTRL_READ == TEMPCTRL_ENABLE){//如果温控数据需要加热
        JDQ_ENABLE;//打开继电器所控电阻丝
        WaterM.State = WATERM_RUN;
        //printf("电阻丝加热工作\r\n");
    }else{
        JDQ_DISABLE;//关闭继电器所控电阻丝
        WaterM.State = WATERM_FINISH;
        //printf("电阻丝加热停止\r\n");
    }
  }else{
    JDQ_DISABLE;
    PWRLED_DISABLE;
  }
  if(WaterM.State != oldstate)
    ReportState();
}

/****************************************************
	函数名:	Cmd_P_Handle
	功能:	接收主控指令 
        作者:	2016年10月18日20:44:34
****************************************************/
void Cmd_P_Handle(PROTOCOL_INFO_T* pi){
  printf("收到控制命令\r\n");
  if(pi->protocol.Cmd_P.para1 == (uint8_t)WATERM_CLOSE){
    WaterM.State = WATERM_AWAIT;
    printf("智控关闭\r\n");
  }else if(pi->protocol.Cmd_P.para1 == (uint8_t)WATERM_OPEN){
    WaterM.State = WATERM_RUN;
    printf("智控开启\r\n");
  }
  ReportState();
  
  
}