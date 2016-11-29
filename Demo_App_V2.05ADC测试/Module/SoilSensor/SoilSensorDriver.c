#include "SoilSensorDriver.h"

#include "SoilSensorProtocol.h"
#include "ProtocolHandle.h"
#include "hal_adc.h"
SoilSensor_Info_T SoilSensorM;
/****************************************************
	函数名:	SoilSensor_Init
	功能:	土壤湿度传感器IO相关初始化  
  作者:	2016年11月24日09:56:30
****************************************************/
void SoilSensor_Init(void){ 
  HalAdcInit();
}

void SoilSensor_Read(void){
  uint16 val[8] = {0};
  for(int i = 0; i < 8; i++){
    val[i] = HalAdcRead(i,HAL_ADC_RESOLUTION_14);
    printf("%d ", val[i]);
  }
  printf("\r\n");
}
/****************************************************
	函数名:	WaterMachine_Init
	功能:	饮水机IO相关初始化  
        作者:	2016年10月18日20:44:34
***************************************************
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
*/
/****************************************************
	函数名:	Cmd_P_Handle
	功能:	接收主控指令 
        作者:	2016年10月18日20:44:34
***************************************************
void Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* cp = pi->ParameterList;
  printf("收到控制命令\r\n");
  if(cp->para1 == (uint8_t)WATERM_CLOSE){
    WaterM.State = WATERM_AWAIT;
    printf("智控关闭\r\n");
  }else if(cp->para1 == (uint8_t)WATERM_OPEN){
    WaterM.State = WATERM_RUN;
    printf("智控开启\r\n");
  }
  ReportState();
  
  
}*/