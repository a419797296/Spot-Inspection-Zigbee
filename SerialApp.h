/**************************************************************************************************
  Filename:       SerialApp.h
  Revised:        $Date: 2009-02-25 17:31:49 -0800 (Wed, 25 Feb 2009) $
  Revision:       $Revision: 19273 $

  Description:    This file contains the Serial Transfer Application definitions.


  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED �AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com. 
**************************************************************************************************/

#ifndef SERIALAPP_H
#define SERIALAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"

/*********************************************************************
 * CONSTANTS
 */

// These constants are only for example and should be changed to the
// device's needs
#define SERIALAPP_ENDPOINT               11

#define SERIALAPP_PROFID                 0x0F05
#define SERIALAPP_DEVICEID               0x0001
#define SERIALAPP_DEVICE_VERSION         0
#define SERIALAPP_FLAGS                  0

#define SERIALAPP_MAX_CLUSTERS           5
#define SERIALAPP_CLUSTERID              1
#define SERIALAPP_CLUSTERID2             2
#define SERIALAPP_JSON_CLUSTERID         3
#define SERIALAPP_GPS_PKG                4
#define SERIALAPP_END_DEV_INFO           5 
  
#define SERIALAPP_SEND_EVT               0x0001
#define SERIALAPP_RESP_EVT               0x0002
#define SERIALAPP_SEND_PERIODIC_EVT      0x0004
#define SERIALAPP_SYSTEM_TIME            0x0008

  
#define SERIALAPP_SEND_PERIODIC_TIMEOUT  500
#define SYS_ON_TIME 20    //second
#define SHUT_DOWN_TIME (SYS_ON_TIME*1000/SERIALAPP_SEND_PERIODIC_TIMEOUT)
  
// OTA Flow Control Delays
#define SERIALAPP_ACK_DELAY              1
#define SERIALAPP_NAK_DELAY              16

// OTA Flow Control Status
#define OTA_SUCCESS                      ZSuccess
#define OTA_DUP_MSG                     (ZSuccess+1)
#define OTA_SER_BUSY                    (ZSuccess+2)

/*********************************************************************
 * MACROS
 */


//�ն���
#define MAX_DEVICE  		6
  

  
//***********************�ն˽ڵ�ģʽѡ��*********************//
//------------------------------------------------------------//
#ifndef ZDO_COORDINATOR
//#define ZIGBEE_RFID
//#define ZIGBEE_GPS
#define ZIGBEE_SENS_TEMP
//#define ZIGBEE_AI_DETECT
//#define ZIGBEE_DI_DETECT
//#define ZIGBEE_DO_CTRL
//#define ZIGBEE_IO_DECT_CTRL
//#define ZIGBEE_SENSOR
//#define ZIGBEE_STEP_MOTOR
//#define ZIGBEE_COLORFUL_LIGHT
#else
  
#define LITHIUM_NUMS    1       //1�ڵ��
#define SAMPLE_RATE     2       //1/2����
#define SOFTWARE_VERSION     "170228"       //����汾
#define HARDWARE_VERSION     "170224"       //Ӳ���汾
  
#endif
//------------------------------------------------------------//
  
  
  
//�����붨��
#define ZIGBEE_FUN_CODE_CHECK_LAMP				0x01	//��ѯ/��Ӧ�����ն��ϵƵ�״̬
#define ZIGBEE_FUN_CODE_CTRL_LAMP				0x02	//����/��Ӧ�����ն��ϵƵ�״̬
#define ZIGBEE_FUN_CODE_CHECK_Smoke				0x03	//��ѯ/��Ӧ�����ն�����������
#define ZIGBEE_FUN_CODE_CHECK_HUMAN				0x04	//��ѯ/��Ӧ�����ն��������Ӧģ��
#define ZIGBEE_FUN_CODE_CHECK_LIGHT				0x05	//��ѯ/��Ӧ�����ն��Ϲ���������
#define ZIGBEE_FUN_CODE_CHECK_WindowCurtains		0x06      //��ѯ/��Ӧ�����ն��ϵ綯����
#define ZIGBEE_FUN_CODE_CTRL_WindowCurtains		0x07	//����/��Ӧ�����ն��ϵ綯����
#define ZIGBEE_FUN_CODE_CHECK_RFID				0x08	//��ѯ/��ӦRFID�ϱ�
#define ZIGBEE_FUN_CODE_CHECK_TEMP_HUM			0x09	//��ѯ/��Ӧ�����ն����¶Ⱥ�ʪ��
#define ZIGBEE_FUN_CODE_CHECK_Flame				0x0a	//��ѯ�����ն��ϻ��洫����״̬
#define ZIGBEE_FUN_CODE_STEP				0x0b	//����/��Ӧ�ն��ϵĲ������
#define ZIGBEE_FUN_CODE_RFID_FINDCARD			0x0c   //Ѱ��/��Ӧ�ն��ϵ�RFID
#define ZIGBEE_FUN_CODE_RFID_Conflict			0x0d   //����ͻ/��Ӧ�ն��ϵ�RFID
#define ZIGBEE_FUN_CODE_RFID_FIND_READ_CARD			0x0E   //Ѱ������
#define ZIGBEE_FUN_CODE_CHECK_TEMP			0x0F	//��ѯ/��Ӧ�����ն����¶�
#define ZIGBEE_FUN_CODE_ZIGBEE_ADDR_REGIST			0x10	//zigbee ��ַ�������
#define ZIGBEE_FUN_CODE_AI_DETECT			0x11	//zigbee ģ�����������������
#define ZIGBEE_FUN_CODE_DI_DETECT			0x12	//zigbee ������������������� 
#define ZIGBEE_FUN_CODE_DO_CTRL			        0x13	//zigbee ������������������� 
#define ZIGBEE_FUN_CODE_DO_CTRL_RETURN			        0x14	//zigbee �����������ⷴ������
#define ZIGBEE_FUN_CODE_ERR			        0x15	//zigbee �����������ⷴ������
#define ZIGBEE_FUN_CODE_VIRB_DETECTED			        0x16	//��⵽�񶯴���������
#define ZIGBEE_FUN_CODE_SENS_DETECTED			        0x17	//��⵽ģ�⴫��������
#define ZIGBEE_FUN_CODE_CUR_OR_VOLT_CTRL			0x18	//������ѹģʽѡ��
#define ZIGBEE_FUN_CODE_CUR_OR_VOLT_CTRL_RETURN			0x19	//������ѹģʽѡ����
#define ZIGBEE_FUN_CODE_SHUT_DOWN			0x20	//�ػ�
#define ZIGBEE_FUN_CODE_CHECK_CONNECTION			0x21	//�������״̬
#define ZIGBEE_FUN_CODE_SET_RGB_DATA				0x22	//�ֻ�/PC-->zigbee ����RGB����
#define ZIGBEE_FUN_CODE_UPDATA_RGB_DATA		0x23	//zigbee-->�ֻ�/PC �ϴ����д���������
#define ZIGBEE_FUN_CODE_END_DEV_INFO		0x24	//�ն˽ڵ���Ϣ
#define ZIGBEE_FUN_CODE_VIRB_SENS_DETECT			        0x25	//��⵽�񶯻��¶ȴ���������
typedef enum
{
	JSON_TYPE_GETWAY_TO_ZIGBEE = 1,
	JSON_TYPE_ZIGBEE_TO_GETWAY, 
	JSON_TYPE_CONTROL_CMD,
	JSON_TYPE_DATA_REPOART,
        JSON_TYPE_PRODUCT_INFO,
	JSON_TYPE_ERROR
}JSON_TYPE; 

