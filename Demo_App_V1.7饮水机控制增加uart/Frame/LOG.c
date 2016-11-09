#include "LOG.h"
#include "stdio.h"

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

