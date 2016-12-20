#ifndef __DHT11_DRIVER_H__
#define __DHT11_DRIVER_H__ 
#include "ioCC2530.h"
#include "hal_defs.h"
#include "Protocol.h"
#include "ProtocolFrame.h"  
 
typedef struct { 
    uint8_t Temperature;//�¶�
    uint8_t Humidity;   //ʪ��
}DHT11_Info_T;

extern DHT11_Info_T DHT11_Info ;
extern void DHT11_Init(void);
extern void DHT11_Read(DHT11_Info_T* DHT11_Info_p); 
#endif
