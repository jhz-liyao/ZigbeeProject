#include "ProtocolFrame.h" 
#include "ProtocolHandle.h"
#include "LOG.h"
#include "Protocol.h"
 
//###################################���������###################################
//PROTOCOL_INFO_T protocol_send_infos[SEND_PROTOCOL_NUM] = {0};//����Э��ջ
//PROTOCOL_INFO_T recv_protocol_infos[RECV_PROTOCOL_NUM] = {0};//����Э��ջ 
//static uint8_t _Recv_Protocol_Arr[RECV_PROTOCOL_NUM] = {0};//�Ѿ����յ�Э�������  
//PROTOCOL_DESC_T Send_Desc_P[SEND_PROTOCOL_NUM];//����Э��ջ
//PROTOCOL_DESC_T Recv_Desc_P[RECV_PROTOCOL_NUM];//����Э��ջ 
//PROTOCOL_DESC_T Transpond_Desc_P[TRANSPOND_PROTOCOL_NUM];//����Э��ջ 
PROTOCOL_DESC_T Desc_P_Arr[PROTOCOL_NUM];//Э��ջ
uint8_t Protocol_Size = 0;
//###################################���ڱ�����################################### 
 


#if UART1_PROTOCOL_RESOLVER
	#define UART1_RPQUEUE_SIZE		1 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _UART1_Resolver;
	PROTOCOL_INFO_T _UART1_Protocol_QueueBuf[UART1_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART1_Resolver = &_UART1_Resolver;
#endif
#if UART2_PROTOCOL_RESOLVER
	#define UART2_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _UART2_Resolver;
	PROTOCOL_INFO_T _UART2_Protocol_QueueBuf[UART2_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART2_Resolver = &_UART2_Resolver;
#endif
#if UART3_PROTOCOL_RESOLVER
	#define UART3_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _UART3_Resolver;
	PROTOCOL_INFO_T _UART3_Protocol_QueueBuf[UART3_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART3_Resolver = &_UART3_Resolver;
#endif
#if UART4_PROTOCOL_RESOLVER
	#define UART4_RPQUEUE_SIZE		30 //����Э�黺�������洢����Э�飩 
	Protocol_Resolver_T _UART4_Resolver;
	PROTOCOL_INFO_T _UART4_Protocol_QueueBuf[UART4_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART1_Resolver = &_UART4_Resolver;
#endif
//-----------------------------------------------------  
//###################################���ں�����###################################
/****************************************************
	������:	clean_recv_buf
	����:		���Э��ջ����д���Э��
	����:		liyao 2015��9��8��14:10:51
****************************************************/
void _clean_recv_buf(Protocol_Resolver_T* pr){  
	memset(&pr->pi, 0, sizeof(PROTOCOL_INFO_T)); 
	pr->Recv_State = 0;
	pr->cnt = 0;
	pr->index = 0;
	pr->Is_FE = 0;
	pr->CheckSum = 0;
}

/****************************************************
	������:	_Fetch_Protocol
	����:		��ȡ��ִ���Ѿ������Э��
	����:		liyao 2016��9��8��10:54:34
****************************************************/
void _Fetch_Protocol(Protocol_Resolver_T* pr){
	PROTOCOL_INFO_T pi;
	while(Queue_Get(pr->Protocol_Queue,&pi) == 0){
		if(pi.check != NULL){
			if(pi.check(&pi) < 0){
				Log.error("Э��У�鲻ͨ��\r\n");
				break;
			}
		}
		if(pi.handle != NULL){
			pi.handle(&pi);
		}else{
			Log.error("�յ�Э�鵫���޴�����\r\n");
		}
	}
}
 
/****************************************************
	������:	Protocol_Put
	����:		����Э�����ݲ�������װ
	����:		Э������
	ע��: 	ͨ��protocol_flag��־λ��ʾ�Ƿ�������µ�Э��
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len){
	uint8_t i,j, data; 
	uint16_t src_board_action;
	
	for(i = 0; i < len; i++){
		data = datas[i];
                if(pr->pi.head != 0xFD && data != 0xFD)
                  continue;
		if(pr->pi.head == 0xFD && data == 0xFD){ //Э�鱻�ж�����
			_clean_recv_buf(pr);
			Log.error("Э����;����0xFD\r\n");
			return -1;
		}
		if(data == 0xFE){//����ת��
			pr->Is_FE = 1;
			continue;
		}else if(pr->Is_FE){
			switch(data){
				case 0x7D: data = 0xFD;break;
				case 0x78: data = 0xF8;break;
				case 0x7E: data = 0xFE;break;
			} 
			pr->Is_FE = 0;
		}
			
		if(pr->Recv_State > 0 && pr->Recv_State < 7)//�ų�֡ͷ֡β����У���
			pr->CheckSum += data; 
	//Э�����״̬��
		switch(pr->Recv_State){
			case 0:	//����֡ͷ
						pr->pi.head = data;
						pr->Recv_State++; 
						break;
			case 1:	//����Ԥ��λ
						pr->pi.standby1 = data;
						pr->Recv_State++; 
						break;
			case 2: //����֡��(��ID������λ���һ��)
						pr->Recv_State++; 
						pr->pi.plen = data;
						if(data < 4){
							_clean_recv_buf(pr);
							Log.error("����֡������\r\n");
							return -2;
						}
						else
							pr->cnt = pr->pi.paralen = data - 3;//������Ϊ��������
							pr->pi.alen = data + 5;//������ΪЭ���ܳ��Ȱ���FD��F8
						break;
			case 3: //����Ŀ���
						pr->pi.module = data;
						pr->Recv_State++; 
						break;
			case 4: //������
						pr->pi.serial = data;
						pr->Recv_State++; 
						break;
			case 5: //����ָ����(ACTION)
						pr->pi.action = data;
						pr->Recv_State++; 
						break;
			case 6: //������� 
						((uint8_t *)(&pr->pi.protocol))[pr->index++] = data;  
						if(--pr->cnt == 0)
							pr->Recv_State++;
						break;
			case 7: //����У���У��  
						pr->pi.checksum = data;
						/*У�����ʱ�ر�*/
						if(((uint8_t)pr->CheckSum & 0xff) != data){
                                                  printf("������:%x\r\n",(uint8_t)pr->CheckSum);
                                                        printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\r\n",
                                                               pr->pi.head,
                                                               pr->pi.standby1,
                                                               pr->pi.plen,
                                                               pr->pi.module,
                                                               pr->pi.serial,
                                                               pr->pi.action,
                                                               pr->pi.protocol.AddrReport_P.para1,
                                                               pr->pi.protocol.AddrReport_P.para2,
                                                               pr->pi.checksum,
                                                               pr->pi.tail);
							_clean_recv_buf(pr);
							Log.error("Э��У��ʹ���\r\n");
							return -3;
						}else{ 
							pr->Recv_State++; 
						} 
						break;
			case 8: //����֡β ֡���ͺͳ��Ƚ���ƥ�� 
						if(data != 0xF8){
							_clean_recv_buf(pr);
							Log.error("֡βλ�÷�0xF8����\r\n");
							return -4;
						}
						pr->pi.tail = data;
						src_board_action = pr->pi.module << 8 | pr->pi.action; 
						for(j = 0; j < Protocol_Size; j++){
							if(	src_board_action ==  Desc_P_Arr[j].ModuleAction &&//Ŀ���ƥ��,����ƥ�� 
									pr->pi.paralen == Desc_P_Arr[j].ProtocolSize)//֡����ƥ��
							{
								pr->pi.handle = Desc_P_Arr[j].handle;
								pr->pi.check = Desc_P_Arr[j].check;
								pr->pi.Desc_T = &Desc_P_Arr[j];
								break;
							}
						}
						if(j == Protocol_Size){//У�鲻ͨ��
							_clean_recv_buf(pr);
							Log.error("����Э�����ƥ�䵱ǰЭ��\r\n");
							return -5;
						}else{
							//Queue_Put(pr->Protocol_Queue, &pr->pi);//��Э����Ϣ����Э�黺�����
                                                        pr->pi.handle(&pr->pi);
							_clean_recv_buf(pr); 
						}
						break;
		}
	}; 
	return 0;
}
/****************************************************
	������:	char_special
	����:		ԭ�ַ�
	����: 	�ַ�ת��
****************************************************/
uint16_t char_special(uint8_t num){
	switch(num){
		case 0xFD:return 0xFE<<8 | 0x7D;
		case 0xF8:return 0xFE<<8 | 0x78;
		case 0xFE:return 0xFE<<8 | 0x7E;
	}
	return num;
}


