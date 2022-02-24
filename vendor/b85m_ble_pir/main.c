/********************************************************************************************************
 * @file	main.c
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


/**
 * @brief   IRQ handler
 * @param   none.
 * @return  none.
 */
//1.21
void  MG_GPIO_Init(void)
{
    gpio_set_func(GPIO_PB5,AS_GPIO);
    gpio_set_output_en(GPIO_PB5,1);
    gpio_write(GPIO_PB5,0);
}


_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}

void bootloader_unlock_flash()  //2_23新增 解锁button
{
    #if 1 // ((MCU_CORE_TYPE == MCU_CORE_8258)||(MCU_CORE_TYPE == MCU_CORE_8278)) // no need for eagle
    gpio_set_func(GPIO_DP, AS_USB);   // input enable inside.
    usb_dp_pullup_en(1);

    gpio_set_func(GPIO_DM, AS_USB);   // input enable inside.
    gpio_setup_up_down_resistor(GPIO_DM, PM_PIN_PULLUP_1M);
    #endif

	#define FLD_USB_BDT_EN      (BIT(3)|BIT(4))
    reg_swire_ctrl1 |= FLD_SWIRE_USB_EN;
    reg_usb_mdev |= FLD_USB_BDT_EN;   // also enable
}
/**
 * @brief		This is main function
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_ int main (void)    //must run in ramcode
{

	bootloader_unlock_flash(); //2.23先解锁芯片

	DBG_CHN0_LOW;   //debug

	blc_pm_select_internal_32k_crystal();

	#if(MCU_CORE_TYPE == MCU_CORE_825x)
//	bootloader_unlock_flash();   //2.23
	cpu_wakeup_init();    //硬件初始化
	#elif(MCU_CORE_TYPE == MCU_CORE_827x)
		cpu_wakeup_init(LDO_MODE,EXTERNAL_XTAL_24M);
	#endif

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init( !deepRetWakeUp );  //analog resistance will keep available in deepSleep mode, so no need initialize again

	clock_init(SYS_CLK_TYPE);

	if(!deepRetWakeUp){//read flash size
		blc_readFlashSize_autoConfigCustomFlashSector();
	}

	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	if( deepRetWakeUp ){
		user_init_deepRetn ();
	}
	else{
		user_init_normal ();
		MG_GPIO_Init();//1.21
//		bootloader_unlock_flash();//2.23
	}

    irq_enable();
    printf("------------------------log print complete !--------------\r\n");
	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();  //主循环任务，ble收发
	}
}

