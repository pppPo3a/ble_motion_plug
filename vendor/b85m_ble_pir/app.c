/********************************************************************************************************
 * @file	app.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "app.h"
#include "app_att.h"
//#include "common/blt_soft_timer.h"


#include "application/keyboard/keyboard.h"
#include "application/usbstd/usbkeycode.h"

#include "app_config.h"  //1.17加
//#include ".../application/u_printf.h"   //1.17加
//#include "../print/u_printf.h"  //1.18加
//1.18新加
#include "app_button.h"

extern int my_key_event();  //1.21 新加放main_loop
extern u16 PIR_Get_Batt_V();
extern void sensor(void);  //1.24
extern void Button_GPIO_Init(void); //2.7
//extern int blt_soft_timer_add(blt_timer_callback_t func, u32 interval_us);
//static u8 sensor_alarm = 0;
//static void advdata_mac(u8 *data);  //1.27
static u8 sensor_alarm = 0;
// extern volatile u8 sensor_alarm = 0;  //2.7
//extern void advertising_init(void);




#define 	   ADV_IDLE_ENTER_DEEP_TIME				60  //60 s
#define 	   CONN_IDLE_ENTER_DEEP_TIME			60  //60 s

#define 	   MY_DIRECT_ADV_TMIE							2000000    //原2s


#define       MY_APP_ADV_CHANNEL					        BLT_ENABLE_ADV_ALL  //广播37、
#define 	   MY_ADV_INTERVAL_MIN						ADV_INTERVAL_30MS
#define 	   MY_ADV_INTERVAL_MAX						ADV_INTERVAL_35MS  //此处可修改广播周期
//原发射功率为RF_POWER_P3dBm
#define	   MY_RF_POWER_INDEX							RF_POWER_P5p65dBm    //可调整发射功率，发射功率越大，广播距离越远



#define	   BLE_DEVICE_ADDRESS_TYPE 					BLE_DEVICE_ADDRESS_PUBLIC

_attribute_data_retention_	own_addr_type_t 	app_own_address_type = OWN_ADDRESS_PUBLIC;


/**
 * @brief      LinkLayer RX & TX FIFO configuration
 */
#define	   RX_FIFO_SIZE										64
#define	   RX_FIFO_NUM								     	8

#define	   TX_FIFO_SIZE										40
#define	   TX_FIFO_NUM										16


_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};


_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_txfifo = {
												TX_FIFO_SIZE,
												TX_FIFO_NUM,
												0,
												0,
												blt_txfifo_b,};



/**
 * @brief	Adv Packet data
 */


_attribute_data_retention_	int device_in_connection_state;
_attribute_data_retention_	u32 advertise_begin_tick;
_attribute_data_retention_	u32	interval_update_tick;
_attribute_data_retention_	u8	sendTerminate_before_enterDeep = 0;
_attribute_data_retention_	u32	latest_user_event_tick;


//1.19使用原文件后新加
#define GPIO_LED_B			GPIO_PB5
#define GPIO_LED_R			GPIO_PB4
_attribute_data_retention_	u8 gpio_wakeup_state_led = 0;//格式需要修改为此
_attribute_data_retention_ u8 g_awake_signal_led_blue = 0;

//1.19新加GPIO输出函数
void gpio_config_output(GPIO_PinTypeDef pin, unsigned int value, GPIO_PullTypeDef up_down)
{
	gpio_set_func (pin, AS_GPIO);
	gpio_set_input_en(pin, 0);
	gpio_set_output_en(pin, 1);
	gpio_setup_up_down_resistor(pin, up_down);
	gpio_write(pin, value);
}

//1.19新加GPIO输入函数
void gpio_config_input(GPIO_PinTypeDef pin, GPIO_PullTypeDef up_down)
{
	gpio_set_func (pin, AS_GPIO);
	gpio_set_input_en(pin, 1);
	gpio_set_output_en(pin, 0);
	gpio_setup_up_down_resistor(pin, up_down);
}

void led_init(void)
{
if (gpio_wakeup_state_led) {
         gpio_config_output(GPIO_LED_B, GPIO_LOW, PM_PIN_PULLDOWN_100K);
   } else {
         gpio_config_output(GPIO_LED_R, GPIO_HIGH, PM_PIN_PULLUP_10K);
       //  u_printf("BatteryPercent = %d%%.\r\n", PM_PIN_PULLDOWN_100K);
         printf("------led_init success Build Time: %s %s \r\n", __DATE__, __TIME__);
  }
}

//1.20新加ADC采集电源电压


