#include "DHT11_Driver.h"
 
#include "OnBoard.h" 
#include "DHT11.h"
DHT11_Info_T DHT11_Info ;
void DHT11_Init(void);
/****************************************************
	函数名:	DHT11_Init
	功能:	预留
  作者:	2016年12月20日
****************************************************/
void DHT11_Init(void){
  
}


void DHT11_Read(DHT11_Info_T* DHT11_Info_p){
  DHT11();
  DHT11_Info_p->Temperature = ucharT_data_H;
  DHT11_Info_p->Humidity = ucharRH_data_H;
}