//###################################���⺯����###################################

/****************************************************
	������:	getCheckSum_ByProtocolInfo
	����:		����Э����Ϣ���У���
	����:		PROTOCOL_INFO_TЭ��������Ϣ
	����ֵ:	У��ͽ��
	ע�⣺	ֻ���ص�8λ
	����:		liyao 2015��9��8��14:10:51
****************************************************/
uint8_t getCheckSum_ByProtocolInfo(PROTOCOL_INFO_T* pi){
	uint8_t i, sum = 0; 
	for(i = 1; i < pi->plen + 3;i++)
		sum+=((uint8_t*)pi)[i]; 
	return (uint8_t)sum;
}

/****************************************************
	������:	Protocol_Register
	����:		Э��ע��
	����:		liyao 2016��9��18��16:12:16  
****************************************************/
int8_t Protocol_Register(PROTOCOL_DESC_T* Desc_T,PROTOCOL_TYPE Protocol_Type){
	uint8_t i = 0 ;
	PROTOCOL_DESC_T* tmp_Desc_T;
//	switch((uint8_t)Protocol_Type){
//		case (uint8_t)SEND:
//			Max_Count = SEND_PROTOCOL_NUM;
//			tmp_Desc_T = Send_Desc_P;
//			break;
//		case (uint8_t)RECEIVE:
//			Max_Count = RECV_PROTOCOL_NUM;
//			tmp_Desc_T = Recv_Desc_P;
//			break;
//		case (uint8_t)TRANSPOND:
//			Max_Count = TRANSPOND_PROTOCOL_NUM;
//			tmp_Desc_T = Transpond_Desc_P;
//			break;  
//	}
         
        tmp_Desc_T = Desc_P_Arr;
	for(i = 0; i < PROTOCOL_NUM; i++){
		if(tmp_Desc_T[i].ModuleAction == 0){
			tmp_Desc_T[i] = *Desc_T;
                        Protocol_Size++;
			return 0;
		}
	}
	Log.error("Э��ע��ʧ��,������\r\n");
	return -1;
	
}

