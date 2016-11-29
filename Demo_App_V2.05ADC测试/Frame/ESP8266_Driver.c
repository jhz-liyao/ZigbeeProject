#include "ESP8266_Driver.h"
#include "hal_uart.h"
#include "OnBoard.h"
#include <stdio.h>

ESP8266_CMD_T ESP_8266_CMD = {
    "AT+CWMODE=%d\r\n",
    "AT+CWJAP=\"%s\",\"%s\"\r\n",
    "AT+CIPMUX=1\r\n",
    "AT+CIPSERVER=1,8080\r\n",
    "AT+CIPSEND=%d,%d\r\n"
};


//QUEUE_T* ESP8266_Pack_Queue;
Queue_Head_T* ESP8266_Queue = NULL;
uint8_t* ESP8266_Pack_Buff;

ESP8266_STATE ESP8266_State = ESP8266_BUSY; 
ESP8266_SECTION ESP8266_Section = ESP8266_INIT; 
//uint8_t ESP8266_CONN[CONN_COUNT] = {0}; 
uint8_t ESP_Change = 0;
//uint8_t ESP8266_Conn_Flag = 0;
ESP8266_Pack_T pack = {0};




void ESP8266_Init(void){
    //ESP8266_Pack_Buff = (uint8_t*)osal_mem_alloc(ESP8266_PACK_LEN * sizeof(ESP8266_Pack_T));
    //ESP8266_Pack_Queue = Queue_Init(ESP8266_Pack_Buff , sizeof(ESP8266_Pack_T), ESP8266_PACK_LEN);
    ESP8266_Queue = Queue_Link_Init(0);
    
}

void ESP8266_Poll(uint8_t* data, uint8_t len){
  if(ESP8266_Section == ESP8266_INIT){
      if(strstr((char const*)data,"ready") != NULL || strstr((char const*)data,"OK") != NULL){
        Log.waring("路由器启动完成\r\n");   
        ESP8266_Section = ESP8266_CONF;
        ESP8266_State = ESP8266_READY;
      }else if(strstr((char const*)data,"WIFI GOT IP") != NULL){ 
        Log.waring("路由器连接成功\r\n");
        ESP8266_Section = ESP8266_CONF;
        ESP8266_State = ESP8266_READY;
      }else if(strstr((char const*)data,"WIFI DISCONNECT") != NULL){
        ESP8266_State = ESP8266_CONNFAIL;
        Log.waring("路由器连接失败\r\n"); 
        ESP8266_Section = ESP8266_CONF;
      }
  }else if(ESP8266_Section == ESP8266_WORK || ESP8266_Section == ESP8266_CONF ){
      if(strstr((char const*)data,"CONNECT") != NULL){
//        ESP8266_CONN[(*data)-(uint8_t)'0'] = 1;
//        ESP8266_Conn_Flag |=  1 << ((*data)-(uint8_t)'0');
        ESP8266_State = ESP8266_READY;
        Log.waring("终端连接\r\n");
      }else if(strstr((char const*)data,"CLOSED") != NULL){ 
//        ESP8266_Conn_Flag &=  0 << ((*data)-(uint8_t)'0');
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
    ESP8266_CMD_Send("AT+CIPSTART=\"TCP\",\"198.55.114.161\",8081\r\n");
    ESP8266_Section = ESP8266_WORK;
  }else if(ESP8266_Section == ESP8266_WORK){
     if(ESP8266_State == ESP8266_READY){  
        uint8_t len = Queue_Link_OutSize(ESP8266_Queue);
        uint8_t* tmpData = osal_mem_alloc(len); 
        if(Queue_Link_Get(ESP8266_Queue,tmpData) == 0){
            //ESP8266_State = ESP8266_BUSY;
            HalUARTWrite(HAL_UART_PORT_1,tmpData, len);
//            HalUARTWrite(HAL_UART_PORT_0,pack.data, pack.len); 
//            printf("发送给WIFI:%X %X %X %X %X %X %X %X %X \r\n",tmpData[0],tmpData[1],tmpData[2],tmpData[3],
//                 tmpData[4],tmpData[5],tmpData[6],tmpData[7],tmpData[8]);
            
        }  
        osal_mem_free(tmpData);
     } 
  } 
}

int8_t ESP8266_CMD_Send(uint8_t* CMD){ 
//  uint8_t len = ;
//  pack.len = len;
//  pack.data = (uint8_t*)osal_mem_alloc(len);
//  memcpy(pack.data, CMD, len); 
//  Queue_Put(ESP8266_Pack_Queue, &pack);
//  
  Queue_Link_Put(ESP8266_Queue, CMD, strlen((char const*)CMD));
  
  return 0;
}

void ESP8266_Data_Send(uint8_t *data, uint8_t  len){  
    do{
       uint8_t tmpData[100] = {0};
       uint8_t cnt = sprintf((char *)tmpData, (char const *)"AT+CIPSEND=%d\r\n", len); 
       Queue_Link_Put(ESP8266_Queue, tmpData, cnt);
       Queue_Link_Put(ESP8266_Queue, data, len); 
                 printf("发送给WIFI:%X %X %X %X %X %X %X %X %X \r\n",data[0],data[1],data[2],data[3],
                 data[4],data[5],data[6],data[7],data[8]);
    }while(0);
}

void ESP8266_Protocol_Send(Protocol_Info_T* pi){
    pi->ProtocolDesc->Send = ESP8266_Data_Send;
    Protocol_Send_Transpond(pi);
    
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
  
  


  
  
