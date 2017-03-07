/*********************************************************************
* INCLUDES
*/
#ifndef _PWM_H_
#define _PWM_H_
#include "SerialApp.h"
#ifdef ZIGBEE_COLORFUL_LIGHT

#define LITHIUM_NUMS    1       //1�ڵ��
#define SAMPLE_RATE     2       //1/2����
#define SOFTWARE_VERSION     "20170224"       //����汾
#define HARDWARE_VERSION     "20170224"       //Ӳ���汾

//���Ժ�,���������Ӧ����Ϣ
#define PWM_TEST

#define KEEP	  P0_0        //direction:out��kill the power
#define INT       P0_1        //direction:in��signal of killing power
#define G_COUNT_L  T1CC1L   //P1_1	��ɫ  ͨ��1
#define G_COUNT_H  T1CC1H

#define B_COUNT_L T1CC2L   //P1_0	��ɫ  ͨ��2
#define B_COUNT_H T1CC2H

#define R_COUNT_L T1CC3L  //P0_7	��ɫ  ͨ��3
#define R_COUNT_H T1CC3H


extern uint8 lightCtlModle;

extern uint8 lightChangeStep;
//����PWM
extern void start_pwm(void);
//ȡ�õ�ǰ��ɫ����
uint8 getCurRed();
//ȡ�õ�ǰ��ɫ����
uint8 getCurGreen();
//ȡ�õ�ǰ��ɫ����
uint8 getCurBlue();
//ȡ�õ�ǰ���ȼ���
uint8 getCurBrightness();
//���õ�ǰ��ɫ����
void pwmConfig_Green(uint8 level);
//���õ�ǰ��ɫ����
void pwmConfig_Blue(uint8 level);
//���õ�ǰ��ɫ����
void pwmConfig_Red(uint8 level);
//���õ�ǰ���ȼ���
void pwmConfig_Brightness(uint8 level);
//���õ�ǰRGB���ȼ���
extern void pwmConfig_RGB(uint8 *RGB);
//����ģʽ
void LightGradualChange(uint8 step);
//��ɫģʽ
void LightSevenColorChange(uint8 color_num);   //color_num belongs to 0~7
//ģʽѡ����
extern void LightOnModle(uint8 modle_num,uint8 step,uint8 color_num);
#endif
#endif