/****************************************************
	������:	Get_Protocol_Description
	����:		��ȡЭ��������Ϣ
	����:		liyao 2016��9��18��16:12:16  
****************************************************/
PROTOCOL_DESC_T* Get_Protocol_Description(MODULE_ACTION ModuleAction,PROTOCOL_TYPE Protocol_Type){
	uint8_t i = 0 ;
	PROTOCOL_DESC_T* tmp_Desc_T;
//	switch((uint8_t)Protocol_Type){
//		case (uint8_t)SEND:
//			Max_Count = SEND_PROTOCOL_NUM;
//			tmp_Desc_T = Send_Desc_P;
//			break;
//		case (uint8_t)RECEIVE:
//			Max_Count = RECV_PROTOCOL_NUM;
//			tmp_Desc_T = Recv_Desc_P;
//			break;
//		case (uint8_t)TRANSPOND:
//			Max_Count = TRANSPOND_PROTOCOL_NUM;
//			tmp_Desc_T = Transpond_Desc_P;
//			break;  
//	} 
        tmp_Desc_T = Desc_P_Arr;
	for(i = 0; i < Protocol_Size; i++){
		if(ModuleAction == tmp_Desc_T[i].ModuleAction)
			return &tmp_Desc_T[i];
	}
	Log.error("Get_Protocol_Descriptionδ�ҵ�Э������\r\n");
	return NULL;
	
}



/****************************************************
	������:	ProtocolFrame_Init
	����:		��ʼ��ȫ��Э������У�顢ִ�к���
	����:		liyao 2015��9��8��14:10:51      
****************************************************/
void ProtocolFrame_Init(){
//	memset(Send_Desc_P,0,sizeof(PROTOCOL_DESC_T) * SEND_PROTOCOL_NUM);
//	memset(Recv_Desc_P,0,sizeof(PROTOCOL_DESC_T) * RECV_PROTOCOL_NUM);
//	memset(Transpond_Desc_P,0,sizeof(PROTOCOL_DESC_T) * TRANSPOND_PROTOCOL_NUM);
        memset(Desc_P_Arr,0,sizeof(PROTOCOL_DESC_T) * PROTOCOL_NUM);
	//Э���б��ʼ��
	Protocol_Init();
	//�������ݶ��г�ʼ�� 
#if UART1_PROTOCOL_RESOLVER
	UART1_Resolver->Protocol_Queue = Queue_Init( _UART1_Protocol_QueueBuf,sizeof(PROTOCOL_INFO_T), UART1_RPQUEUE_SIZE);
	UART1_Resolver->RPQueue_Size = UART1_RPQUEUE_SIZE; 
	UART1_Resolver->Protocol_Put = _Protocol_Put;
	UART1_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif  

#if UART2_PROTOCOL_RESOLVER
	UART2_Resolver->Protocol_Queue = Queue_Init( _UART2_Protocol_QueueBuf,sizeof(PROTOCOL_INFO_T), UART2_RPQUEUE_SIZE);
	UART2_Resolver->RPQueue_Size = UART2_RPQUEUE_SIZE; 
	UART2_Resolver->Protocol_Put = _Protocol_Put;
	UART2_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif  
	
#if UART3_PROTOCOL_RESOLVER
	UART3_Resolver->Protocol_Queue = Queue_Init( _UART3_Protocol_QueueBuf,sizeof(PROTOCOL_INFO_T), UART3_RPQUEUE_SIZE);
	UART3_Resolver->RPQueue_Size = UART3_RPQUEUE_SIZE; 
	UART3_Resolver->Protocol_Put = _Protocol_Put;
	UART3_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif 
	
#if UART4_PROTOCOL_RESOLVER
	UART4_Resolver->Protocol_Queue = Queue_Init( _UART4_Protocol_QueueBuf,sizeof(PROTOCOL_INFO_T), UART4_RPQUEUE_SIZE);
	UART4_Resolver->RPQueue_Size = UART4_RPQUEUE_SIZE; 
	UART4_Resolver->Protocol_Put = _Protocol_Put;
	UART4_Resolver->Fetch_Protocol = _Fetch_Protocol;
#endif 
	
}

