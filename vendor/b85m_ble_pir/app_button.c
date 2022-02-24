/*
File name		: app_button.c
Developer		: Jack Kilby
First Bread		: 2020-03-21
Description		: The button module code
*/
#include "app_button.h"
#include "drivers.h"
#define key_state_0  0
#define key_state_1  1
#define key_state_2  2
#define key_state_3  3
extern void led_init(void);
#define GPIO_LED_B			GPIO_PB5
/***************************************************************************
程序功能：一个按键的单击、双击、短按、长按。4种按键方式，然后做不同的处理。
***************************************************************************/
static unsigned char key_driver(void)
{
      static unsigned char key_state_buffer1 = key_state_0;
      static unsigned int key_timer_cnt1 = 0;
      unsigned char key_return = key_no;
      unsigned char key;
      unsigned char IO_Data;
      IO_Data = gpio_read(GPIO_PB6);//read the I/O states//GPIO_PB6  GPIO_PD4不能读取
      if(IO_Data)//IO设置为上拉输入，按键未按下时IO_Data为>0的数值，而不是为1
      {
            IO_Data = 1;
      }
      else
      {
            IO_Data = 0;
      }
      key = IO_Data;
      switch(key_state_buffer1)
      {
            case key_state_0:
                  if(key == 0)
                        key_state_buffer1 = key_state_1;
                        //按键被按下，状态转换到按键消抖和确认状态//
                  break;
            case key_state_1:
                  if(key == 0)
                  {
                        key_timer_cnt1 = 0;
                        key_state_buffer1 = key_state_2;
                        //按键仍然处于按下状态
                        //消抖完成，key_timer开始准备计时
                        //状态切换到按下时间计时状态
                  }
                  else
                        key_state_buffer1 = key_state_0;
                        //按键已经抬起，回到按键初始状态
                  break;  //完成软件消抖
            case key_state_2:
                  if(key == 1)
                  {
                        key_return = key_click;  //按键抬起，产生一次click操作
                        key_state_buffer1 = key_state_0;  //转换到按键初始状态
                  }
                  key_timer_cnt1++;
                  if(key_timer_cnt1 >= 200 && key_timer_cnt1< 500)  //按键继续按下，计时超过2000ms且不超过5000ms
                  {
                        key_state_buffer1 = key_state_3;  //转换到等待按键释放状态
                  }
                  if(key_timer_cnt1>=500)
                  {
                        key_state_buffer1 = key_state_3;  //转换到等待按键释放状态
                  }
                  break;
            case key_state_3:  //等待按键释放
                  if(key == 1)  //按键释放
                        {
                        printf("[BUTTON]:key is release\r\n");
                        printf("[BUTTON]the timecnt is:%d\r\n",key_timer_cnt1);    //打印按键按下的时长
                        if(key_timer_cnt1 >= 200 && key_timer_cnt1< 500)   //按键继续按下，计时超过2000ms且不超过5000ms
                        {
                        	//此处长按时间计数有问题，尚未解决
                              key_return = key_long_2s;  //送回长按事件
                        }
                        if(key_timer_cnt1>=500)
                        {
                              key_return = key_long_5s;
                        }
                        key_state_buffer1 = key_state_0;  //切回按键初始状态
                        }
                  else
                  {
                        key_timer_cnt1++;
                  }
                  break;
      }
      return key_return;
}
/***************************************************************************
函数功能：中层按键处理函数，调用底层函数一次，处理双击事件的判断，
                                        返回上层正确的无键、单击、双击、短按、长按5种状态
本函数由上层循环调用，间隔10ms
***************************************************************************/
unsigned char key_read(void)
{
      static unsigned char key_state_buffer2 = key_state_0;
      static unsigned char key_timer_cnt2 = 0;
      unsigned char key_return = key_no;
      unsigned char key;
      key = key_driver();
      switch(key_state_buffer2)
      {
            case key_state_0:
                  if(key == key_click)
                  {
                        key_timer_cnt2 = 0;  //第一次单击，不返回，到下个状态判断是否会出现双击
                        key_state_buffer2 = key_state_1;
                  }
                  else
                        key_return = key;  //对于无键、长按，返回原事件
                  break;
            case key_state_1:
                  if(key == key_click)  //又一次单击，时间间隔小于500ms
                  {
                        key_return = key_double;  //返回双击事件，回到初始状态
                        key_state_buffer2 = key_state_0;
                  }
                  else if(++key_timer_cnt2 >= 20)
                  {
                        //这里500ms内肯定读到的都是无键事件，因为长按大于2000ms
                        //在2s前底层返回的都是无键
                        key_return = key_click;  //500ms内没有再次出现单击事件，返回单击事件
                        key_state_buffer2 = key_state_0;  //返回初始状态
                  }
                  break;
      }
      return key_return;
}




int my_key_event()
{
      unsigned char temp = 0;
      temp = key_read();
     // printf("the temp is :%d\r\n",temp);
      switch(temp)
      {
            //单击
            case key_click:
            {
                  printf("[BUTTON]:key_click!\r\n");
                 // led_init(); //单击测试，灯亮

                  break;
            }
            //双击
            case key_double:
            {
                  printf("[BUTTON]:key_double!\r\n");
                  break;
            }
            //短按2s
            case key_long_2s:
            {
                  printf("[BUTTON]：key_long_2s!\r\n");
                  break;
            }
            //长按5s
            case key_long_5s:
            {
                  printf("[BUTTON]：key_long_5s!\r\n");
                  //led_init(); //长按5s，灯亮
// //               gpio_config_output(GPIO_LED_B, GPIO_HIGH, PM_PIN_PULLUP_10K);
                  break;
            }
            default:
                  break;
      }
      return 0;
      //break;
}


void Button_GPIO_Init(void){
	gpio_set_func(GPIO_PB6,AS_GPIO);
	gpio_set_input_en(GPIO_PB6,1);
	gpio_setup_up_down_resistor(GPIO_PB6,PM_PIN_PULLUP_10K);
}


