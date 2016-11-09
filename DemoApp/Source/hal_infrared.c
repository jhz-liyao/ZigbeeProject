#include "hal_infrared.h"
#include "stdio.h"



uint8 Infrared_Lock = 0;
void hal_infrared_init(void){
    HAL_INFRARED_SEL &= ~HAL_INFRARED_BIT;//set 0 普通IO
    HAL_INFRARED_DIR &= ~HAL_INFRARED_BIT;//set 0 输入
    
//    HAL_INFRARED_ICTL |= HAL_INFRARED_BIT;//set 1    脚 使能中断
//    HAL_INFRARED_IEN |= HAL_INFRARED_IENBIT;//set 1 组使能P0中断
//    
//    EA = 1;//使能总中断
    
}
 

void hal_infrared_poll(void){
  uint8 data = HAL_INFRARED_PORT & BV(4);
  
  if(data && Infrared_Lock == 0){
    printf("检测到人体\r\n");
    Infrared_Lock = 1;
  }else if(data == 0 && Infrared_Lock == 1){
    printf("未检测到\r\n");
    Infrared_Lock = 0;
  }
}
void hal_infrared_isr(void){
    printf("中断触发\r\n");
}

