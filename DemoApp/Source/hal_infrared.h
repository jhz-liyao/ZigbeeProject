#ifndef __INFRARED_H__
#define __INFRARED_H__


#include "hal_board.h"
//#define INFRARED_ISR
#define INFRARED_POLL

#define HAL_INFRARED_PORT   P0
#define HAL_INFRARED_BIT    BV(4)
#define HAL_INFRARED_SEL    P0SEL
#define HAL_INFRARED_DIR    P0DIR


#define HAL_INFRARED_IEN      IEN1  /* CPU interrupt mask register */
#define HAL_INFRARED_IENBIT   BV(5) /* Mask bit for all of Port_0 */
#define HAL_INFRARED_ICTL     P0IEN /* Port Interrupt Control register */
#define HAL_INFRARED_ICTLBIT  BV(1) /* P0IEN - P0.1 enable/disable bit */
#define HAL_INFRARED_PXIFG    P0IFG /* Interrupt flag at source */


extern void hal_infrared_init(void);
extern void hal_infrared_isr(void);
extern void hal_infrared_poll(void);  
  
#endif