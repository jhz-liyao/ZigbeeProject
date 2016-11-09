#include "ProtocolFrame.h" 
#include "ProtocolHandle.h"
#include "LOG.h"
#include "Protocol.h"
 
//###################################对外变量区###################################
//PROTOCOL_INFO_T protocol_send_infos[SEND_PROTOCOL_NUM] = {0};//发送协议栈
//PROTOCOL_INFO_T recv_protocol_infos[RECV_PROTOCOL_NUM] = {0};//接收协议栈 
//static uint8_t _Recv_Protocol_Arr[RECV_PROTOCOL_NUM] = {0};//已经接收到协议的索引  
//PROTOCOL_DESC_T Send_Desc_P[SEND_PROTOCOL_NUM];//发送协议栈
//PROTOCOL_DESC_T Recv_Desc_P[RECV_PROTOCOL_NUM];//接收协议栈 
//PROTOCOL_DESC_T Transpond_Desc_P[TRANSPOND_PROTOCOL_NUM];//接收协议栈 
PROTOCOL_DESC_T Desc_P_Arr[PROTOCOL_NUM];//协议栈
uint8_t Protocol_Size = 0;
//###################################对内变量区################################### 
 


#if UART1_PROTOCOL_RESOLVER
	#define UART1_RPQUEUE_SIZE		1 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART1_Resolver;
	PROTOCOL_INFO_T _UART1_Protocol_QueueBuf[UART1_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART1_Resolver = &_UART1_Resolver;
#endif
#if UART2_PROTOCOL_RESOLVER
	#define UART2_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART2_Resolver;
	PROTOCOL_INFO_T _UART2_Protocol_QueueBuf[UART2_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART2_Resolver = &_UART2_Resolver;
#endif
#if UART3_PROTOCOL_RESOLVER
	#define UART3_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART3_Resolver;
	PROTOCOL_INFO_T _UART3_Protocol_QueueBuf[UART3_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART3_Resolver = &_UART3_Resolver;
#endif
#if UART4_PROTOCOL_RESOLVER
	#define UART4_RPQUEUE_SIZE		30 //接收协议缓冲区（存储多条协议） 
	Protocol_Resolver_T _UART4_Resolver;
	PROTOCOL_INFO_T _UART4_Protocol_QueueBuf[UART4_RPQUEUE_SIZE] = {0}; 
	Protocol_Resolver_T *UART1_Resolver = &_UART4_Resolver;
#endif
//-----------------------------------------------------  
//###################################对内函数区###################################
/****************************************************
	函数名:	clean_recv_buf
	功能:		清除协议栈正在写入的协议
	作者:		liyao 2015年9月8日14:10:51
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
	函数名:	_Fetch_Protocol
	功能:		提取并执行已经缓存的协议
	作者:		liyao 2016年9月8日10:54:34
****************************************************/
void _Fetch_Protocol(Protocol_Resolver_T* pr){
	PROTOCOL_INFO_T pi;
	while(Queue_Get(pr->Protocol_Queue,&pi) == 0){
		if(pi.check != NULL){
			if(pi.check(&pi) < 0){
				Log.error("协议校验不通过\r\n");
				break;
			}
		}
		if(pi.handle != NULL){
			pi.handle(&pi);
		}else{
			Log.error("收到协议但是无处理函数\r\n");
		}
	}
}
 
/****************************************************
	函数名:	Protocol_Put
	功能:		接收协议数据并解析封装
	参数:		协议数据
	注意: 	通过protocol_flag标志位标示是否解析出新的协议
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
int8_t _Protocol_Put(Protocol_Resolver_T* pr,uint8_t* datas,uint8_t len){
	uint8_t i,j, data; 
	uint16_t src_board_action;
	
	for(i = 0; i < len; i++){
		data = datas[i];
                if(pr->pi.head != 0xFD && data != 0xFD)
                  continue;
		if(pr->pi.head == 0xFD && data == 0xFD){ //协议被切断抛弃
			_clean_recv_buf(pr);
			Log.error("协议中途出现0xFD\r\n");
			return -1;
		}
		if(data == 0xFE){//处理转义
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
			
		if(pr->Recv_State > 0 && pr->Recv_State < 7)//排除帧头帧尾计算校验和
			pr->CheckSum += data; 
	//协议解析状态机
		switch(pr->Recv_State){
			case 0:	//处理帧头
						pr->pi.head = data;
						pr->Recv_State++; 
						break;
			case 1:	//处理预留位
						pr->pi.standby1 = data;
						pr->Recv_State++; 
						break;
			case 2: //处理帧长(从ID到数据位最后一个)
						pr->Recv_State++; 
						pr->pi.plen = data;
						if(data < 4){
							_clean_recv_buf(pr);
							Log.error("处理帧长错误\r\n");
							return -2;
						}
						else
							pr->cnt = pr->pi.paralen = data - 3;//计算结果为参数个数
							pr->pi.alen = data + 5;//计算结果为协议总长度包括FD、F8
						break;
			case 3: //处理目标板
						pr->pi.module = data;
						pr->Recv_State++; 
						break;
			case 4: //处理编号
						pr->pi.serial = data;
						pr->Recv_State++; 
						break;
			case 5: //处理指令码(ACTION)
						pr->pi.action = data;
						pr->Recv_State++; 
						break;
			case 6: //处理参数 
						((uint8_t *)(&pr->pi.protocol))[pr->index++] = data;  
						if(--pr->cnt == 0)
							pr->Recv_State++;
						break;
			case 7: //处理校验和校验  
						pr->pi.checksum = data;
						/*校验和暂时关闭*/
						if(((uint8_t)pr->CheckSum & 0xff) != data){
                                                  printf("计算结果:%x\r\n",(uint8_t)pr->CheckSum);
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
							Log.error("协议校验和错误\r\n");
							return -3;
						}else{ 
							pr->Recv_State++; 
						} 
						break;
			case 8: //处理帧尾 帧类型和长度进行匹配 
						if(data != 0xF8){
							_clean_recv_buf(pr);
							Log.error("帧尾位置非0xF8错误\r\n");
							return -4;
						}
						pr->pi.tail = data;
						src_board_action = pr->pi.module << 8 | pr->pi.action; 
						for(j = 0; j < Protocol_Size; j++){
							if(	src_board_action ==  Desc_P_Arr[j].ModuleAction &&//目标板匹配,动作匹配 
									pr->pi.paralen == Desc_P_Arr[j].ProtocolSize)//帧长度匹配
							{
								pr->pi.handle = Desc_P_Arr[j].handle;
								pr->pi.check = Desc_P_Arr[j].check;
								pr->pi.Desc_T = &Desc_P_Arr[j];
								break;
							}
						}
						if(j == Protocol_Size){//校验不通过
							_clean_recv_buf(pr);
							Log.error("现有协议库无匹配当前协议\r\n");
							return -5;
						}else{
							//Queue_Put(pr->Protocol_Queue, &pr->pi);//将协议信息放入协议缓冲队列
                                                        pr->pi.handle(&pr->pi);
							_clean_recv_buf(pr); 
						}
						break;
		}
	}; 
	return 0;
}
/****************************************************
	函数名:	char_special
	参数:		原字符
	功能: 	字符转义
****************************************************/
uint16_t char_special(uint8_t num){
	switch(num){
		case 0xFD:return 0xFE<<8 | 0x7D;
		case 0xF8:return 0xFE<<8 | 0x78;
		case 0xFE:return 0xFE<<8 | 0x7E;
	}
	return num;
}


