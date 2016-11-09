#include "List.h" 
#include <string.h>
#include "LOG.h" 

/*ÿ���ڵ�12�ֽڿ���*/
/****************************************************
	������:List_Init
	����:	��ȡһ������ͷ
	����:	liyao 2016��10��26��11:47:26
****************************************************/
List_Head_T* List_Init(uint16_t Size){
	List_Head_T* ListHead = MALLOC(sizeof(List_Head_T));
	MALLOC_CHECK(ListHead, "List_Init"); 
	memset(ListHead, 0, sizeof(List_Head_T));
	ListHead->Size = Size;
	return ListHead;
}

/****************************************************
	������:List_Add
	����:	������ͷ�����һ��Ԫ��
	����:	liyao 2016��10��26��
****************************************************/
int8_t List_Add(List_Head_T* List_Head, void* Data, uint16_t Len){
	List_Node_T* ListNode = NULL;
	if(List_Head->Size > 0 && List_Head->Count == List_Head->Size){
		Log.error("������\r\n");
		return -1;
	}
	//����ListNode_T
	ListNode = MALLOC(sizeof(List_Node_T)); 
	MALLOC_CHECK(ListNode, "List_Add"); 
	ListNode->Data = MALLOC(sizeof(uint8_t) * Len);
	MALLOC_CHECK(ListNode->Data, "List_Add"); 
	//��ֵListNode_T 
	ListNode->Next = NULL;
	ListNode->Len	 = Len;
	memcpy(ListNode->Data, Data, Len); 
	//��������
	if(List_Head->Head == NULL){//�״μ���
		List_Head->Head = List_Head->Tail = ListNode;
	}else{//�������ݼ��� 
		ListNode->Next = List_Head->Head; 
		List_Head->Head = ListNode;
	}
	List_Head->Count++;
	return 0;
}

/****************************************************
	������:List_Add_Tail
	����:	������β�����һ��Ԫ��
	����:	liyao 2016��10��26��
****************************************************/
int8_t List_Add_Tail(List_Head_T* List_Head, void* Data, uint16_t Len){
List_Node_T* ListNode = NULL;
	if(List_Head->Size > 0 && List_Head->Count == List_Head->Size){
		Log.error("������\r\n");
		return -1;
	}
	//����ListNode_T
	ListNode = MALLOC(sizeof(List_Node_T)); 
	MALLOC_CHECK(ListNode, "List_Add_Tail"); 
	ListNode->Data = MALLOC(sizeof(uint8_t) * Len);
	MALLOC_CHECK(ListNode->Data, "List_Add_Tail"); 
	//��ֵListNode_T 
	ListNode->Next = NULL;
	ListNode->Len	 = Len;
	memcpy(ListNode->Data, Data, Len); 
	//��������
	if(List_Head->Head == NULL){//�״μ���
		List_Head->Head = List_Head->Tail = ListNode;
	}else{//�������ݼ��� 
		List_Head->Tail->Next = ListNode;
		List_Head->Tail = ListNode;
	}
	List_Head->Count++;
	return 0;
}

/****************************************************
	������:List_Remove
	����: �Ƴ�����ڵ�
	����:	liyao 2016��10��26��
****************************************************/
int8_t List_Remove(List_Head_T* List_Head, List_Node_T* List_Node){
	List_Node_T* cur = List_Head->Head;
	List_Node_T* last = NULL;
	List_Node_T* next = NULL;
	while(cur != NULL){
		if(cur == List_Node){
			next = List_Node->Next;
			FREE(cur->Data);
			FREE(cur); 
			if(last != NULL)
				last->Next = next;
			return 0;
			
		}
		last = cur;
		cur = cur->Next;
	}
	Log.waring("List_Removeδ�ҵ��ڵ�\r\n");
	return -1;
}

/****************************************************
	������:List_Free
	����:	�ͷ�����
	����:	liyao 2016��10��26��
****************************************************/
int8_t List_Free(List_Head_T* List_Head){
	List_Node_T* cur = List_Head->Head;
	List_Node_T* next = NULL;
	while(cur != NULL){
		next = cur->Next;
		FREE(cur->Data);
		FREE(cur);
		cur = next;
	}
	if(List_Head != NULL)
		FREE(List_Head);
	return 0;
}


