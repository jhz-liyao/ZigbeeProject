#ifndef DEMO_H
#define DEMO_H

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
#define DEMO_ENDPOINT           10

#define DEMO_PROFID             0x0F04
#define DEMO_DEVICEID           0x0001
#define DEMO_DEVICE_VERSION     0
#define DEMO_FLAGS              0

#define DEMO_MAX_CLUSTERS       1
#define DEMO_CLUSTERID          1



#if defined( IAR_ARMCM3_LM )
#define DEMO_RTOS_MSG_EVT       0x0002
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
extern void Demo_Init( byte task_id );

/*
 * Task Event Processor for the Generic Application
 */
extern UINT16 Demo_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DEMO_H */
