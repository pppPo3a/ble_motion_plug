#include "sensor.h"
#include "drivers.h"

#include "tl_common.h"
//#include "app_device.h"
//#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "app.h"
#include "app_att.h"

#include "app_config.h"  //1.17加


typedef unsigned char uchar;
typedef unsigned int  uint;   //UNIT无符号整型
#define LED1 GPIO_PB4      //定义P1.0口为LED1控制端
#define LED2 GPIO_PB5       //定义P1.1口为LED2控制端
#define GPIO_LED_B			GPIO_PB5
#define GPIO_LED_R			GPIO_PB4

//#define LED3 P1_4       //定义P1.4口为LED3控制端
#define GPIO_SENSOR            GPIO_PB7        //定义P0.4口为传感器的输入端
volatile u8 sensor_detecting = 0;				//Used to tell the sensor status.
volatile u8 sensor_alarm = 0;
/****************************************************************************
* 名    称: InitUart()
* 功    能: 串口初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitUart(void)
{
	   gpio_set_func (GPIO_SENSOR, AS_GPIO);
		gpio_set_output_en(GPIO_SENSOR, 0);
		gpio_set_input_en(GPIO_SENSOR, 1);
}

#define	   BLE_DEVICE_ADDRESS_TYPE 					BLE_DEVICE_ADDRESS_PUBLIC
static u8	tbl_advData1[] = {

		0x09,
		GAP_ADTYPE_MANUFACTURER_SPECIFIC, //GAP_ADTYPE_LE_BD_ADDR, //不能使用，编译报错，
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00, //给自定义数据

		0x03,
		0x09,
		0x4f,     //'o'   //查url编码字符对应表
		0x4e,     //'n'
};

static u8	tbl_advData2[] = {

		0x09,
		GAP_ADTYPE_MANUFACTURER_SPECIFIC, //GAP_ADTYPE_LE_BD_ADDR, //不能使用，编译报错，
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00, //给自定义数据

		0x04,
		0x09,
		0x4f,   //'o'
		0x46,   //'f'
		0x46,   //'f'
};



//tbl_advData[6]= sensor_alarm;

/**
 * @brief	Scan Response Packet data
 */
//u8 name_w [12]={0xe4,0xbd,0xa0,0xe5,0xa5,0xbd,0xe4,0xb8,0x96,0xe7,0x95,0x8c};中文响应包


static u8	tbl_scanRsp1 [] = {
		 0x04, 0x09, 'p', 'i', 'r',

	};


static u8	tbl_scanRsp2[] = {
		 0x04, 0x09, 'P', 'I', 'R',

	};


void sensor(void)
{
                           //设置LED灯和P0.4相应的IO口
     InitUart();                       //调置串口相关寄存器
     u8  mac_public[6];
     u8  mac_random_static[6];
     	//for 512K Flash, flash_sector_mac_address equals to 0x76000
     	//for 1M  Flash, flash_sector_mac_address equals to 0xFF000
     blc_initMacAddress(flash_sector_mac_address, mac_public, mac_random_static);

     // InitUart();                       //调置串口相关寄存器
    // while(1)                          //无限循环
 //{
     if (gpio_read(GPIO_SENSOR))    {
    	 sensor_alarm = 1;
    	 sensor_detecting = 1;
     }
     else{
    	 sensor_alarm = 0;
    	 sensor_detecting = 0;
     }
 	printf("Init Sensor as %s.\r\n", sensor_alarm?"Detected":"Cleared");
 //	   DelayMS(2000) ;
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

     if(sensor_alarm == 1)
                       {
        	   gpio_config_output(GPIO_LED_R, GPIO_HIGH, PM_PIN_PULLUP_10K); //灯亮
        	  //有人时LED亮
        		gpio_setup_up_down_resistor(GPIO_SENSOR, PM_PIN_PULLUP_1M); //红外电平拉高
        		//cpu_set_gpio_wakeup(GPIO_SENSOR, Level_Low, 1);
        		//blt_soft_timer_add(&sensor,5000000);
//        		printf("sensor is ON   \r\n"); //串口发送数据,提示用户
//        		DelayMS(2000);
              }
             else
      {
            	 gpio_config_output(GPIO_LED_R, GPIO_LOW, PM_PIN_PULLUP_10K);
            //无人时LED熄灭
            	 gpio_setup_up_down_resistor(GPIO_SENSOR, PM_PIN_PULLDOWN_100K);
            	// cpu_set_gpio_wakeup(GPIO_SENSOR, Level_High, 1);  //sensor置低电平
//           	 printf("sensor is OFF    \r\n "); //串口发送数据,提示用户
//            	  DelayMS(2000);
      }


}

