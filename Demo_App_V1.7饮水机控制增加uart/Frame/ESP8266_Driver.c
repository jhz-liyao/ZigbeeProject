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


QUEUE_T* ESP8266_CMD_Queue;

ESP8266_STATE ESP8266_State = ESP8266_BUSY; 
uint8_t ESP8266_CMD[ESP8266_CMD_LEN] = {0}; 
uint8_t ESP8266_CONN[CONN_COUNT] = {0}; 
uint8_t ESP_Change = 0;





void ESP8266_Init(void){
    ESP8266_CMD_Queue = Queue_Init(ESP8266_CMD , sizeof(uint8_t), ESP8266_CMD_LEN); 
    
    HalUARTWrite(HAL_UART_PORT_1,"AT+CIPMUX=1\r\n",13); 
    HalUARTWrite(HAL_UART_PORT_1,"AT+CIPSERVER=0,8080\r\n",12); 
    
}

void ESP8266_Poll(uint8_t* data, uint8_t len){
  if(strstr((char const*)data,"ready") != NULL){
    ESP8266_State = ESP8266_READY; 
    Log.waring("·�����������\r\n");  
  }else if(strstr((char const*)data,"WIFI GOT IP") != NULL){
    ESP8266_State = ESP8266_CONNECTED;
    Log.waring("·�������ӳɹ�\r\n"); 
    ESP8266_CMD_Send(ESP_8266_CMD.MultiConn);
    ESP8266_CMD_Send(ESP_8266_CMD.ServerMode);
  }else if(strstr((char const*)data,"WIFI DISCONNECT") != NULL){
    ESP8266_State = ESP8266_CONNFAIL;
    Log.waring("·��������ʧ��\r\n");
  }else if(strstr((char const*)data,"CONNECT") != NULL){
    ESP8266_CONN[(*data)-(uint8_t)'0'] = 1; 
    Log.waring("�ն�����\r\n");
  }else if(strstr((char const*)data,"CLOSED") != NULL){
    ESP8266_CONN[(*data)-(uint8_t)'0'] = 0; 
    Log.waring("�ն��˳�\r\n");
  }else if(strstr((char const*)data,"OK") != NULL){
    ESP8266_State = ESP8266_READY;
  }else if(strstr((char const*)data,"ERROR") != NULL){
    //ESP8266_State = ESP8266_ERROR; 
    //Log.waring("WIFIģ��ERROR\r\n"); 
  }
}



int8_t ESP8266_CMD_Send(uint8_t* CMD){ 
  uint8_t len = strlen((char const*)CMD),i;
  for(i = 0; i < len; i++)
    Queue_Put(ESP8266_CMD_Queue,CMD+i);
  return 0;
}

//int8_t ESP8266_Data_Send(uint8_t *data, uint8_t  len){
//    uint8_t i, j, cnt = 0;
//    uint8_t CMD[30] = {0};
//    for(i = 0; i < CONN_COUNT; i++){
//      if(ESP8266_CONN[i] == 1){ 
//          cnt = sprintf((char*)CMD,(char const *)ESP_8266_CMD.SendCMD,i,len);
//          for(j = 0; j < cnt; j++)
//            Queue_Put(ESP8266_CMD_Queue,&CMD[j]);
//          for(j = 0; j < len; j++)
//            Queue_Put(ESP8266_CMD_Queue,&data[j]);
//      }
//    }
//    if(cnt != 0)
//        HalUARTWrite(HAL_UART_PORT_1,ESP_8266_CMD.ServerMode,strlen((char const*)ESP_8266_CMD.ServerMode));
//    return 0;
//}

int8_t ESP8266_Data_Send(uint8_t *data, uint8_t  len){
    uint8_t i, j, cnt = 0;
    uint8_t CMD[30] = {0};
    for(i = 0; i < CONN_COUNT; i++){
      if(ESP8266_CONN[i] == 1){ 
          cnt = sprintf((char*)CMD,(char const *)ESP_8266_CMD.SendCMD,i,len);
          HalUARTWrite(HAL_UART_PORT_1,CMD,cnt);
//          MicroWait(100);
//          HalUARTWrite(HAL_UART_PORT_1,data,len);
//          for(j = 0; j < cnt; j++)
//            Queue_Put(ESP8266_CMD_Queue,&CMD[j]);
//          for(j = 0; j < len; j++)
//            Queue_Put(ESP8266_CMD_Queue,&data[j]);
      }
    }
//    if(cnt != 0)
//        HalUARTWrite(HAL_UART_PORT_1,ESP_8266_CMD.ServerMode,strlen((char const*)ESP_8266_CMD.ServerMode));
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
  if(ESP8266_State != ESP8266_BUSY && ESP8266_State != ESP8266_ERROR){
    uint8_t cmd[40] = {0},i = 0, data;
    while(Queue_Get(ESP8266_CMD_Queue,&data) == 0){
        cmd[i] = data;
        i++;
        if(data == '\n')
          break;
    }
    if(i != 0){ 
       ESP8266_State = ESP8266_BUSY;
       //printf("cmdlen %d\r\n", i);
       HalUARTWrite(HAL_UART_PORT_1,cmd,i); 
    }
  }else if(ESP8266_State == ESP8266_ERROR){
    //HalUARTWrite(HAL_UART_PORT_1,"AT+RST",strlen("AT+RST"));
    Log.error("ESP8266����ERROR");
  }else{
    //Log.waring("ESP8266 BUSY\r\n");
  } 
}

  
  
