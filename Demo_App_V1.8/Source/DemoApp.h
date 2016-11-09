#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define APP_ENDPOINT           10

#define APP_PROFID             0x0F04
#define APP_DEVICEID           0x0001
#define APP_DEVICE_VERSION     0
#define APP_FLAGS              0

#define APP_MAX_CLUSTERS       1
#define APP_CLUSTERID          1



#if defined( IAR_ARMCM3_LM )
#define APP_RTOS_MSG_EVT       0x0002
#endif  

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Generic Application
 */
extern void App_Init( byte task_id );

extern uint8* App_GetTaskID(void);

extern endPointDesc_t App_GetepDesc(void);

extern const cId_t App_ClusterList[APP_MAX_CLUSTERS];//¥ÿ¡–±Ì

extern const SimpleDescriptionFormat_t App_SimpleDesc;

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 App_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
