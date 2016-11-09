#include "WaterMachineDriver.h"

#include "WaterMachineProtocol.h"
#include "ProtocolHandle.h"

WaterM_Info_T WaterM;
/****************************************************
	������:	WaterMachine_Init
	����:	��ˮ��IO��س�ʼ��  
        ����:	2016��10��18��20:44:34
****************************************************/
void WaterMachine_Init(void){ 
  WaterM.State = WATERM_FINISH;
  WaterM.Short_Addr = 0;
  
  P0SEL &= ~BV(4);//�̵��� ������
  P0SEL &= ~BV(6);//ָʾ��
  P1SEL &= ~BV(0);//LED1 
  P1SEL &= ~BV(1);//LED2
  P1SEL &= ~BV(4);//LED3
  P1SEL &= ~BV(3);//�¿�����
   
  P0DIR |= BV(4);//�̵��� ������
  P0DIR |= BV(6);//ָʾ��
  P1DIR |= BV(0); 
  P1DIR |= BV(1);
  P1DIR |= BV(4); 
  P1DIR &= ~BV(3);//�¿����� 
  
  P1_0 = 1;
  P1_1 = 1;
  P1_4 = 1;
  
  JDQ_ENABLE;
  PWRLED_ENABLE; 
  
}

/****************************************************
	������:	WaterMachine_Init
	����:	��ˮ��IO��س�ʼ��  
        ����:	2016��10��18��20:44:34
****************************************************/
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

/****************************************************
	������:	Cmd_P_Handle
	����:	��������ָ�� 
        ����:	2016��10��18��20:44:34
****************************************************/
void Cmd_P_Handle(PROTOCOL_INFO_T* pi){
  printf("�յ���������\r\n");
  if(pi->protocol.Cmd_P.para1 == (uint8_t)WATERM_CLOSE){
    WaterM.State = WATERM_AWAIT;
    printf("�ǿعر�\r\n");
  }else if(pi->protocol.Cmd_P.para1 == (uint8_t)WATERM_OPEN){
    WaterM.State = WATERM_RUN;
    printf("�ǿؿ���\r\n");
  }
  ReportState();
  
  
}