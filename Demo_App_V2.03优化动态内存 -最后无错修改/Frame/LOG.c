#include "LOG.h" 
#include "app_types.h"
#include <stdio.h>
#include <stdlib.h>
//------------------------------LOG------------------------------
Log_T Log = {0};

void _error(const char* message){ 
	#ifdef LOG_OUT
		printf("Log->ERROR:%s", message); 
	#endif
}

void _warning(const char* message){ 
	#ifdef LOG_OUT
		printf("Log->WARNING:%s", message); 
	#endif
}

void _info(const char* message){ 
	#ifdef LOG_OUT
		printf("Log->INFO:%s", message); 
	#endif
}

void Log_Init(void){
	Log.error = _error;
	Log.waring = _warning;
	Log.info = _info;
}

/*------------------------------MEM_LOG------------------------------*/
uint32_t MEM_USE = 0;
Mem_Info_T MemList[POINT_COUNT] = {0};


void* os_alloc(uint16_t len){
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == NULL){
			MemList[is].point = osal_mem_alloc(len);
			MemList[is].len	 = len;
			MEM_USE += len;
			//printf("s:%d\r\n", len);
			return MemList[is].point;
		}
	}
	//Log.error("·ÖÅäÊ§°Ü\r\n");
	return NULL;
}

void* os_calloc(uint16_t len, uint16_t size){
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == NULL){
			MemList[is].point = osal_mem_alloc(len);
			MemList[is].len	 = len;
			MEM_USE += len;
			//printf("s:%d\r\n", len);
			return MemList[is].point;
		}
	}
	Log.error("·ÖÅäÊ§°Ü\r\n");
	return NULL;
}

void os_free(void* point){
	uint16_t is = 0; 
	for(is = 0; is < POINT_COUNT; is++){
		if(MemList[is].point == point){
			MEM_USE -= MemList[is].len;
			//printf("r:%d\r\n", MemList[i].len);
			MemList[is].point = NULL;
			MemList[is].len	 = 0; 

			osal_mem_free(point);			
			return;
		}
	}
	FREE(point);
	Log.error("ÊÍ·ÅÊ§°Ü\r\n");
}



