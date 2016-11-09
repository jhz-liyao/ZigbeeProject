#include "ESP8266_Driver.h"
#include "hal_uart.h"

ESP8266_CMD_T ESP_8266_CMD = {
    "AT+CWMODE=%d\n",
    "AT+CWJAP=\"%s\",\"%s\"\n",
    "AT+CIPMUX=1\n",
    "AT+CIPSERVER=1,8080\n",
    "AT+SEND=%d,%d\n"
};


QUEUE_T* ESP8266_CMD_Queue;

ESP8266_STATE ESP8266_State = ESP8266_BUSY; 
uint8_t ESP8266_CMD[ESP8266_CMD_LEN] = {0}; 
uint8_t ESP_Change = 0;





void ESP8266_Init(void){
    ESP8266_CMD_Queue = Queue_Init(ESP8266_CMD , sizeof(uint8_t), ESP8266_CMD_LEN);
    ESP8266_CMD_Send(ESP_8266_CMD.MultiConn);
    ESP8266_CMD_Send(ESP_8266_CMD.ServerMode);
}

void ESP8266_Poll(uint8_t* data, uint8_t len){
  if(strstr((char const*)data,"ready") != NULL){
    ESP8266_State = ESP8266_READY; 
    Log.waring("路由器启动完成\r\n");  
  }else if(strstr((char const*)data,"WIFI GOT IP") != NULL){
    ESP8266_State = ESP8266_CONNECTED;
    Log.waring("路由器连接成功\r\n"); 
  }else if(strstr((char const*)data,"WIFI DISCONNECT") != NULL){
    ESP8266_State = ESP8266_CONNFAIL;
    Log.waring("路由器连接失败\r\n");
  }else if(strstr((char const*)data,"OK") != NULL){
    ESP8266_State = ESP8266_READY;
  }else if(strstr((char const*)data,"ERROR") != NULL){
    ESP8266_State = ESP8266_ERROR; 
    Log.waring("WIFI模块ERROR\r\n"); 
  }   
}



int8_t ESP8266_CMD_Send(uint8_t* CMD){ 
  uint8_t len = strlen((char const*)CMD),i;
  for(i = 0; i < len; i++)
    Queue_Put(ESP8266_CMD_Queue,&CMD[i]);
  return 0;
}

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
  
  
void ESP8266_Run(void){ 
  if(ESP8266_State != ESP8266_BUSY){
    uint8_t cmd[40] = {0},i = 0, data;
    while(Queue_Get(ESP8266_CMD_Queue,&data) == 0){
        cmd[i] = data;
        i++;
        if(data == '\n')
          break;
    }
    if(i != 0)
      HalUARTWrite(HAL_UART_PORT_1,cmd,strlen((char const*)cmd));
    ESP8266_State = ESP8266_BUSY;
  }else if(ESP8266_State == ESP8266_ERROR){
    HalUARTWrite(HAL_UART_PORT_1,"AT+RST",strlen("AT+RST"));
    Log.error("ESP8266遇到ERROR 重启");
  }else{
    //Log.waring("ESP8266 BUSY\r\n");
  } 
}

  
  