typedef enum
{
        DEV_TYPE_RFID = 1,
	DEV_TYPE_TEMP, 
	DEV_TYPE_8AI,
	DEV_TYPE_8DI,
	DEV_TYPE_8DO,
        DEV_TYPE_IO_DECT_CTRL,
        DEV_TYPE_SENS,
        DEV_TYPE_STEP_MOTOR,
        DEV_TYPE_GPS,
        DEV_TYPE_NULL
}DEV_TYPE; 

extern DEV_TYPE dev_type;
extern uint8 system_time;
extern uint8 period_time;
extern uint8 shut_down_time;
extern uint8 bt_press_down_time;
extern uint8 bt_press_up_time;
extern uint8 system_SOC;

//DEV_TYPE dev_type=DEV_TYPE_NULL;
typedef enum
{
        ERR_CAN_NOT_FIND_RFID_CARD = 1,
}ERR_TYPE; 

//--------------------------------------------------------//

//--------------------------------------------------------
#ifdef ZDO_COORDINATOR                    //���������Э����

#define KEEP	  P0_0        //direction:out��kill the power
#define INT       P0_1        //direction:in��signal of killing power
#define IS_VIRB   P0_4        //direction:in��detect whether the virb sensor is inserted
#define IS_SENS   P0_5        //direction:in��detect whether the 0-5V,4-20mA sensor is inserted
#define _24V_ON    P0_6        //direction:out��control the 24V power, 1: power on
#define CUR_ON    P0_7        //direction:out��control the detect model: 1:detect 4 ~ 20mA signal, otherwise detect 0 ~ 5V signal
#define LED       P1_0        //direction:out��led displayment
#define P0_DIR    0xc1
#define P1_DIR    0x01



#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])

#endif
//--------------------------------------------------------

#define MAX_BYTES_OF_VERS_INFO  15
typedef struct End_Point_Info
{
  uint16      short_addr;
  uint8       ieee_addr[8];
  char        soft_vers[MAX_BYTES_OF_VERS_INFO];   //����汾
  char        hard_vers[MAX_BYTES_OF_VERS_INFO];   //Ӳ���汾
  DEV_TYPE    dev_type;
  char *      dev_alis;
}EP_INFO;

extern EP_INFO ep_addr;



////ÿһ���ն��ϵ��豸���ݽṹ
//typedef struct _end_device_data_info_
//{
//	int addr;		//�ն˶˵�ַ
//	byte LampState;//�Ƶ�״̬  TRUE:����
//	bool Smoke;   //��������TRUE:������
//	bool HumanState; //�����Ӧ��״̬  true:����
//	bool LightSensor; //��ÿ������ true:��ǿ���Ӧ
//	int WindowCurtains;//�綯��״̬,��Ϊ10��0~10
//	byte rfid[4];			//rfid�ϱ�����
//	byte Temperature;//	�¶�
//	byte Humidity;//ʪ��
//	bool Flame;  //���洫����
//}EndDeviceDataInfo;
//



/*********************************************************************
 * GLOBAL VARIABLES
 */
extern byte SerialApp_TaskID;

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Serial Transfer Application
 */
extern void SerialApp_Init( byte task_id );

/*
 * Task Event Processor for the Serial Transfer Application
 */
extern UINT16 SerialApp_ProcessEvent( byte task_id, UINT16 events );

extern uint8 systemSoc(uint8 channel);
extern bool SerialApp_SendDataToCoordinator(uint8* data, uint8 len,uint16 cluster_id);
extern uint8 XorCheckSum(uint8 * pBuf, uint8 len);
extern void Delay1ms(unsigned int msec);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SERIALAPP_H */
