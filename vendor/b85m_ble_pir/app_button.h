/*
File name		: app_button.h
Developer		: Jack Kilby
First Bread		: 2020-03-20
Description		: Header file of app_button.c
*/

#ifndef __APP_BUTTON_H__
#define __APP_BUTTON_H__
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
extern unsigned char key_read(void);
//extern static unsigned char key_driver(void);
//void key_read(void);
//void key_driver(void);
int my_key_event();
void Button_GPIO_Init(void);

#endif /* MG_KEY_H_ */
