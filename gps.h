#ifndef _GPS_H_
#define _GPS_H_
#include "SerialApp.h"

#ifdef ZIGBEE_GPS
#include "Hal_types.h"

//#define LITHIUM_NUMS    1       //1�ڵ��
//#define SAMPLE_RATE     2       //1/2����
//#define SOFTWARE_VERSION     "170228"       //����汾
//#define HARDWARE_VERSION     "170224"       //Ӳ���汾

#define GPS_MAX_SVS 32
#define NMEA_MAX_SV_INFO    4

//���Ժ�
#define GPS_DEBUG       0


//ÿ��������Ϣ
typedef struct {
    //���
    uint8     prn;
    //�Ƿ�λ true:���������ڶ�λ
    uint8 isValid;
    //�����
    int   snr;
    //����
    float   elevation;
    //��λ��
    float   azimuth;
}GpsSvInfo;


typedef struct _time_
{
	uint8 year;
	uint8 month;
	uint8 day;
	uint8 hour;
	uint8 min;
	uint8 sec;
}TIME;

typedef struct _gps_info_
{
	uint8 state;//״̬ 0û�ź� 1�Ѿ���λ 2���ڶ�λ
	uint8 planet;//��������
	uint8 usePlanet;//ʹ����������
	uint8 fix;  //0:δ��λ��1:2D��λ��2:3D��λ
	double longitude;//����
	double latitude;//γ��
	double altitude;//����
	uint8 speed;//�ٶ�
	double direction;//����� ������Ϊ0 ˳ʱ��360��
	double hdop;    //ˮƽ����
	TIME UTC_time; 
	uint8 SorN;  //S(�ϰ���) is 0 N(������) is 1
	uint8 EorW;  //E(����) is 1 W(����) is 0
	uint8 valid; //TRUE:��������Ч

	//������Ϣ��
	int sv_count;
	GpsSvInfo sv_list[GPS_MAX_SVS];
}GpsInfo;


typedef struct {
  uint8 fix;  //0:δ��λ��1:2D��λ��2:3D��λ    //1 byte
  TIME UTC_time;                                //6 byte
  uint8 location;                                //1 byte 0x11,��������γ
  uint32 longitude;                       //4 byte 
  uint32 latitude;                        //4 byte
  uint16 speed;                           //2 byte
  uint16 direction;                       //2 byte
  uint16 altitude;                        //2 byte
}T_GpsOutData,*P_GpsOutData;


void  SendGpsDataToCoor();
void GpsDataParse(char* buff, int mLen);
uint8 GetYear();
uint8 GetMonth();
uint8 GetDay();
uint8 GetHour();
uint8 GetMinute();
uint8 GetSecond();
uint8 GetSorN();
uint8 GetEorW();
double GetLongitude();
double GetLatitude();
uint8 GetSpeed();
double GetDirection();
double GetAltitude();
uint8 GetFix();
void GpsInit();
double GetLongitude1();
double GetLatitude1();

extern void gpsSave(P_GpsOutData p_gps_data);
extern void  SendGpsDataToCoor();
extern void gpsInit(void);
extern void gpsGet(void);
#endif
extern void gpsDisplay(uint8* gpsData);
#endif