//1.21  测试led亮灭
/*
int test0()
{
     gpio_toggle(GPIO_PB5);
     return 0;
}

u32 tick_loop = 0;
*/

//1.24 sensor











#if (UI_KEYBOARD_ENABLE)

_attribute_data_retention_	int 		key_not_released;
_attribute_data_retention_	u8 		key_type;
_attribute_data_retention_	static 	u32 keyScanTick = 0;

extern u32	scan_pin_need;

#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2

/**
 * @brief		this function is used to process keyboard matrix status change.
 * @param[in]	none
 * @return      none
 */
void key_change_proc(void)
{
	latest_user_event_tick = clock_time();  //record latest key change time

	u8 key0 = kb_event.keycode[0];
	u8 key_buf[8] = {0,0,0,0,0,0,0,0};

	key_not_released = 1;
	if (kb_event.cnt == 2)   //two key press, do  not process
	{
	}
	else if(kb_event.cnt == 1)
	{
		if(key0 >= CR_VOL_UP )  //volume up/down
		{
			key_type = CONSUMER_KEY;
			u16 consumer_key;
			if(key0 == CR_VOL_UP){  	//volume up
				consumer_key = MKEY_VOL_UP;
			}
			else if(key0 == CR_VOL_DN){ //volume down
				consumer_key = MKEY_VOL_DN;
			}
			blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
		}
		else
		{
			key_type = KEYBOARD_KEY;
			key_buf[2] = key0;
			blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
		}
	}
	else   //kb_event.cnt == 0,  key release
	{
		key_not_released = 0;
		if(key_type == CONSUMER_KEY)
		{
			u16 consumer_key = 0;
			blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
		}
		else if(key_type == KEYBOARD_KEY)
		{
			key_buf[2] = 0;
			blc_gatt_pushHandleValueNotify (BLS_CONN_HANDLE, HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
		}
	}
}



/**
 * @brief      this function is used to detect if key pressed or released.
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
_attribute_ram_code_
void proc_keyboard (u8 e, u8 *p, int n)
{
	if(clock_time_exceed(keyScanTick, 8000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}

	kb_event.keycode[0] = 0;
	int det_key = kb_scan_key (0, 1);

	if (det_key){
		key_change_proc();
	}
}



#elif (UI_BUTTON_ENABLE)


_attribute_data_retention_ static int button_detect_en = 0;
_attribute_data_retention_ static u32 button_detect_tick = 0;

#endif



/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_ENTER"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * SYSTEM_TIMER_TICK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
	}
}












/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_ADV_DURATION_TIMEOUT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void 	app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{
	 //配置广播参数,每一T_advEvent进行一轮广播，三个信道37 38 39
	//ADV_TYPE_CONNECTABLE_UNDIRECTED是ADV_NONCONN_IND类型，对其他设备的scan、connect请求都不回应
	bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
						ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
						0,  NULL,
						MY_APP_ADV_CHANNEL,
						ADV_FP_NONE);

	bls_ll_setAdvEnable(1);  //must: set adv enable启动广播
}



/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_TERMINATE"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void 	task_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	device_in_connection_state = 0;


	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}

	#if (UI_LED_ENABLE)
		gpio_write(GPIO_LED_RED, !LED_ON_LEVAL);
	#endif


	#if (BLE_APP_PM_ENABLE)
		 //user has push terminate pkt to ble TX buffer before deepsleep
		if(sendTerminate_before_enterDeep == 1){
			sendTerminate_before_enterDeep = 2;
		}
	#endif

	advertise_begin_tick = clock_time();
}



/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_EXIT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
_attribute_ram_code_ void	user_set_rf_power (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}


/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_CONNECT"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void	task_connect (u8 e, u8 *p, int n)
{

#if (!UI_BUTTON_ENABLE) //if use button, do not use big latency
//	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 19, CONN_TIMEOUT_4S);  // 200mS
	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 99, CONN_TIMEOUT_4S);  // 1 S   //可修改连接间隔
//	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 149, CONN_TIMEOUT_8S);  // 1.5 S
//	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 199, CONN_TIMEOUT_8S);  // 2 S
//	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 249, CONN_TIMEOUT_8S);  // 2.5 S
//	bls_l2cap_requestConnParamUpdate (CONN_INTERVAL_10MS, CONN_INTERVAL_10MS, 299, CONN_TIMEOUT_8S);  // 3 S
#endif

	latest_user_event_tick = clock_time();

	device_in_connection_state = 1;  //

	interval_update_tick = clock_time() | 1; //none zero


	#if (UI_LED_ENABLE)
		gpio_write(GPIO_LED_RED, LED_ON_LEVAL);
	#endif
}


/**
 * @brief      power management code for application
 * @param	   none
 * @return     none
 */
_attribute_ram_code_ void blt_pm_proc(void)
{
#if(BLE_APP_PM_ENABLE)
	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif

	//do not care about keyScan/button_detect power here, if you care about this, please refer to "8258_ble_remote" demo
	#if (UI_KEYBOARD_ENABLE)
		if(scan_pin_need || key_not_released){
			bls_pm_setSuspendMask (SUSPEND_DISABLE);
		}
	#endif


	#if (!TEST_CONN_CURRENT_ENABLE)   //test connection power, should disable deepSleep
			if(sendTerminate_before_enterDeep == 2){  //Terminate OK
				analog_write(USED_DEEP_ANA_REG, analog_read(USED_DEEP_ANA_REG) | CONN_DEEP_FLG);
				cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepSleep
			}


			if(  !blc_ll_isControllerEventPending() ){  //no controller event pending
				//adv 60s, deepsleep
				if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && !sendTerminate_before_enterDeep && \
					clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000))
				{
					cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
				}
				//conn 60s no event(key/voice/led), enter deepsleep
				else if( device_in_connection_state && \
						clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) )
				{

					bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
					bls_ll_setAdvEnable(0);   //disable adv
					sendTerminate_before_enterDeep = 1;
				}
			}
	#endif  //end of !TEST_CONN_CURRENT_ENABLE
