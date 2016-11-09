#ifndef __TOOL_H
#define __TOOL_H
#include <stdio.h> 
#include "app_types.h"

//###################################����###################################
#define QUEUE_MAXNUM 5

/****************************************************
	�ṹ����:	QUEUE_T
	����:	����ͷʵ��
	���ߣ�liyao 2015��9��8��14:10:51
****************************************************/
typedef struct{
	void* data;
	
	uint8_t single_size;
	uint16_t count;
	
	uint16_t start;
	uint16_t end;
	uint16_t num;
	uint8_t full_flag;
	
	uint8_t use_state; 
	u8 locked;
}QUEUE_T;

extern QUEUE_T* Queue_Init(void* _array,uint8_t _single_size ,uint16_t _count);
/*extern int8_t queue_put(QUEUE_T* , int32_t );
extern int8_t queue_get(QUEUE_T* , int32_t* );*/
extern int8_t Queue_Put(QUEUE_T* queue,void* _data);
extern int8_t Queue_Get(QUEUE_T* queue,void* _data);
extern uint16_t Queue_Size(QUEUE_T* queue);
extern uint16_t Queue_Empty_size(QUEUE_T* queue);
extern void Queue_Free(QUEUE_T* queue);




#endif
