#include <stdio.h>
#include <string.h>
#include "Queue.h"  
/*
 *���߰�
 *author����Ң 
 *time��2015��8��26��10:41:23
 */

//###################################������غ���###################################

QUEUE_T queue_list[QUEUE_MAXNUM] = {0}; 
u8 queue_locd = 0;
/****************************************************
	������:	queue_init
	����:	���г�ʼ��
	����:	�����������鵥��Ԫ�ش�С������Ԫ�ظ���
	����ֵ:	���о�� �� NULL
	����:	liyao 2015��9��8��14:10:51
****************************************************/
QUEUE_T* Queue_Init(void* _array,uint8_t _single_size ,uint16_t _count){ 
	uint8_t i = 0;
	QUEUE_T* queue = NULL;
	for(i = 0; i < QUEUE_MAXNUM; i++)
		if(queue_list[i].use_state == 0)
			queue = &queue_list[i];
	if(queue == NULL)
		return NULL;
	queue->use_state = 1;
	queue->single_size = _single_size;
	queue->count = _count;
	queue->start = queue->end = queue->num = queue->full_flag =0; 
	queue->data = _array;
	return queue;
}

/****************************************************
	������:	queue_put
	����:	���ݲ��뵽����
	����:	���о��������
	����:	liyao 2015��9��8��14:10:51
****************************************************/
/*int8_t queue_put(QUEUE_T* queue,int32* _data){
		if(queue->num == queue->count)  
			return -1;
		switch(queue->single_size){ 
			case 1:((int8_t*)queue->data)[queue->start] = (int8_t)_data ;	break;
			case 2:((int16_t*)queue->data)[queue->start] = (int16_t)_data;	break;
			case 4:((int32_t*)queue->data)[queue->start] = (int32_t)_data;	break;
			default:return -1;
		}
		queue->start = ((++queue->start) % queue->count);
		queue->num ++;
		return 0;
}*/

/****************************************************
	������:	queue_get
	����:	�Ӷ���ȡ������
	����:	���о�����������������
	����:	liyao 2015��9��8��14:10:51
****************************************************/
/*int8_t queue_get(QUEUE_T* queue, int32_t* _data){
		if(queue->num == 0) 
			return -1;
		switch(queue->single_size){
			case 1:*_data = ((int8_t*)queue->data)[queue->end];
										  ((int8_t*)queue->data)[queue->end] = 0;	break;
			case 2:*_data = ((int16_t*)queue->data)[queue->end];
											((int16_t*)queue->data)[queue->end] = 0;			break;
			case 4:*_data = ((int32_t*)queue->data)[queue->end];
											((int32_t*)queue->data)[queue->end] = 0;break;
			default:return -1;
		}
		queue->end = ((++queue->end) % queue->count);
		queue->num --;
		return 0;
}*/
#define QUEUE_LOCK do{if(queue->locked == 1) return -1;else queue->locked = 1;}while(0);
#define QUEUE_UNLOCK do{queue->locked = 0;}while(0);
/****************************************************
	������:	queue_put 
	����:	���ݲ��뵽����
	����:	���о��������
	����:	liyao 2015��9��8��14:10:51
****************************************************/
int8_t Queue_Put(QUEUE_T* queue,void* _data){
		if(queue->full_flag == 1)  
			return -1;
		//QUEUE_LOCK;
		memcpy((char*)queue->data + (queue->start * queue->single_size),_data,queue->single_size);
		queue->start = ((++queue->start) % queue->count);
		if(queue->start == queue->end)
			queue->full_flag = 1;
		//queue->num ++;
		//QUEUE_UNLOCK
		return 0;
}

/****************************************************
	������:	queue_get
	����:	�Ӷ���ȡ������
	����:	���о�����������������
	����:	liyao 2015��9��8��14:10:51
****************************************************/
int8_t Queue_Get(QUEUE_T* queue, void* _data){
		if(queue->full_flag == 0 && queue->start == queue->end) 
			return -1;
		//QUEUE_LOCK;
		memcpy(_data,(char*)queue->data + (queue->end * queue->single_size),queue->single_size);
		queue->end = ((++queue->end) % queue->count);
		queue->full_flag = 0;
		//queue->num --;
		//QUEUE_UNLOCK;
		return 0;
}

/****************************************************
	������:	queue_size
	����:		��ȡ���е�ǰ��Ա����
	����:	���о��
	����ֵ���������г�Ա����
	����:	liyao 2015��9��8��14:10:51
****************************************************/
uint16_t Queue_Size(QUEUE_T* queue){
	if(queue->full_flag == 1)
		return queue->count;
	else if(queue->start == queue->end)
		return 0;
	else if(queue->start > queue->end)
		return queue->start - queue->end;
	else if(queue->start < queue->end)
		return queue->count - queue->end + queue->start;
	return 0;
	//return queue->num;
}

/****************************************************
	������:	queue_empty_size
	����:	��ȡ����ʣ���λ����
	����:	���о��
	����ֵ������ʣ���λ����
	����:	liyao 2015��9��8��14:10:51
****************************************************/
uint16_t Queue_Empty_Size(QUEUE_T* queue){
	return queue->count - queue->num;
}

/****************************************************
	������:queue_free
	����:	�ͷŶ���
	����:	���о��
	����:	liyao 2015��9��8��14:10:51
****************************************************/
void Queue_Free(QUEUE_T* queue){
	queue->use_state = 0;
}
/* Usage:
	QUEUE_T* queue = queue_init(test ,sizeof(uint8_t)  ,10 );  
	queue_put(queue,(int8_t)200,0,0);
	queue_put(queue,(int8_t)201,0,0);
	queue_get(queue,(int8_t*)&i,NULL,NULL); */

















































