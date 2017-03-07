/*********************************************************************
* INCLUDES
*/
#include <stdio.h>
#include <string.h>

#include "AF.h"
#include "OnBoard.h"
#include "OSAL_Tasks.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "hal_drivers.h"
#include "hal_key.h"
#include "hal_led.h"
#include "hal_uart.h"
#include "nwk_globals.h"
#include "pwm.h"
#include "SerialApp.h"

#ifdef ZIGBEE_COLORFUL_LIGHT

//P1_1	��ɫ  ͨ��1
//P1_0	��ɫ  ͨ��2
//P0_7  ��ɫ  ͨ��3

//PWM����������
#define MAX_PWM_CYCLE         1500

//�������ֵ�����Ե����ƵĹ���
//���ֵԽ�󣬹���Խ��
//���ܴ��������ֵMAX_PWM_CYCLE
//18ŷ������ʱ��1000�ﵽ��1W,1200��Լ1.3
#define MAX_PWM_UP_CYCLE 1200

//PWM��󼶱����ﶨ��256������0~255����RGB��ɫ��һ�µġ�
#define MAX_PWM_LEVEL         255

static uint8 CurRed=150;
static uint8 CurGreen=120;
static uint8 CurBlue=200;
static uint8 CurBrightness=32;
static uint8 index=0;
uint8 lightCtlModle=1;
uint8 lightChangeStep=255;

extern uint16 HalUARTWrite(uint8 port, uint8 *buf, uint16 len);

/*ʹ��P1_0��Ϊ���������˿ڣ������PWM����*/
void init_port(void)
{
    P1DIR |= 0x03;    // p10,p11 ���output
    P1SEL |= 0x03;    // p10,p11 ���� peripheral

    P0DIR |= 0x81;    // p07 ���output  P00���  KEEP
    P0SEL |= 0x80;    // p07 ���� peripheral
    
    
    P2DIR |= 0xC0;//��ʱ��1ͨ��2~3����
    PERCFG|= 0x40; // set timer_1 I/Oλ��Ϊ2
    
    KEEP=1;
}

/*
    ����׼ֵ����T1CC0 �Ĵ���, �����Ƚ�ֵ����T1CC2�Ĵ���
    ��T1CC2�е�ֵ��T1CC0�е�ֵ���ʱ����T1CC2 ����or���
*/
void init_timer(void)
{
    //��ʼ������ֵ
    T1CNTL=0;
    T1CNTH=0;

    //�ܵļ���
    
    T1CC0L = LO_UINT16(MAX_PWM_CYCLE);
    T1CC0H = HI_UINT16(MAX_PWM_CYCLE);

    //P1_1	��ɫ  ͨ��1
    B_COUNT_L = 0x7f;  //  ͨ��1   PWM signal period ռ�ձ�
    B_COUNT_H = 0x02;
    T1CCTL1 = 0x34;    // �������ϱȽ�

    //P1_0	��ɫ  ͨ��2
    G_COUNT_L = 0x7f;  //  ͨ��2   PWM signal period ռ�ձ�
    G_COUNT_H = 0x02;
    T1CCTL2 = 0x34;    // �������ϱȽ�


    //P0_7  ��ɫ  ͨ��3
    R_COUNT_L = 0x7f;  //  ͨ��3   PWM signal period ռ�ձ�
    R_COUNT_H = 0x02;
    T1CCTL3 = 0x34;    // �������ϱȽ�

    //128��Ƶ,ģģʽ�����¼���
    T1CTL |= 0x0E; // divide with 128 and to do i up-down mode
}

void start_pwm(void) 
{
    init_port();
    init_timer();
    pwmConfig_Red(CurRed);
    pwmConfig_Green(CurGreen);
    pwmConfig_Blue(CurBlue);
}

uint8 getCurRed()
{
  return CurRed;
}

uint8 getCurGreen()
{
  return CurGreen;
}

uint8 getCurBlue()
{
  return CurBlue;
}

uint8 getCurBrightness()
{
  return CurBrightness;
}

//���ݽ������õ�����,
//���㵱ǰ����
uint16 CalcCurBrightness()
{
    return(uint16)((float) ((float)MAX_PWM_UP_CYCLE/(float)MAX_PWM_LEVEL))*CurBrightness;
}


//level 0~255�����Ա仯
//�����ǿ���P10������ɫ
void pwmConfig_Green(uint8 level)
{
    uint16 curLevel=MAX_PWM_CYCLE;//Ĭ�������
 
    CurGreen=level;

    //ȷ�����ֵ��0~MAX_PWM_LEVEL
    if(level>MAX_PWM_LEVEL)
    {
      level=level%MAX_PWM_LEVEL;
    }

    //�������ǰ��ռ�ձȶ�Ӧ��ֵ
    if(level>=0)
    {
      curLevel=((float)((float)CalcCurBrightness()/(float)MAX_PWM_LEVEL))*level;
    }

    if(curLevel>MAX_PWM_CYCLE)
    {
        curLevel=MAX_PWM_CYCLE;
    }
    if(curLevel==0)
    {
        curLevel=1;
    }

#ifdef PWM_TEST    
    {
        uint8 buff[20]={0};
        sprintf(buff, "green:%d.\r\n", curLevel);
        HalUARTWrite(0,buff, osal_strlen(buff));
    }
#endif
    
  //  T1CTL &= ~0x03;    
    G_COUNT_L = LO_UINT16(curLevel);  //  ͨ��2   PWM signal period ռ�ձ�
    G_COUNT_H = HI_UINT16(curLevel);
 //   T1CTL |= 0x03;    
}

