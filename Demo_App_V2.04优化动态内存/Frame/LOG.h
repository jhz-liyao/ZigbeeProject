#ifndef __LOG_H__
#define __LOG_H__ 
#include "app_types.h"
#include "OSAL_Memory.h"
/*------------------------------LOG------------------------------*/
typedef struct _Log_T Log_T;
struct _Log_T{
	void(*info)(const char*);
	void(*waring)(const char*);
	void(*error)(const char*);
};

extern Log_T Log;
extern void Log_Init(void);


/*------------------------------MEM_LOG------------------------------*/

#define POINT_COUNT 100
#define MALLOC_CHECK(var, fun) do{if(var == NULL) Log.error(#fun"ø’º‰∑÷≈‰ ß∞‹");}while(0)
#define FREE_CHECK(var,len) do{}while(0);
	
#define MALLOC(len) osal_mem_alloc(len)
#define CALLOC(len, size) osal_mem_alloc(len)
//#define REALLOC(len, size) os_realloc(len, size)
#define FREE(point) osal_mem_free(point)
typedef struct{
	char* point;
	uint16_t len;
}Mem_Info_T;
 
extern uint32_t MEM_USE;
extern void* os_alloc(uint16_t len);
extern void* os_calloc(uint16_t len, uint16_t size);
extern void os_free(void* point);



#endif 