#endif  //end of BLE_APP_PM_ENABLE
}






/**
 * @brief		user initialization when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */



void user_init_normal(void)  //ble系统初始化
{
	//random number generator must be initiated here( in the beginning of user_init_nromal)
	//when deepSleep retention wakeUp, no need initialize again
	random_generator_init();  //this is must
    //led_init();//1.19新加,1.21测试关闭,下午复原,加进按钮事件，注释
    u16 PIR_Get_Batt_V();
    printf("[BATT] batt_vol_mv is: %d\r\n",PIR_Get_Batt_V());//1.20,此时可以读取
//    printf("--------user_init_normal run success Build Time: %s    %s                \r\n", __DATE__, __TIME__);
//    printf("log success");
//1.21定时器初始化
    Button_GPIO_Init();  //1.21
    blt_soft_timer_init();
//    blt_soft_timer_add(&test0,200000);

////////////////// BLE stack initialization ////////////////////////////////////
	u8  mac_public[6];
	u8  mac_random_static[6];

	//for 512K Flash, flash_sector_mac_address equals to 0x76000
	//for 1M  Flash, flash_sector_mac_address equals to 0xFF000
     blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);
	//不需要 random static address，上⾯的 mac_random_static 填NULL//如果改为null将收不到

     #if(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_PUBLIC)
		app_own_address_type = OWN_ADDRESS_PUBLIC;
	#elif(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_RANDOM_STATIC)
		app_own_address_type = OWN_ADDRESS_RANDOM;
		blc_ll_setRandomAddr(mac_random_static);
	#endif
		//u8 mac_read[8];
		//device.getAddress
            	printf("MacAddress is    ");
	            printf("%x ：",mac_public[5]);
				printf("%x ：",mac_public[4]);
				printf("%x ：",mac_public[3]);
				printf("%x ：",mac_public[2]);
				printf("%x ：",mac_public[1]);
				printf("%x\r\n",mac_public[0]);  //1.25


	////// Controller Initialization  //////////
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(mac_public);				//mandatory
	blc_ll_initAdvertising_module(mac_public); 	//adv module: 		 mandatory for BLE slave,
	//connection用于初始化master slave 共用module
	blc_ll_initConnection_module();				//connection module  mandatory for BLE slave/master
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional



	////// Host Initialization  //////////
	blc_gap_peripheral_init();    //gap initialization
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization gap间隙  master调用语句为blc_gap_central_init(void);

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//   is about to exceed the sector threshold, this sector must be erased, and all useful information
	//   should re_stored) , so it must be done after battery check
#if (BLE_REMOTE_SECURITY_ENABLE)
	blc_smp_peripheral_init();    //SMP参数配置，安全等级
#else
	blc_smp_setSecurityLevel(No_Security);    //安全级别1，不支持加密配对
#endif






