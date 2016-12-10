#include "SoilSensorDriver.h"

#include "SoilSensorProtocol.h"
#include "ProtocolHandle.h"
#include "hal_adc.h"
#include "OnBoard.h"
SoilSensor_Info_T SoilSensorM;
/****************************************************
	������:	SoilSensor_Init
	����:	����ʪ�ȴ�����IO��س�ʼ��  
  ����:	2016��11��24��09:56:30
****************************************************/
void SoilSensor_Init(void){
  P0SEL = 0x00; 
  P0DIR = 0xff; 
  P0 = 0xff; 
  P1SEL = P1SEL & ~BV(0);
  P1DIR = P1DIR | BV(0);
  P1 = P1 & ~BV(0); 
}

void SoilSensor_AdcConfig(void){
  P1 = P1 | BV(0); 
}
void SoilSensor_Read(void){
  SolidSensor_State_P_T SolidSensor_State = {0};
  SoilSensor_AdcConfig(); 
  for(int i = 0; i < 8; i++){
    uint16_t adc_value = HalAdcRead(i,HAL_ADC_RESOLUTION_14);  
    PERCENT_VALUE(((uint8* )&SolidSensor_State)[i], adc_value);
    printf("%d %d %d \r\n", i, ((uint8* )&SolidSensor_State)[i], adc_value); 
  } 
  SoilSensor_Init();
  Protocol_Send(SOIL_SENSOR_STATE_PROTOCOL, &SolidSensor_State, sizeof(SolidSensor_State_P_T));
}
/****************************************************
	������:	WaterMachine_Init
	����:	��ˮ��IO��س�ʼ��  
        ����:	2016��10��18��20:44:34
***************************************************
void State_Check(void){
  RUN_STATE oldstate = WaterM.State;
  if(WaterM.State != WATERM_AWAIT){//���û�йرյ�Դ�ǿ� 
    PWRLED_ENABLE;
    if(TEMPCTRL_READ == TEMPCTRL_ENABLE){//����¿�������Ҫ����
        JDQ_ENABLE;//�򿪼̵������ص���˿
        WaterM.State = WATERM_RUN;
        //printf("����˿���ȹ���\r\n");
    }else{
        JDQ_DISABLE;//�رռ̵������ص���˿
        WaterM.State = WATERM_FINISH;
        //printf("����˿����ֹͣ\r\n");
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
	������:	Cmd_P_Handle
	����:	��������ָ�� 
        ����:	2016��10��18��20:44:34
***************************************************
void Cmd_P_Handle(Protocol_Info_T* pi){
  CMD_PROTOCOL_T* cp = pi->ParameterList;
  printf("�յ���������\r\n");
  if(cp->para1 == (uint8_t)WATERM_CLOSE){
    WaterM.State = WATERM_AWAIT;
    printf("�ǿعر�\r\n");
  }else if(cp->para1 == (uint8_t)WATERM_OPEN){
    WaterM.State = WATERM_RUN;
    printf("�ǿؿ���\r\n");
  }
  ReportState();
  
  
}*/