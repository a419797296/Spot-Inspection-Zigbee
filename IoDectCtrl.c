#include "IoDectCtrl.h"
#include <ioCC2530.h>
#include "SerialApp.h"
#include "hal_types.h"
#include "hal_adc.h"
#ifdef ZIGBEE_IO_DECT_CTRL

void SerialApp_Send_ADC_Message( void );
void SerialApp_Send_DI_Message( void );
void SerialApp_Do_Ctrl(uint8* data);

void io_dect_ctrl_init(DEV_TYPE model)
{
  switch(model)
  {
  case DEV_TYPE_8AI:
    dev_type=DEV_TYPE_8AI;
    P0SEL &= ~0xFF;                  //����P0��ͨIO
    P0DIR = 0x00; //P0����Ϊ����
    P0INP = 0x00;  //����P0�˿�Ϊ����������
    P2INP |= 0x20;  //����P0�˿�Ϊ����
    break;
  case DEV_TYPE_8DI:
    dev_type=DEV_TYPE_8DI;
    break;
  case DEV_TYPE_8DO:
    dev_type=DEV_TYPE_8DO;
    P0SEL &= ~0xFF;                  //����P0��ͨIO
    P0DIR = 0xFF; //P0����Ϊ���
    P0=0;
    break;
  default:
    break;
  }
}

void io_dect_ctrl_exec(DEV_TYPE model,uint8 *data)
{
  switch(model)
  {
  case DEV_TYPE_8AI:
    SerialApp_Send_ADC_Message();
    break;
  case DEV_TYPE_8DI:
    SerialApp_Send_DI_Message();
    break;
  case DEV_TYPE_8DO:
    SerialApp_Do_Ctrl(data);
    break;
  default:
    break;
  }
}

//-------------------------------------8AI------------------------
static void SerialApp_Send_ADC_Message( void )
{
  uint8 SendBuf[30]={0};  
//  float vol=0.0; //adc������ѹ  
//  byte len=19;
  uint16 adc[8]={0};
  uint8 i;
  for(i=0;i<8;i++)
  {
    adc[i]=HalAdcRead(i, HAL_ADC_RESOLUTION_14);
    //������ֵ8192(��Ϊ���λ�Ƿ���λ)
    {       
      /* //�ն������ѹ
      vol=(float)((float)(adc[i]*3.3))/8192.0;
      
      //ȡС�����3λ���Ŵ�1000��
      adc[i]=vol*1000;*/
      adc[i]=((long)adc[i]*3300)>>13;
      
      SendBuf[2*i+7]=adc[i]>>8;            //data
      SendBuf[2*i+8] =adc[i];  //data
    }
  }
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_AI_DETECT; //fc  ģ��������
  SendBuf[5]=17;   //data len
  SendBuf[6]=1;   //�ɹ�
  
  SendBuf[23] = XorCheckSum(&SendBuf[2], 21);
  SendBuf[24] = '\r';
  SendBuf[25] = '\n'; 
  SerialApp_SendDataToCoordinator(SendBuf, 26, SERIALAPP_CLUSTERID);
}
//-------------------------------8DI----------------------
static void SerialApp_Send_DI_Message( void )
{
  uint8 SendBuf[20]={0};  
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_DI_DETECT; //fc  ��ʪ�ȵ���Ӧ
  SendBuf[5]=2;   //data len
  SendBuf[6]=1;   //�ɹ�
  SendBuf[7]=P0;  //detect the 8DI
  SendBuf[8] = XorCheckSum(&SendBuf[2], 6);
  SendBuf[9] = '\r';
  SendBuf[10] = '\n';      
  SerialApp_SendDataToCoordinator(SendBuf, 11, SERIALAPP_CLUSTERID);
}
//---------------------------8DO----------------------
static void SerialApp_Do_Ctrl(uint8* data)
{
  P0=data[6];
  uint8 SendBuf[20]={0};  
  SendBuf[0]='$';
  SendBuf[1]='@';
  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
  SendBuf[4] = ZIGBEE_FUN_CODE_DO_CTRL_RETURN; //fc  ��ʪ�ȵ���Ӧ
  SendBuf[5]=1;   //data len
  SendBuf[6]=1;   //�ɹ�
  SendBuf[7] = XorCheckSum(&SendBuf[2], 5);
  SendBuf[8] = '\r';
  SendBuf[9] = '\n';      
  SerialApp_SendDataToCoordinator(SendBuf, 10, SERIALAPP_CLUSTERID);
}
#endif
//
////------------------------------------------------------------�����̵���״̬json��
//void JsonCreatIoDetect(uint8 *data, uint16 addr)
//{
//    bool stat=0;
//    uint8 Success,cmd;
//    uint8 rfid_json[100]={0};
//    Success=data[6];//��Ӧ�ĵ�һ���ֽڴ����Ƿ�ɹ�	
//    cmd=data[4];
//    if(Success==1)
//    {
//      //�ɹ�
//      stat=data[7];
//      sprintf(rfid_json,"{\"jsonType\":%d,\"cmd\":%d,\"short_addr\":\"%04X\",\"stat\":%d}",JSON_TYPE_ZIGBEE_TO_GETWAY,cmd,addr,stat);
//      HalUARTWrite (UART0, rfid_json, strlen(rfid_json)+1);      
//    }
//
//}
//
////------------------------------------------------------------����8AI json��
//void JsonCreatAiDetect(uint8 *data, uint16 addr)
//{
//    uint16 adc[8]={0};   // store the 8 AI value
//    uint8 Success,cmd;
//    uint8 rfid_json[200]={0};
//    Success=data[6];//��Ӧ�ĵ�һ���ֽڴ����Ƿ�ɹ�	
//    cmd=data[4];
//    if(Success==1)
//    {
//      int i;
//      for(i=0;i<8;i++)
//      {
//        adc[i]=(data[2*i+7]<<8) |data[2*i+8];
//      }
//      sprintf(rfid_json,"{\"jsonType\":%d,\"cmd\":%d,\"short_addr\":\"%04X\",\"ch0\":%d,\"ch1\":%d,\"ch2\":%d,\"ch3\":%d,\"ch4\":%d,\"ch5\":%d,\"ch6\":%d,\"ch7\":%d}",JSON_TYPE_ZIGBEE_TO_GETWAY,cmd,addr,adc[0],adc[1],adc[2],adc[3],adc[4],adc[5],adc[6],adc[7]);
//      HalUARTWrite (UART0, rfid_json, strlen(rfid_json)+1);      
//    }
//
//}
//
////------------------------------------------------------------����8DI json��
//void JsonCreatDiDetect(uint8 *data, uint16 addr)
//{
//    uint8 DI=0;   // store the 8 AI value
//    uint8 di[9]={0};
//    uint8 Success,cmd;
//    uint8 rfid_json[200]={0};
//    Success=data[6];//��Ӧ�ĵ�һ���ֽڴ����Ƿ�ɹ�	
//    cmd=data[4];
//    if(Success==1)
//    {
//      DI=data[7];
//      /*int i;
//      for(i=0;i<8;i++)
//      {
//        di[i]+=(DI&0x01)+'0';
//        DI>>=1; 
//      }
//      sprintf(rfid_json,"{\"jsonType\":%d,\"cmd\":%d,\"short_addr\":\"%04X\",\"DI\":\"%s\"}",JSON_TYPE_ZIGBEE_TO_GETWAY,cmd,addr,di);
//      */
//      sprintf(rfid_json,"{\"jsonType\":%d,\"cmd\":%d,\"short_addr\":\"%04X\",\"DI\":%d}",JSON_TYPE_ZIGBEE_TO_GETWAY,cmd,addr,DI);
//      HalUARTWrite (UART0, rfid_json, strlen(rfid_json)+1);      
//    }
//
//}
//
//

