#ifndef __SENSORS_H__
#define __SENSORS_H__
#include "SerialApp.h"
#ifdef ZIGBEE_SENSOR
#include "hal_board.h"
#include "hal_types.h"

#define HUMAN_PIN     P0_4        //�����Ӧ1:����0������
#define LAMP_PIN      P0_5        //����P0.5��Ϊ�̵��������
#define GAS_PIN       P1_5        //����P0.6��Ϊ���������������  
#define DHT11_DATA    P0_7        //dht11
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])

//----------------------����﮵���������ܵ�ѹ�������ѹ�ı�������
#define LITHIUM_NUMS    1       //1�ڵ��
#define SAMPLE_RATE     2       //1/2����
#define SOFTWARE_VERSION     "20170224"       //����汾
#define HARDWARE_VERSION     "20170224"       //Ӳ���汾
extern void sensors_init(void);
extern void SetLamp(bool on);
extern uint8 GetGas( void );
extern uint8 GetHuman( void );
extern void SerialApp_SendData_DHT11();
extern void SerialApp_SendData_MQ2();
extern void SerialApp_SendData_Human();
extern void SerialApp_SendData_Lamp(uint8 fc);
#endif

#endif