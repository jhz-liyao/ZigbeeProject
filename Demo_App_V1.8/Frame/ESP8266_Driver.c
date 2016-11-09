#include "ESP8266_Driver.h"
#include "hal_uart.h"
#include "OnBoard.h"

ESP8266_CMD_T ESP_8266_CMD = {
    "AT+CWMODE=%d\r\n",
    "AT+CWJAP=\"%s\",\"%s\"\r\n",
    "AT+CIPMUX=1\r\n",
    "AT+CIPSERVER=1,8080\r\n",
    "AT+CIPSEND=%d,%d\r\n"
};


QUEUE_T* ESP8266_Pack_Queue;
QUEUE_T* ESP8266_Data_Queue;

//uint8_t ESP8266_Pack[ESP8266_PACK_LEN] = {0}; 
//uint8_t ESP8266_Data[ESP8266_DATA_LEN] = {0};


ESP8266_STATE ESP8266_State = ESP8266_BUSY; 
ESP8266_SECTION ESP8266_Section = ESP8266_INIT; 
uint8_t ESP8266_CONN[CONN_COUNT] = {0}; 
uint8_t ESP_Change = 0;

ESP8266_Pack_T pack = {0};




void ESP8266_Init(void){
    ESP8266_Pack_Queue = (QUEUE_T*)osal_mem_alloc(ESP8266_PACK_LEN * sizeof(ESP8266_Pack_T));//osal_mem_free
//    ESP8266_Pack_Queue = Queue_Init(ESP8266_Pack , sizeof(ESP8266_Pack_T), ESP8266_PACK_LEN/sizeof(ESP8266_Pack_T));
//    ESP8266_Data_Queue = Queue_Init(ESP8266_Data , sizeof(uint8_t), ESP8266_DATA_LEN); 
}

void ESP8266_Poll(uint8_t* data, uint8_t len){
  if(ESP8266_Section == ESP8266_INIT){
      if(strstr((char const*)data,"ready") != NULL || strstr((char const*)data,"OK") != NULL){
        Log.waring("路由器启动完成\r\n");   
        ESP8266_Section = ESP8266_CONF;
      }else if(strstr((char const*)data,"WIFI GOT IP") != NULL){
        ESP8266_State = ESP8266_CONNECTED;
        Log.waring("路由器连接成功\r\n");   
        ESP8266_Section = ESP8266_CONF;
      }else if(strstr((char const*)data,"WIFI DISCONNECT") != NULL){
        ESP8266_State = ESP8266_CONNFAIL;
        Log.waring("路由器连接失败\r\n"); 
        ESP8266_Section = ESP8266_CONF;
      }
  }else if(ESP8266_Section == ESP8266_WORK || ESP8266_Section == ESP8266_CONF ){
      if(strstr((char const*)data,"CONNECT") != NULL){
        ESP8266_CONN[(*data)-(uint8_t)'0'] = 1; 
        ESP8266_State = ESP8266_READY;
        Log.waring("终端连接\r\n");
      }else if(strstr((char const*)data,"CLOSED") != NULL){
        ESP8266_CONN[(*data)-(uint8_t)'0'] = 0; 
        Log.waring("终端退出\r\n");
      }else if(strstr((char const*)data,"OK") != NULL){
        ESP8266_State = ESP8266_READY;
      }else if(strstr((char const*)data,"ERROR") != NULL){
        //ESP8266_State = ESP8266_ERROR; 
        Log.waring("WIFI模块ERROR\r\n"); 
      } 
  } 
}


void ESP8266_Run(void){ 
  if(ESP8266_Section == ESP8266_INIT){
    return;
  }else if(ESP8266_Section == ESP8266_CONF){
    ESP8266_CMD_Send("AT+CIPMUX=1\r\n");
    ESP8266_CMD_Send("AT+CIPSERVER=0,8080\r\n");
    ESP8266_Section = ESP8266_WORK;
  }else if(ESP8266_Section == ESP8266_WORK){
     if(ESP8266_State == ESP8266_READY){
        uint8_t cmd[LINE_DATA_SIZE] = {0},i = 0; 
        if(Queue_Get(ESP8266_Pack_Queue,&pack) == 0){
            for(i = 0; i < pack.len; i++ )
              Queue_Get(pack.queue,&cmd[i]);
            
            ESP8266_State = ESP8266_BUSY;
            HalUARTWrite(HAL_UART_PORT_1,cmd,i);
        }  
     }
  } 
}

int8_t ESP8266_CMD_Send(uint8_t* CMD){ 
  uint8_t len = strlen((char const*)CMD);
  pack.len = len;
  pack.data = (uint8_t*)osal_mem_alloc(len);
  memcpy(pack.data, CMD, len); 
  Queue_Put(ESP8266_Pack_Queue, &pack);
  return 0;
}

int8_t ESP8266_Data_Send(uint8_t *data, uint8_t  len){
    uint8_t i; 
    for(i = 0; i < CONN_COUNT; i++){
      if(ESP8266_CONN[i] == 1){
          pack.data = osal_mem_alloc(strlen((char const*)ESP_8266_CMD.SendCMD) + 10);
          pack.len = sprintf((char*)pack.data,(char const *)ESP_8266_CMD.SendCMD,i,len); 
          Queue_Put(ESP8266_Pack_Queue, &pack); 
          
          pack.data = osal_mem_alloc(len);
          pack.len = len;
          memcpy(pack.data, data, len);
          Queue_Put(ESP8266_Pack_Queue, &pack); 
      }
    }
//    if(cnt != 0)
//        HalUARTWrite(HAL_UART_PORT_1,ESP_8266_CMD.ServerMode,strlen((char const*)ESP_8266_CMD.ServerMode));
    return 0;
}

//int8_t ESP8266_Data_Send(uint8_t *data, uint8_t  len){
//    uint8_t i, cnt = 0;
//    uint8_t CMD[30] = {0};
//    for(i = 0; i < CONN_COUNT; i++){
//      if(ESP8266_CONN[i] == 1){ 
//          cnt = sprintf((char*)CMD,(char const *)ESP_8266_CMD.SendCMD,i,len);
//          HalUARTWrite(HAL_UART_PORT_1,CMD,cnt);
////          MicroWait(100);
////          HalUARTWrite(HAL_UART_PORT_1,data,len);
////          for(j = 0; j < cnt; j++)
////            Queue_Put(ESP8266_CMD_Queue,&CMD[j]);
////          for(j = 0; j < len; j++)
////            Queue_Put(ESP8266_CMD_Queue,&data[j]);
//      }
//    }
////    if(cnt != 0)
////        HalUARTWrite(HAL_UART_PORT_1,ESP_8266_CMD.ServerMode,strlen((char const*)ESP_8266_CMD.ServerMode));
//    return 0;
//}

void SetSsidPwd(char* SSID, char* PWD){
  char Ssid[20] = {0};
  char  Pwd[20] = {0};
  memcpy(Ssid, SSID, strlen(SSID));
  memcpy(Pwd, PWD, strlen(PWD));
  uint8_t cmd[40] = {0}; 
  sprintf((char*)cmd, (char const*)ESP_8266_CMD.SsidPwd, Ssid, Pwd);
  ESP8266_CMD_Send(cmd);
  ESP8266_CMD_Send(ESP_8266_CMD.MultiConn);
  ESP8266_CMD_Send(ESP_8266_CMD.ServerMode);
}  
  
  


  
  