/****************************************************
	������:	Protocol_To_Uart
	����:		�򻺳���д�������������
	����:		PROTOCOL_INFO_TЭ��������Ϣ
	����:		liyao 2015��9��8��14:10:51
****************************************************/
int8_t Protocol_To_Uart(PROTOCOL_INFO_T* pi){ 
	uint8_t data[100] = {0};
	uint8_t i = 0,special_H,special_L, index = 0,tmp_val;
	uint16_t special_char; 
	data[index++] = pi->head;
	for(i = 1; i < pi->alen-1;i++){
            if(i == pi->alen - 2)
                tmp_val = pi->checksum;
            else
		tmp_val = ((uint8_t *)pi)[i];
		if(tmp_val == 0xFD || tmp_val == 0xF8 || tmp_val == 0xFE){//ת��
			special_char = char_special(tmp_val); 
			special_H =  special_char >> 8;
			special_L =  special_char & 0x00ff;
			data[index++] = special_H;
			data[index++] = special_L;
		}else{
			data[index++] = tmp_val;
		}
	}
	//data[index++] = pi->checksum;
	data[index++] = pi->tail;
        if(pi->Desc_T != NULL)
            pi->Desc_T->send(data, index);
	return 0;
}

//###################################�Զ�����չ������###################################
/****************************************************
	������:	Protocol_Send
	����:		����Э��
	����:		Ŀ���,�����ṹ��,�����ṹ�峤��
	ע��Э����ʷ���ͳһ����
	����:		liyao 2016��9��18��11:50:55
****************************************************/
void Protocol_Send(MODULE_ACTION ModuleAction,PROTOCOL_T* Protocol_t,u8 Len){ 
	PROTOCOL_INFO_T pi = {0}; 
	PROTOCOL_DESC_T* Desc_T = Get_Protocol_Description(ModuleAction, SEND);
	pi.Desc_T = Desc_T;
	pi.head = 0xFD;
	pi.plen = Len + 3;//��������+3   ֡����
	pi.module = ModuleAction >> 8; 
	pi.serial = Desc_T->Serial++;
	pi.action = ModuleAction & 0x00ff;
	pi.protocol = *Protocol_t; 
	pi.checksum = getCheckSum_ByProtocolInfo(&pi);
	pi.tail = 0xF8;
	pi.paralen = Len;
	pi.alen = pi.plen + 5; 
//         printf("%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\r\n",
//                                                               pi.head,
//                                                               pi.standby1,
//                                                               pi.plen,
//                                                               pi.module,
//                                                               pi.serial,
//                                                               pi.action,
//                                                               pi.protocol.AddrReport_P.para1,
//                                                               pi.protocol.AddrReport_P.para2,
//                                                               pi.checksum,
//                                                               pi.tail);
	Protocol_To_Uart(&pi);	
}

/****************************************************
	������:	Protocol_Send_Transpond
	����:		ת��Э��
	����:		PROTOCOL_INFO_T�ṹ��
	����:		liyao 2016��9��18��11:51:35
****************************************************/
void Protocol_Send_Transpond(PROTOCOL_INFO_T* pi){  
	Protocol_To_Uart(pi);	
}

/*****************************************************************
������:FetchProtocols
��ע: ������λ�����������ܺ���
******************************************************************/
void FetchProtocols(void)
{
	#if UART1_PROTOCOL_RESOLVER
		UART1_Resolver->Fetch_Protocol(UART1_Resolver);
	#endif
	#if UART2_PROTOCOL_RESOLVER
		UART2_Resolver->Fetch_Protocol(UART2_Resolver);
	#endif
	#if UART3_PROTOCOL_RESOLVER
		UART3_Resolver->Fetch_Protocol(UART3_Resolver);
	#endif
	#if UART4_PROTOCOL_RESOLVER
		UART4_Resolver->Fetch_Protocol(UART4_Resolver);
	#endif
}
