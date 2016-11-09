#ifndef __TOOL_H__
#define __TOOL_H__

#include <stdio.h>
#include "hal_uart.h"
#include "MT.h"
#include "MT_UART.h" 
#include "ZDApp.h"

#include "AF.h"
#define LIYAO_ENDPOINT           20
#define LIYAO_GROUPID            0x0001


#define LIYAO_PROFID             0x0F08
#define LIYAO_DEVICEID           0x0001
#define LIYAO_DEVICE_VERSION     0
#define LIYAO_FLAGS              0

#define LIYAO_MAX_CLUSTERS       2
#define LIYAO_PERIODIC_CLUSTERID 1
#define LIYAO_FLASH_CLUSTERID    2


extern afAddrType_t Liyao_DstAddr;
extern void UART_Config_L(void);
extern void LiyaoApp_Init(void);
extern void SendDataToShortAddr(uint8 TaskID,uint16 shortAddr,unsigned char* data, uint16 len);
extern void ZdoStateChange_printf(devStates_t devStates);










#endif