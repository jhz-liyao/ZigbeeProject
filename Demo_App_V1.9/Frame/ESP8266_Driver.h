#ifndef __ESP8266_DRIVER_H__
#define __ESP8266_DRIVER_H__
#include "app_types.h"
#include <string.h>
#include "LOG.h"
#include "Queue.h"
#include "ProtocolFrame.h"

#define STATION_MODE 1
#define AP_MODE 2
#define AP_STATION_MODE 3

#define CONN_COUNT 5
#define LINE_DATA_SIZE 40


 
#define ESP8266_PACK_LEN 30
#define ESP8266_DATA_LEN 200
typedef enum{ ESP8266_BUSY, ESP8266_READY, ESP8266_SEND_CMD, ESP8266_SEND_DATA, ESP8266_ERROR, ESP8266_CONNECTED, ESP8266_CONNFAIL }ESP8266_STATE;
typedef enum{ ESP8266_INIT, ESP8266_CONF ,ESP8266_WORK }ESP8266_SECTION;


typedef struct {
  uint8_t* WorkMode;
  uint8_t* SsidPwd;
  uint8_t* MultiConn;
  uint8_t* ServerMode;
  uint8_t* SendCMD; 
}ESP8266_CMD_T;

typedef struct {
    uint8_t* data;
    uint8_t  len;
}ESP8266_Pack_T;


extern ESP8266_CMD_T ESP_8266_CMD;
extern void ESP8266_Init(void);
extern int8_t ESP8266_CMD_Send(uint8_t* CMD);
extern void ESP8266_Run(void);
extern void ESP8266_Poll(uint8_t* data, uint8_t len);
extern void SetSsidPwd(char* SSID, char* PWD);
extern void ESP8266_Data_Send(uint8_t *data, uint8_t len);
extern void ESP8266_Protocol_Send(PROTOCOL_INFO_T* pi);
#endif


