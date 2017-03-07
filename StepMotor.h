#ifndef __STEP_MOTOR_H__
#define __STEP_MOTOR_H__
#include "SerialApp.h"
#ifdef ZIGBEE_STEP_MOTOR
#include "hal_types.h"

//----------------------����﮵���������ܵ�ѹ�������ѹ�ı�������
#define LITHIUM_NUMS    1       //1�ڵ��
#define SAMPLE_RATE     2       //1/2����
#define SOFTWARE_VERSION     "20170224"       //����汾
#define HARDWARE_VERSION     "20170224"       //Ӳ���汾
static void motor_down(float n);
static void motor_up(float n);
static void Delay_ms(unsigned int msec);

extern void step_motor_init(void);
extern void Contrl_Step_motor(int state, float count);
#endif
#endif