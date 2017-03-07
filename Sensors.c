#include <ioCC2530.h>
#include "Sensors.h"
#include "SerialApp.h"
#include "hal_types.h"
#ifdef ZIGBEE_SENSOR
#include "DHT11.h"
void sensors_init(void)
{
  dev_type=DEV_TYPE_SENS;
  P0SEL &= 0xEf;                  //����P0.4��Ϊ��ͨIO
  P0SEL &= 0xDf;                  //����P0.5��Ϊ��ͨIO
  P0DIR |= 0x20;                  //����P0.5Ϊ���
  
  P0SEL &= ~0x40;                 //����P0.6Ϊ��ͨIO��
  P0DIR &= ~0x40;                 //P0.6����Ϊ�����
  P0SEL &= 0x7f;                  //P0_7���ó�ͨ��io
  
  //p15
  P1SEL &= ~0x20;                 //����P1.5Ϊ��ͨIO��
  P1DIR &= ~0x20;                 //P1.5����Ϊ�����
  
  LAMP_PIN = 0;                   //�͵�ƽ�̵����Ͽ�;�ߵ�ƽ�̵�������
}

//���P0_5 �̵������ŵĵ�ƽ
//on  1:��  0:��
uint8 GetLamp( void )
{
  uint8 ret;
  
  if(LAMP_PIN == 0)
  {	
    ret = 0;
  }
  else
  {
    ret = 1;
  }
  
  return ret;
}

//on  true:��  false:��
void SetLamp(bool on)
{
  LAMP_PIN=(on)?1:0;
}

//���P0_6 MQ-2���崫����������
//�ߵ�ƽʱ����������
uint8 GetGas( void )
{
  uint8 ret;
  
  if(GAS_PIN == 0)
    ret = 0;
  else
    ret = 1;
  
  return ret;
}

//���P0_4 ������⴫����������
//����,1:����0������
uint8 GetHuman( void )
{
  uint8 ret;
  
  if(HUMAN_PIN == 0)
    ret = 0;
  else
    ret = 1;
  
  return ret;
}

//����dht11����
void SerialApp_SendData_DHT11()
{
  uint8 SendBuf[20]={0};
  
  DHT11();                //��ȡ��ʪ��
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_CHECK_TEMP_HUM; //fc  ��ʪ�ȵ���Ӧ
  SendBuf[5]=5;   //data len
  SendBuf[6]=1;   //�ɹ�
  SendBuf[7]=0;            //data
  SendBuf[8] = wendu;  //data
  SendBuf[9]=0;            //data
  SendBuf[10] = shidu;    //data
  SendBuf[11] = XorCheckSum(&SendBuf[2], 9);
  SendBuf[12] = '\r';
  SendBuf[13] = '\n';
  SerialApp_SendDataToCoordinator(SendBuf, 14,SERIALAPP_CLUSTERID);
}

//����mq2����
void SerialApp_SendData_MQ2()
{
  uint8 SendBuf[20]={0};
  
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_CHECK_Smoke; //fc  �������Ӧ
  SendBuf[5]=2;   //data len
  SendBuf[6]=1;   //�ɹ�
  SendBuf[7]=GetGas()>0?true:false;  //��ȡ���崫������״̬  
  SendBuf[8] = XorCheckSum(&SendBuf[2], 6);
  SendBuf[9] = '\r';
  SendBuf[10] = '\n';
  SerialApp_SendDataToCoordinator(SendBuf, 11,SERIALAPP_CLUSTERID);
}

//������������
void SerialApp_SendData_Human()
{
  uint8 SendBuf[20]={0};
  
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_CHECK_HUMAN; //fc  
  SendBuf[5]=2;   //data len
  SendBuf[6]=1;   //�ɹ�
  SendBuf[7]=GetHuman()>0?true:false;  //��ȡ���崫������״̬  
  SendBuf[8] = XorCheckSum(&SendBuf[2], 6);
  SendBuf[9] = '\r';
  SendBuf[10] = '\n';
  
  SerialApp_SendDataToCoordinator(SendBuf, 11,SERIALAPP_CLUSTERID);
}

//���͵�״̬����
void SerialApp_SendData_Lamp(uint8 fc)
{
  uint8 SendBuf[20]={0};
  
  SendBuf[0]='$';
  SendBuf[1]='@'; 
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = fc; //fc
  SendBuf[5]=2;   //data len
  SendBuf[6]=1;   //�ɹ�
  SendBuf[7]=GetLamp()>0?true:false;  //true:��
  SendBuf[8] = XorCheckSum(&SendBuf[2], 6);
  SendBuf[9] = '\r';
  SendBuf[10] = '\n'; 
  SerialApp_SendDataToCoordinator(SendBuf, 11,SERIALAPP_CLUSTERID);
}
#endif