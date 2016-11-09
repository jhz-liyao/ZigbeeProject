#ifndef __ESP8266_DRIVER_H__
#define __ESP8266_DRIVER_H__
#include "app_types.h"
#include <string.h>
#include "LOG.h"
#include "Queue.h"

#define STATION_MODE 1
#define AP_MODE 2
#define AP_STATION_MODE 3

#define CONN_COUNT 5


 
#define ESP8266_CMD_LEN 100
typedef enum{ ESP8266_INIT, ESP8266_BUSY, ESP8266_READY, ESP8266_ERROR, ESP8266_CONNECTED, ESP8266_CONNFAIL }ESP8266_STATE;

typedef struct {
  uint8_t* WorkMode;
  uint8_t* SsidPwd;
  uint8_t* MultiConn;
  uint8_t* ServerMode;
  uint8_t* SendCMD; 
}ESP8266_CMD_T;


extern ESP8266_CMD_T ESP_8266_CMD;
extern void ESP8266_Init(void);
extern int8_t ESP8266_CMD_Send(uint8_t* CMD);
extern void ESP8266_Run(void);
extern void ESP8266_Poll(uint8_t* data, uint8_t len);
extern void SetSsidPwd(char* SSID, char* PWD);
extern int8_t ESP8266_Data_Send(uint8_t *data, uint8_t len);
#endif


