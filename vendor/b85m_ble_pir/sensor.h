

#ifndef __SENSOR_H__
#define __SENSOR_H__
/************************ Include Files ***********************************************/
/************************ MACRO Definitions *******************************************/
/************************ Type Definitions ********************************************/
/************************ Externed Varibles *******************************************/
/************************ Global Varibles *********************************************/
/************************ Static Varibles *********************************************/
/************************ Static Functions Prototypes *********************************/
/************************ Externed Functions Prototypes ******************************/
//#if (__TRTL_SENSOR__)
#include "tl_common.h"
#define key_no           0
#define key_click        1
#define key_double       2
#define key_long_2s      3
#define key_long_5s      4
//API

void sensor(void);

#endif /* MG_KEY_H_ */