///////////////////// USER application initialization ///////////////////

	//bls_ll_setAdvData((u8 *) mac_public, sizeof(mac_public) );//如与下行同时存在，数据冲突不广播，单独存在，收不到广播
	//bls_ll_setAdvData( (u8 *)advdata, sizeof(advdata) );
	//advdata_mac(&tbl_advData[3]);
/*
	tbl_advData[4]=mac_public[5];
	tbl_advData[5]=mac_public[4];
	tbl_advData[6]=mac_public[3];
	tbl_advData[7]=mac_public[2];
	tbl_advData[8]=mac_public[1];
	tbl_advData[9]=mac_public[0];
	*/
//	tbl_advData[12]=sensor_alarm;
//	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );//增加新的数据格式之后再注释会收不到广播

	//advdata_mac(&tbl_scanRsp[7]);
	//tbl_scanRsp[7]=mac_public[5];
	//tbl_scanRsp[8]=mac_public[4];
	//tbl_scanRsp[9]=mac_public[3];
	//tbl_scanRsp[10]=mac_public[2];
	//tbl_scanRsp[11]=mac_public[1];
	//tbl_scanRsp[12]=mac_public[0];


//	bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &advertising_init);
//	advertising_init();
//	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));
	//printf("advdata is %s    \r\n",tbl_advData[12]);  //增加之后不广播
/*
	#define GPIO_SENSOR            GPIO_PB7
	gpio_set_func (GPIO_SENSOR, AS_GPIO);
			gpio_set_output_en(GPIO_SENSOR, 0);
			gpio_set_input_en(GPIO_SENSOR, 1);
			 if (gpio_read(GPIO_SENSOR))    {
			    	 sensor_alarm = 1;
			     }
			     else{
			    	 sensor_alarm = 0;
			     }
			printf("sensor is %s.\r\n", sensor_alarm?"on":"off");
	if(sensor_alarm == 1){
	    		 tbl_advData1[4]=mac_public[5];
	    		 	tbl_advData1[5]=mac_public[4];
	    		 	tbl_advData1[6]=mac_public[3];
	    		 	tbl_advData1[7]=mac_public[2];
	    		 	tbl_advData1[8]=mac_public[1];
	    		 	tbl_advData1[9]=mac_public[0];
	    	bls_ll_setAdvData( (u8 *)tbl_advData1, sizeof(tbl_advData1) );//增加新的数据格式之后再注释会收不到广播
	    	bls_ll_setScanRspData( (u8 *)tbl_scanRsp1, sizeof(tbl_scanRsp1));
	    	 }
	    	 else{
	    		 tbl_advData2[4]=mac_public[5];
	    		 	tbl_advData2[5]=mac_public[4];
	    		 	tbl_advData2[6]=mac_public[3];
	    		 	tbl_advData2[7]=mac_public[2];
	    		 	tbl_advData2[8]=mac_public[1];
	    		 	tbl_advData2[9]=mac_public[0];
	    		 bls_ll_setAdvData( (u8 *)tbl_advData2, sizeof(tbl_advData2) );//增加新的数据格式之后再注释会收不到广播
	    		 	bls_ll_setScanRspData( (u8 *)tbl_scanRsp2, sizeof(tbl_scanRsp2));
	    	 }
*/
	////////////////// config adv packet /////////////////////
#if (BLE_REMOTE_SECURITY_ENABLE)
	u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
	smp_param_save_t  bondInfo;
	if(bond_number)   //at least 1 bonding device exist
	{
		bls_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

	}

	if(bond_number)   //set direct adv
	{
		//set direct adv
		//LOW_DUTY只要收到匹配连接请求就响应，不会whitelist被过滤  peer_addr指定设备MAC地址和类型
		//1.25原ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY更换广播事件类型为 ADV_TYPE_SCANNABLE_UNDIRECTED
		u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
				ADV_TYPE_SCANNABLE_UNDIRECTED, app_own_address_type,
										bondInfo.peer_addr_type,  bondInfo.peer_addr,
										MY_APP_ADV_CHANNEL,
										ADV_FP_NONE);
		if(status != BLE_SUCCESS) {  	while(1); }  //debug: adv setting err

		//it is recommended that direct adv only last for several seconds, then switch to indirect adv
		bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 1);
		bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &app_switch_to_indirect_adv);
		//bls_app_registerEventCallback为注册回调函数的api,BLT_EV_FLAG_ADV_DURATION_TIMEOUT为回调事件，app_switch_to_indirect_adv为回调函数执行时底层上传指针
	}
	else   //set indirect adv
