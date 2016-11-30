#ifndef __PROTOCOL__H
#define __PROTOCOL__H
#include "app_types.h" 
 

#define PROTOCOL_NUM 10
#if defined(ZDO_COORDINATOR)
    #define RECV_PROTOCOL_NUM 4				//����Э��ʵ������
    #define SEND_PROTOCOL_NUM 3				//����Э��ʵ������
    #define TRANSPOND_PROTOCOL_NUM 1	                //ת��Э��ʵ������ 
#endif

//###################################��������Э����###################################

/****************************************************
    �ṹ����:	HEARTBEAT_PROTOCOL_T
    ����:	����Э��ʵ��
    ���ߣ�      2016��10��18��17:03:41
    ��:		
****************************************************/
typedef struct{
    uint8_t para1;//������
    uint8_t para2;//��ַ��8λ
    uint8_t para3;//��ַ��8λ
}HEARTBEAT_PROTOCOL_T;
 
/****************************************************
    �ṹ����:	STATE_PROTOCOL_T
    ����:	״̬�ϱ�
    ���ߣ�      2016��10��18��17:50:45
    ��:		
****************************************************/
typedef struct{
    uint8_t para1;//״̬�ϱ�
}STATE_PROTOCOL_T;

/****************************************************
	�ṹ����:ASK_PROTOCOL_T
	����: Ӧ��Э��ʵ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
    uint8_t para1;//�������
}ACK_PROTOCOL_T;

/****************************************************
	�ṹ����:ADDRREPOT_PROTOCOL_T
	����: �ͻ����ϱ���ַ
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
    uint8_t para1;//��ַ��8λ
    uint8_t para2;//��ַ��8λ
}ADDRREPORT_PROTOCOL_T;



//###################################��������Э����###################################

/****************************************************
    �ṹ����:	CMD_PROTOCOL_T
    ����:	����ָ��
    ���ߣ�      2016��10��18��17:50:45
    ��:		
****************************************************/
typedef struct{
    uint8_t para1;//����ָ��
    uint8_t para2;//����
}CMD_PROTOCOL_T;

/****************************************************
    �ṹ����:	STATEGET_PROTOCOL_T
    ����:	״̬��ȡ
    ���ߣ�      2016��10��18��17:50:45
    ��:		
****************************************************/
typedef struct{
    uint8_t para1;//״̬��ȡ
    uint8_t para2;//����
}STATEGET_PROTOCOL_T;

//###################################���ط���Э����###################################


//###################################���ؽ���Э����###################################








//----------------------������--------------------------------
//typedef union 
//{
//    HEARTBEAT_PROTOCOL_T    Heartbeat_P;
//    STATE_PROTOCOL_T        State_P;
//    ACK_PROTOCOL_T          Ack_P;
//    ADDRREPORT_PROTOCOL_T    AddrReport_P;
//    
//    CMD_PROTOCOL_T          Cmd_P;
//    STATEGET_PROTOCOL_T     StateGet_P;
//}PROTOCOL_T;
 
/*ģ����*/
typedef enum{ 
	COORDINATOR_MODULE	 = 0x00,
	WATER_MODULE         = 0x01, 
  SOIL_SENSOR_MODULE   = 0x02,
}MODULE;
/*Э��Ŀ��弰����*/
typedef enum { 	
	WATER_HEARTBEAT_PROTOCOL  = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0x31,
	SOIL_SENSOR_HEARTBEAT_PROTOCOL  = (SOIL_SENSOR_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0x31, 
  
	STATE_PROTOCOL      = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0x20,
        ACK_PROTOCOL        = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0xA1,
        ADDRREPORT_PROTOCOL = (WATER_MODULE << 4 | COORDINATOR_MODULE) <<8 | 0xB1,
        
        CMD_PROTOCOL        = (COORDINATOR_MODULE << 4 | WATER_MODULE) <<8 | 0x01,
        STATEGET_PROTOCOL   = (COORDINATOR_MODULE << 4 | WATER_MODULE) <<8 | 0x02,
}MODULE_ACTION;

typedef void(*SEND_INTERFACE)(uint8_t* data, uint8_t len);
extern void Protocol_Init(void);
































#endif

