#ifndef __SS_PROTOCOL__H__
#define __SS_PROTOCOL__H__
#include "app_types.h" 

//#ifndef ZDO_COORDINATOR 
//    #define RECV_PROTOCOL_NUM 3				//����Э��ʵ������
//    #define SEND_PROTOCOL_NUM 4				//����Э��ʵ������
//    #define TRANSPOND_PROTOCOL_NUM 1	                //ת��Э��ʵ������ 
//#endif
/****************************************************
    �ṹ����:	SolidSensor_State_P_T
    ����:	״̬�ϱ�
    ���ߣ�2016��12��2��
    ��:		
****************************************************/
typedef struct{
    uint8_t vcc;
    uint8_t para1;
    uint8_t para2;
    uint8_t para3;
    uint8_t para4;
    uint8_t para5;
    uint8_t para6;
    uint8_t para7;
}SolidSensor_State_P_T;


#endif