#endif
	{
		u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
										 ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
										 0,  NULL,
										 MY_APP_ADV_CHANNEL,
										 ADV_FP_NONE);
		if(status != BLE_SUCCESS) {  	while(1); }  //debug: adv setting err
	}

	bls_ll_setAdvEnable(1);  //adv enable  为1使能广播，0则不广播，idle状态使能，进入广播状态  adv状态失能，进入Idle状态
	//此处为最后一次调用bls_ll_setAdvEnable,为1，从连接状态退出，则回到adv状态
	blc_ota_initOtaServer_module();

	//set rf power index, user must set it after every suspend wakeup, cause relative setting will be reset in suspend
	user_set_rf_power(0, 0, 0);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &user_set_rf_power);



	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);




	///////////////////// Power Management initialization///////////////////
#if(BLE_APP_PM_ENABLE)
	blc_ll_initPowerManagement_module();

	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
	    blc_pm_setDeepsleepRetentionType(DEEPSLEEP_MODE_RET_SRAM_LOW16K); //default use 16k deep retention
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		blc_pm_setDeepsleepRetentionThreshold(95, 95);

		#if(MCU_CORE_TYPE == MCU_CORE_825x)
			blc_pm_setDeepsleepRetentionEarlyWakeupTiming(TEST_CONN_CURRENT_ENABLE ? 240 : 260);
		#elif((MCU_CORE_TYPE == MCU_CORE_827x))
			blc_pm_setDeepsleepRetentionEarlyWakeupTiming(TEST_CONN_CURRENT_ENABLE ? 340 : 350);
		#else
		#endif
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif

	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


	#if (UI_KEYBOARD_ENABLE)
		/////////// keyboard gpio wakeup init ////////
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High,1);  //drive pin pad high wakeup deepsleep
		}

		bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
	#elif (UI_BUTTON_ENABLE)

		cpu_set_gpio_wakeup (SW1_GPIO, Level_Low,1);  //button pin pad low wakeUp suspend/deepSleep
		cpu_set_gpio_wakeup (SW2_GPIO, Level_Low,1);  //button pin pad low wakeUp suspend/deepSleep

		bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_button);
	#endif


	advertise_begin_tick = clock_time();
}



/**
 * @brief		user initialization when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ void user_init_deepRetn(void)
{
#if (PM_DEEPSLEEP_RETENTION_ENABLE)

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	DBG_CHN0_HIGH;    //debug

	irq_enable();

	#if (UI_KEYBOARD_ENABLE)
		/////////// keyboard gpio wakeup init ////////
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			cpu_set_gpio_wakeup (pin[i], Level_High,1);  //drive pin pad high wakeup deepsleep
		}
	#elif (UI_BUTTON_ENABLE)

		cpu_set_gpio_wakeup (SW1_GPIO, Level_Low,1);  //button pin pad low wakeUp suspend/deepSleep
		cpu_set_gpio_wakeup (SW2_GPIO, Level_Low,1);  //button pin pad low wakeUp suspend/deepSleep
	#endif

#endif
}


/**
 * @brief     BLE main loop
 * @param[in]  none.
 * @return     none.
 */

void main_loop (void)
{

	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();  //必须被周期性的调用，内部执行BLE数据解析和逻辑处理
//1.21
	 //tick_loop++;
	// blt_soft_timer_process(CALLBACK_ENTRY);   //自定义定时任务处理
	 my_key_event();  //1.21增加按键任务处理
     sensor();  //1.24,一直输出，没有on//1.25,sensor状态输出不停，缺定时器



 	//    blt_soft_timer_add(&sensor,5000000);  //5s定时,上电执行时间显示结果不对，500好像是5分钟
	////////////////////////////////////// UI entry /////////////////////////////////
	#if (UI_KEYBOARD_ENABLE)
			proc_keyboard (0,0, 0);
	#elif (UI_BUTTON_ENABLE)
			// process button 1 second later after power on, to avoid power unstable
			if(!button_detect_en && clock_time_exceed(0, 1000000)){
				button_detect_en = 1;
			}
			if(button_detect_en && clock_time_exceed(button_detect_tick, 5000))
			{
				button_detect_tick = clock_time();
				proc_button(0, 0, 0);  //button triggers pair & unpair  and OTA
			}
	#endif


	////////////////////////////////////// PM Process /////////////////////////////////
	#if (UI_KEYBOARD_ENABLE)
			blt_pm_proc();
	#elif (UI_BUTTON_ENABLE)
			if(button_not_released){
				bls_pm_setSuspendMask (SUSPEND_DISABLE);
			}
			else{
				bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
			}
	#endif
}