//
////-------------------------------------------------------start of DI dection block
//#ifdef ZIGBEE_DI_DETECT
//void SerialApp_Send_DI_Message( void )
//{
//  uint8 SendBuf[20]={0};  
//  SendBuf[0]='$';
//  SendBuf[1]='@';
//  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
//  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
//  SendBuf[4] = ZIGBEE_FUN_CODE_DI_DETECT; //fc  ��ʪ�ȵ���Ӧ
//  SendBuf[5]=2;   //data len
//  SendBuf[6]=1;   //�ɹ�
//  SendBuf[7]=P0;  //detect the 8DI
//  SendBuf[8] = XorCheckSum(&SendBuf[2], 6);
//  SendBuf[9] = '\r';
//  SendBuf[10] = '\n';      
//
//  if ( AF_DataRequest( &SerialApp_TxAddr, (endPointDesc_t *)&SerialApp_epDesc,
//                      SERIALAPP_CLUSTERID,
//                      11,
//                      SendBuf,
//                      &SerialApp_MsgID, 
//                      0, 
//                      AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
//  {
//    // Successfully requested to be sent.
//    //HalUARTWrite(UART0, &SendBuf[7], 1); //���߷��ͳɹ���ԭ�����ظ���λ��	
//  }
//  else
//  {
//    // Error occurred in request to send.
//  }
//}
//#endif              //endif of AD detect block
////----------------------------------------------------
//
//
////-------------------------------------------------------start of DO CONTROL block
//#ifdef ZIGBEE_DO_CTRL
//void SerialApp_Do_Ctrl(uint8* data, int len)
//{
//  P0=data[6];
//  uint8 SendBuf[20]={0};  
//  SendBuf[0]='$';
//  SendBuf[1]='@';
//  SendBuf[2] = HI_UINT16( ep_addr.short_addr );
//  SendBuf[3] = LO_UINT16( ep_addr.short_addr );
//  SendBuf[4] = ZIGBEE_FUN_CODE_DO_CTRL_RETURN; //fc  ��ʪ�ȵ���Ӧ
//  SendBuf[5]=1;   //data len
//  SendBuf[6]=1;   //�ɹ�
//  SendBuf[7] = XorCheckSum(&SendBuf[2], 5);
//  SendBuf[8] = '\r';
//  SendBuf[9] = '\n';      
//
//  if ( AF_DataRequest( &SerialApp_TxAddr, (endPointDesc_t *)&SerialApp_epDesc,
//                      SERIALAPP_CLUSTERID,
//                      10,
//                      SendBuf,
//                      &SerialApp_MsgID, 
//                      0, 
//                      AF_DEFAULT_RADIUS ) == afStatus_SUCCESS )
//  {
//    // Successfully requested to be sent.
//    //HalUARTWrite(UART0, &SendBuf[7], 1); //���߷��ͳɹ���ԭ�����ظ���λ��	
//  }
//  else
//  {
//    // Error occurred in request to send.
//  }
//}
//#endif              //endif of DO control block
