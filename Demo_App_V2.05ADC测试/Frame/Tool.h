#ifndef __TOOL_H__
#define __TOOL_H__

#include <stdio.h>
#include "hal_uart.h"
#include "MT.h"
#include "MT_UART.h" 
#include "ZDApp.h"

#include "AF.h"
#include "Queue.h"

#define SET(key, bit) key |= ((1<<bit));
#define RESET(key, bit) key &= (~(1<<bit));


#if defined(ZDO_COORDINATOR)
    #define COO TRUE
#elif defined(RTR_NWK)  && !defined(ZDO_COORDINATOR)
    #define ROT TRUE
#else
    #define EDV
#endif
      
      
#define LIYAO_ENDPOINT           20
#define LIYAO_GROUPID            0x0001


#define LIYAO_PROFID             0x0F08
#define LIYAO_DEVICEID           0x0001
#define LIYAO_DEVICE_VERSION     0
#define LIYAO_FLAGS              0

#define LIYAO_MAX_CLUSTERS       2
#define LIYAO_PERIODIC_CLUSTERID 1
#define LIYAO_FLASH_CLUSTERID    2

extern Queue_Head_T* WifiToUart_Queue;
extern Queue_Head_T* UartToWifi_Queue;
extern Queue_Head_T* DeviceRecv_Queue;

extern afAddrType_t Liyao_DstAddr;
extern void Device_Info(void);
extern void UART_Config_1(void);
extern void UART_Config_2(void);
extern void SendDataToShortAddr(uint8 TaskID,uint16 shortAddr,unsigned char* data, uint16 len);
extern void ZdoStateChange_printf(devStates_t devStates);
extern void Meter_Leave(void);
extern void Protocol_Printf(uint8 *data, uint8 len);








#endif