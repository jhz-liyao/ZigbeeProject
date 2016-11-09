#ifndef __LOG_H__
#define __LOG_H__
/*------------------------------LOG------------------------------*/
typedef struct _Log_T Log_T;
struct _Log_T{
	void(*waring)(const char*);
	void(*error)(const char*);
        void(*info)(const char*);
};

extern Log_T Log;
extern void Log_Init(void);

 
#endif 

