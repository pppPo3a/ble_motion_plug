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
�����ܣ�һ�������ĵ�����˫�����̰���������4�ְ�����ʽ��Ȼ������ͬ�Ĵ���
***************************************************************************/
static unsigned char key_driver(void)
{
      static unsigned char key_state_buffer1 = key_state_0;
      static unsigned int key_timer_cnt1 = 0;
      unsigned char key_return = key_no;
      unsigned char key;
      unsigned char IO_Data;
      IO_Data = gpio_read(GPIO_PB6);//read the I/O states//GPIO_PB6  GPIO_PD4���ܶ�ȡ
      if(IO_Data)//IO����Ϊ�������룬����δ����ʱIO_DataΪ>0����ֵ��������Ϊ1
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
                        //���������£�״̬ת��������������ȷ��״̬//
                  break;
            case key_state_1:
                  if(key == 0)
                  {
                        key_timer_cnt1 = 0;
                        key_state_buffer1 = key_state_2;
                        //������Ȼ���ڰ���״̬
                        //������ɣ�key_timer��ʼ׼����ʱ
                        //״̬�л�������ʱ���ʱ״̬
                  }
                  else
                        key_state_buffer1 = key_state_0;
                        //�����Ѿ�̧�𣬻ص�������ʼ״̬
                  break;  //����������
            case key_state_2:
                  if(key == 1)
                  {
                        key_return = key_click;  //����̧�𣬲���һ��click����
                        key_state_buffer1 = key_state_0;  //ת����������ʼ״̬
                  }
                  key_timer_cnt1++;
                  if(key_timer_cnt1 >= 200 && key_timer_cnt1< 500)  //�����������£���ʱ����2000ms�Ҳ�����5000ms
                  {
                        key_state_buffer1 = key_state_3;  //ת�����ȴ������ͷ�״̬
                  }
                  if(key_timer_cnt1>=500)
                  {
                        key_state_buffer1 = key_state_3;  //ת�����ȴ������ͷ�״̬
                  }
                  break;
            case key_state_3:  //�ȴ������ͷ�
                  if(key == 1)  //�����ͷ�
                        {
                        printf("[BUTTON]:key is release\r\n");
                        printf("[BUTTON]the timecnt is:%d\r\n",key_timer_cnt1);    //��ӡ�������µ�ʱ��
                        if(key_timer_cnt1 >= 200 && key_timer_cnt1< 500)   //�����������£���ʱ����2000ms�Ҳ�����5000ms
                        {
                        	//�˴�����ʱ����������⣬��δ���
                              key_return = key_long_2s;  //�ͻس����¼�
                        }
                        if(key_timer_cnt1>=500)
                        {
                              key_return = key_long_5s;
                        }
                        key_state_buffer1 = key_state_0;  //�лذ�����ʼ״̬
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
�������ܣ��в㰴�������������õײ㺯��һ�Σ�����˫���¼����жϣ�
                                        �����ϲ���ȷ���޼���������˫�����̰�������5��״̬
���������ϲ�ѭ�����ã����10ms
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
                        key_timer_cnt2 = 0;  //��һ�ε����������أ����¸�״̬�ж��Ƿ�����˫��
                        key_state_buffer2 = key_state_1;
                  }
                  else
                        key_return = key;  //�����޼�������������ԭ�¼�
                  break;
            case key_state_1:
                  if(key == key_click)  //��һ�ε�����ʱ����С��500ms
                  {
                        key_return = key_double;  //����˫���¼����ص���ʼ״̬
                        key_state_buffer2 = key_state_0;
                  }
                  else if(++key_timer_cnt2 >= 20)
                  {
                        //����500ms�ڿ϶������Ķ����޼��¼�����Ϊ��������2000ms
                        //��2sǰ�ײ㷵�صĶ����޼�
                        key_return = key_click;  //500ms��û���ٴγ��ֵ����¼������ص����¼�
                        key_state_buffer2 = key_state_0;  //���س�ʼ״̬
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
            //����
            case key_click:
            {
                  printf("[BUTTON]:key_click!\r\n");
                 // led_init(); //�������ԣ�����

                  break;
            }
            //˫��
            case key_double:
            {
                  printf("[BUTTON]:key_double!\r\n");
                  break;
            }
            //�̰�2s
            case key_long_2s:
            {
                  printf("[BUTTON]��key_long_2s!\r\n");
                  break;
            }
            //����5s
            case key_long_5s:
            {
                  printf("[BUTTON]��key_long_5s!\r\n");
                  //led_init(); //����5s������
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


