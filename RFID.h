#ifndef __RFID_H__
#define __RFID_H__
#include "SerialApp.h"
#ifdef ZIGBEE_RFID
#include "hal_types.h"

#define LITHIUM_NUMS    1       //1�ڵ��
#define SAMPLE_RATE     2       //1/2����
#define SOFTWARE_VERSION     "170228"       //����汾
#define HARDWARE_VERSION     "170224"       //Ӳ���汾


extern bool FindReadRfidErr;

extern void rfidInit(void);
extern void RfidFindCard(void);
extern void RfidConflict(void);
extern void RfidFindReadCard(void);
#endif
#endif