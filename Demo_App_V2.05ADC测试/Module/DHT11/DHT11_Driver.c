#include "DHT11_Driver.h"
 
#include "OnBoard.h" 
#include "DHT11.h"
DHT11_Info_T DHT11_Info ;
void DHT11_Init(void);
/****************************************************
	������:	DHT11_Init
	����:	Ԥ��
  ����:	2016��12��20��
****************************************************/
void DHT11_Init(void){
  
}


void DHT11_Read(DHT11_Info_T* DHT11_Info_p){
  DHT11();
  DHT11_Info_p->Temperature = ucharT_data_H;
  DHT11_Info_p->Humidity = ucharRH_data_H;
}