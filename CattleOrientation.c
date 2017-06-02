#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Hal_uart.h"
#include "Hal_types.h"
#include "hal_lcd.h"
#include "OnBoard.h"
#include "gps.h"
#include "DHT11.h"
#include "SerialApp.h"
#include "CattleOrientation.h"

#ifdef ZIGBEE_CATTER_ORIENTATION
uint8 report_buf[40]={0};
uint16 report_buf_len=0;

void cattleOrientationReport(void)
{
  uint8 SendBuf[40]={0};
  uint8 gps_data_len=0;
  T_GpsOutData gps_data;
  gpsSave(&gps_data);     //��ȡgps���� 
  DHT11();                //��ȡ��ʪ��
  
  gps_data_len=sizeof(gps_data);
  
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_CATTER_ORIENTATION; //fc  ţȺ��λ
  SendBuf[5]=24;   //data len
  osal_memcpy(&SendBuf[6], &gps_data,gps_data_len);
  SendBuf[6+gps_data_len]=wendu;
  SendBuf[7+gps_data_len]=shidu;
  SendBuf[8+gps_data_len] = XorCheckSum(&SendBuf[2], SendBuf[5]+4);
  SendBuf[9+gps_data_len] = '\r';
  SendBuf[10+gps_data_len] = '\n';
  SerialApp_SendDataToCoordinator(SendBuf, gps_data_len+11,SERIALAPP_PACKAGED);
}

void ioInitGpsPow()
{
  P1SEL &= 0xFE;                  //����P1.0��Ϊ��ͨIO
  P1DIR |= 0x01;                  //����1.0Ϊ���
  GPS_POWER = 0;                  //�ϵ�
}
#endif