//level 0~255�����Ա仯
//�����ǿ���P11�����Ժ�ɫ
void pwmConfig_Red(uint8 level)
{
    uint16 curLevel=MAX_PWM_CYCLE;//Ĭ�������
 
    CurRed=level;
    
    //ȷ�����ֵ��0~MAX_PWM_LEVEL
    if(level>MAX_PWM_LEVEL)
    {
      level=level%MAX_PWM_LEVEL;
    }

    //�������ǰ��ռ�ձȶ�Ӧ��ֵ
    if(level>=0)
    {
      curLevel=((float)((float)CalcCurBrightness()/(float)MAX_PWM_LEVEL))*level;
    }

    if(curLevel>MAX_PWM_CYCLE)
    {
        curLevel=MAX_PWM_CYCLE;
    }
    if(curLevel==0)
    {
        curLevel=1;
    }

#ifdef PWM_TEST
    {
        uint8 buff[20]={0};
        sprintf(buff, "red:%d.\r\n", curLevel);
        HalUARTWrite(0,buff, osal_strlen(buff));
    }
#endif    
    
  //  T1CTL &= ~0x03;    
    B_COUNT_L = LO_UINT16(curLevel);  //  ͨ��1   PWM signal period ռ�ձ�
    B_COUNT_H = HI_UINT16(curLevel);
//    T1CTL |= 0x03;    
}

//level 0~255�����Ա仯
//�����ǿ���P07������ɫ
void pwmConfig_Blue(uint8 level)
{
    uint16 curLevel=MAX_PWM_CYCLE;//Ĭ�������
 
    CurBlue=level;
    
    //ȷ�����ֵ��0~MAX_PWM_LEVEL
    if(level>MAX_PWM_LEVEL)
    {
      level=level%MAX_PWM_LEVEL;
    }

    //�������ǰ��ռ�ձȶ�Ӧ��ֵ
    if(level>=0)
    {
      curLevel=((float)((float)CalcCurBrightness()/(float)MAX_PWM_LEVEL))*level;
    }

    if(curLevel>MAX_PWM_CYCLE)
    {
        curLevel=MAX_PWM_CYCLE;
    }
    if(curLevel==0)
    {
        curLevel=1;
    }

#ifdef PWM_TEST    
    {
        uint8 buff[20]={0};
        sprintf(buff, "blue:%d.\r\n", curLevel);
        HalUARTWrite(0,buff, osal_strlen(buff));
    }
#endif
    
  //  T1CTL &= ~0x03;    
    R_COUNT_L = LO_UINT16(curLevel);  //  ͨ��3   PWM signal period ռ�ձ�
    R_COUNT_H = HI_UINT16(curLevel);
  //  T1CTL |= 0x03;    
}


//��������
void pwmConfig_Brightness(uint8 level)
{
    CurBrightness=level;
}

void pwmConfig_RGB(uint8 *RGB)      //����RGB���������飬4���ֽ�
{
  pwmConfig_Red(*RGB++);
  pwmConfig_Green(*RGB++);
  pwmConfig_Blue(*RGB++);
  pwmConfig_Brightness(*RGB);
}

void LightGradualChange(uint8 step)
{
//    uint8[4]=rgb;   //4bit:R,G,B,brightness 
    uint16 red=0,green=0,blue=0;
    red=getCurRed();
    green=getCurGreen();
    blue=getCurBlue();   
    if(index==0)
    {
        //0:��-->��
      red>step? (red-=step) : (red=1);
      blue>step ? (blue-=step) : (blue=1);
      green<255-step ? (green+=step) : (green=255); 
        if(red==1 && blue==1 && green==255)
        {
            //ת����һ��
            index=1;
        }
    }
    else if(index==1)
    {
        // 1:��-->��
      green>step ? (green-=step) : (green=1);
      red>step ? (red-=step) : (red=1);
      blue<255-step ? (blue+=step) : (blue=255); 
        if(red==1 && green==1 && blue==255)
        {
            //ת����һ��
            index=2;
        }
    }
    else if(index==2)
    {
        // 2:��-->��
      red<255-step ? (red+=step) : (red=255); 
      green<255-step ? (green+=step) : (green=255); 
      blue<255-step ? (blue+=step) : (blue=255); 
        if(red==255 && green==255 && blue==255)
        {
            //ת����һ��
            index=3;
        }
    }
    else
    {
        // 3:��-->��
      green>step ? (green-=step) : (green=1);
      blue>step ? (blue-=step) : (blue=1);
      red<255-step ? (red+=step) : (red=255); 
        if(red==255 && green==1 && blue==1)
        {
            //ת����һ��
            index=0;
        }		
    }
    pwmConfig_Red(red);
    pwmConfig_Green(green);
    pwmConfig_Blue(blue);
}

void LightSevenColorChange(uint8 color_num)   //color_num belongs to 0~7
{
  uint8 red=0,green=0,blue=0;

  if(color_num>7)
    color_num%=7;
  if(color_num<1)
    color_num=1;
  if(color_num&0x01)
    blue=255;
  if(color_num&0x02)
    green=255;
  if(color_num&0x04)  
    red=255;
  pwmConfig_Red(red);
  pwmConfig_Green(green);
  pwmConfig_Blue(blue);
}

void LightOnModle(uint8 modle_num,uint8 step,uint8 color_num)   //
{
  switch(modle_num)
  {
    case 1:
      LightGradualChange(step);
      break;
    case 2:
      LightSevenColorChange(color_num);
      break;
    default:
      break;
    
  }

}
#endif