#include "SoilSensorDriver.h"

#include "SoilSensorProtocol.h"
#include "ProtocolHandle.h"
#include "hal_adc.h"
SoilSensor_Info_T SoilSensorM;
/****************************************************
	������:	SoilSensor_Init
	����:	����ʪ�ȴ�����IO��س�ʼ��  
  ����:	2016��11��24��09:56:30
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