//###################################对外函数区###################################

/****************************************************
	函数名:	getCheckSum_ByProtocolInfo
	功能:		根据协议信息获得校验和
	参数:		PROTOCOL_INFO_T协议描述信息
	返回值:	校验和结果
	注意：	只返回低8位
	作者:		liyao 2015年9月8日14:10:51
****************************************************/
uint8_t getCheckSum_ByProtocolInfo(PROTOCOL_INFO_T* pi){
	uint8_t i, sum = 0; 
	for(i = 1; i < pi->plen + 3;i++)
		sum+=((uint8_t*)pi)[i]; 
	return (uint8_t)sum;
}

/****************************************************
	函数名:	Protocol_Register
	功能:		协议注册
	作者:		liyao 2016年9月18日16:12:16  
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
	Log.error("协议注册失败,数组满\r\n");
	return -1;
	
}

/****************************************************
	函数名:	Get_Protocol_Description
	功能:		获取协议描述信息
	作者:		liyao 2016年9月18日16:12:16  
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
	Log.error("Get_Protocol_Description未找到协议描述\r\n");
	return NULL;
	
}



/****************************************************
	函数名:	ProtocolFrame_Init
	功能:		初始化全部协议和相关校验、执行函数
	作者:		liyao 2015年9月8日14:10:51      
****************************************************/
void ProtocolFrame_Init(){
//	memset(Send_Desc_P,0,sizeof(PROTOCOL_DESC_T) * SEND_PROTOCOL_NUM);
//	memset(Recv_Desc_P,0,sizeof(PROTOCOL_DESC_T) * RECV_PROTOCOL_NUM);
//	memset(Transpond_Desc_P,0,sizeof(PROTOCOL_DESC_T) * TRANSPOND_PROTOCOL_NUM);
        memset(Desc_P_Arr,0,sizeof(PROTOCOL_DESC_T) * PROTOCOL_NUM);
	//协议列表初始化
	Protocol_Init();
	//发送数据队列初始化 
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
	函数名:	Protocol_To_Uart
	功能:		向缓冲区写入待发送至串口
	参数:		PROTOCOL_INFO_T协议描述信息
	作者:		liyao 2015年9月8日14:10:51
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
		if(tmp_val == 0xFD || tmp_val == 0xF8 || tmp_val == 0xFE){//转义
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

//###################################自定义扩展函数区###################################
/****************************************************
	函数名:	Protocol_Send
	功能:		发送协议
	参数:		目标板,参数结构体,参数结构体长度
	注：协议历史编号统一处理
	作者:		liyao 2016年9月18日11:50:55
****************************************************/
void Protocol_Send(MODULE_ACTION ModuleAction,PROTOCOL_T* Protocol_t,u8 Len){ 
	PROTOCOL_INFO_T pi = {0}; 
	PROTOCOL_DESC_T* Desc_T = Get_Protocol_Description(ModuleAction, SEND);
	pi.Desc_T = Desc_T;
	pi.head = 0xFD;
	pi.plen = Len + 3;//参数个数+3   帧长度
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
	函数名:	Protocol_Send_Transpond
	功能:		转发协议
	参数:		PROTOCOL_INFO_T结构体
	作者:		liyao 2016年9月18日11:51:35
****************************************************/
void Protocol_Send_Transpond(PROTOCOL_INFO_T* pi){  
	Protocol_To_Uart(pi);	
}

/*****************************************************************
函数名:FetchProtocols
备注: 处理上位机串口命令总函数
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
