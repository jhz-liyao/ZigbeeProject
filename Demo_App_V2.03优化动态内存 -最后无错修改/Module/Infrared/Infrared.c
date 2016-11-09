#include "hal_infrared.h"
#include "stdio.h"



uint8 Infrared_Lock = 0;
void hal_infrared_init(void){
    HAL_INFRARED_SEL &= ~HAL_INFRARED_BIT;//set 0 ��ͨIO
    HAL_INFRARED_DIR &= ~HAL_INFRARED_BIT;//set 0 ����
    
//    HAL_INFRARED_ICTL |= HAL_INFRARED_BIT;//set 1    �� ʹ���ж�
//    HAL_INFRARED_IEN |= HAL_INFRARED_IENBIT;//set 1 ��ʹ��P0�ж�
//    
//    EA = 1;//ʹ�����ж�
    
}
 

void hal_infrared_poll(void){
  uint8 data = HAL_INFRARED_PORT & BV(4);
  
  if(data && Infrared_Lock == 0){
    printf("��⵽����\r\n");
    Infrared_Lock = 1;
  }else if(data == 0 && Infrared_Lock == 1){
    printf("δ��⵽\r\n");
    Infrared_Lock = 0;
  }
}
void hal_infrared_isr(void){
    printf("�жϴ���\r\n");
}

