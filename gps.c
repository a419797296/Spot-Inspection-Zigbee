#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Hal_uart.h"
#include "Hal_types.h"
#include "hal_lcd.h"
#include "OnBoard.h"
#include "gps.h"
#include "gprs.h"
#include "CattleOrientation.h"
#include "SerialApp.h"
#ifdef ZIGBEE_GPS

extern uint16 HalUARTWrite(uint8 port, uint8 *buf, uint16 len);

static char frameData[100]; //ÿһ֡Ҫ����������
static unsigned char frameDataLen; //����֡���ݵĳ���
GpsInfo m_Info;

void gpsInit(void)
{
  dev_type=DEV_TYPE_GPS;
  memset(ep_addr.hard_vers,0,MAX_BYTES_OF_VERS_INFO);
  memset(ep_addr.soft_vers,0,MAX_BYTES_OF_VERS_INFO);
  strcpy(ep_addr.hard_vers,HARDWARE_VERSION);
  strcpy(ep_addr.soft_vers,SOFTWARE_VERSION);
  HalUARTWrite(0, "AT+MGPSC=1\r", osal_strlen("AT+MGPSC=1\r"));
  
}
void gpsGet(void)
{
  HalUARTWrite(0, "AT+GETGPS=\"ALL\"\r", osal_strlen("AT+GETGPS=\"ALL\"\r")); 
  serial_type = SERIAL_TYPE_GPS_DATA;
}
void CalibrateTime()
{
	uint8 year=m_Info.UTC_time.year;
	uint8 month=m_Info.UTC_time.month;
	uint8 day=m_Info.UTC_time.day;
	uint8 hour=m_Info.UTC_time.hour;

	hour+=8;
	if(hour>23)
	{
		hour=hour%24;
		day++;

		switch(month)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			case 8:
			case 10:
			{
				if(day>31)
				{
					day=1;
					month++;
				}
			}
			break;
			
			case 2:
			{
				if(year==16||year==20||year==24||year==28||year==32||year==36||year==40)
				{
					if (day>29)
					{
						day=1;
						month++;
					}
				}
				else
				{
					if(day>28)
					{
						day=1;
						month++;
					}
				}
			}
			break;

			case 4:
			case 6:
			case 9:
			case 11:
			{
				if(day>30)
				{
					day=1;
					month++;
				}
			}
			break;
			
			case 12:
			{
				if(day>31)
				{
					day=1;
					month=1;
					year++;
				}
			}
			break;
				
		}
	}

	m_Info.UTC_time.year=year;
	m_Info.UTC_time.month=month;
	m_Info.UTC_time.day=day;
	m_Info.UTC_time.hour=hour;
}

void ParseframeData(char* frame, int bufflen)
{
	//��buff�����ݽ��н���,����������','��֮������ݷ���keyBuff��
	int keyBuffIndex=0;
	int idx=0;
	char keyBuff[30][12];
	int i=0;
//    char printfBuff[30]={0};

	if(bufflen<=0) return;

	osal_memset(keyBuff, 0, sizeof(keyBuff));
	for(i=0; i<bufflen; i++)
	{
		if(frame[i]==',')
		{
			//�±�++
			keyBuffIndex++;
			idx=0;
		}
		else if(frame[i]=='\r' || frame[i]=='\n')
		{
		}
		else
		{
			keyBuff[keyBuffIndex][idx]=frame[i];
			idx++;
		}
	}
	
	if(0==strcmp( &keyBuff[0][3], "GSV" )) //�����������
	{
		//$GPGSV��(1)��(2)��(3)��(4)��(5)��(6)��(7)����(4),(5)��(6)��(7)*hh(CR)(LF)
		//(1)�ܵ�GSV����������2;
		//(2)��ǰGSV����:1;
		//(3)������������:08;
		//(4)PRN�루α��������룩��Ҳ������Ϊ�����Ǳ��
		//(5)����(00��90��):33��;
		//(6)��λ��(000��359��):240��;
		//(7)�����(00��99dB):


		//δע��ʱ��Ϣ��"$GPGSV,1,1,00*79",��ʱ����û����������
		if(keyBuffIndex<4) return;

        int num = atoi(keyBuff[1]);//gsv���������
        int seq = atoi(keyBuff[2]);//��ǰgsv�ǵڼ���
        int cnt = atoi(keyBuff[3]);//������
        int sv_base = (seq - 1)*NMEA_MAX_SV_INFO;
        int sv_num = cnt - sv_base;
		int idx, base = 4, base_idx;

		if (sv_num > NMEA_MAX_SV_INFO)
		{
			sv_num = NMEA_MAX_SV_INFO;
		}
		
		m_Info.planet=cnt;

        for (idx = 0; idx < sv_num; idx++)
		{            
			base_idx = base*(idx+1);
			int id    = atoi(keyBuff[base_idx+0]);
			float ele = atof(keyBuff[base_idx+1]);
			float azi = atof(keyBuff[base_idx+2]);
			int snr = atoi(keyBuff[base_idx+3]);

			//���һ���Ƿ����ظ���
			for(i=0; i<m_Info.sv_count && i<GPS_MAX_SVS; i++)
			{
				if(m_Info.sv_list[i].prn==id)
				{
					m_Info.sv_list[i].prn=id;
					m_Info.sv_list[i].snr=snr;
					m_Info.sv_list[i].elevation=ele;
					m_Info.sv_list[i].azimuth=azi; 
					break;
				}  
			}

			//û���ظ�������
			if(i==m_Info.sv_count)
			{
				if(i<GPS_MAX_SVS)
				{
					m_Info.sv_list[i].prn=id;
					m_Info.sv_list[i].snr=snr;
					m_Info.sv_list[i].elevation=ele;
					m_Info.sv_list[i].azimuth=azi;
					m_Info.sv_count++;
				}
				else
				{
				}
			}
        }
	}
	else if(0==strcmp( &keyBuff[0][3], "GSA" )) //�Ƿ���ʽ  2D or 3D
	{
		//$GPGSA,<1>,<2>,<3>,<3>,,,,,<3>,<3>,<3>,<4>,<5>,<6>,<7><CR><LF> 
		//1)ģʽ 2��M = �ֶ��� A = �Զ��� 
		//2)ģʽ 1����λ��ʽ 1 = δ��λ�� 2 = ��ά��λ�� 3 = ��ά��λ�� 
		//3) PRN ���֣�01 �� 32 �����ʹ���е����Ǳ�ţ����ɽ���12��������Ϣ��
		//4) PDOP-λ�þ���ϡ�� 0.5 �� 99.9. 
		//5) HDOP-ˮƽ����ϡ�� 0.5 to 99.9. 
		//6) VDOP-��ֱ����ϡ�� 0.5 to 99.9. 
		//7) Checksum.(���λ).


		//0:δ��λ��1:2D��λ��2:3D��λ
		if(keyBuff[2][0]=='2')
		{
			m_Info.fix=1;
		}
		else if(keyBuff[2][0]=='3')
		{
			m_Info.fix=2;
		}
		else
		{
			m_Info.fix=0;
		}


		//��λΪδ��λ״̬
		for(i=0; i<GPS_MAX_SVS; i++)
		{
            m_Info.sv_list[i].isValid=FALSE;
		}

		//�Ѿ���λ��
		if(m_Info.fix==1 || m_Info.fix==2)
		{
			int idx, max = 12; 

			for(idx=0; idx<max; idx++)
			{
				//�Ѿ�û��
				if(keyBuff[idx+3][0]==0)
				{
					break;
				}

				//��λ�����Ǻ�
				int id = atoi(keyBuff[idx+3]);


				//���һ���Ƿ����ظ���
				for(i=0; i<m_Info.sv_count && i<GPS_MAX_SVS; i++)
				{
					if(m_Info.sv_list[i].prn==id)
					{
						m_Info.sv_list[i].prn=id;
						m_Info.sv_list[i].isValid=TRUE;
						break;
					}  
				}

				//û���ظ�������
				if(i==m_Info.sv_count)
				{
					if(i<GPS_MAX_SVS)
					{
						m_Info.sv_list[i].prn=id;
						m_Info.sv_list[i].isValid=TRUE;
						m_Info.sv_count++;
					}
					else
					{
					}
				}
			}
		}
	}
	else if(0==strcmp( &keyBuff[0][3], "RMC" )) //ȡʱ��
	{
		//$GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11><CR><LF>			
		//1) ��׼��λʱ�䣨UTC time����ʽ��ʱʱ�ַ�����.�����루hhmmss.sss���� 
		//2) ��λ״̬��A = ���ݿ��ã�V = ���ݲ����á� 
		//3) γ�ȣ���ʽ���ȶȷַ�.�ַַַ֣�ddmm.mmmm���� 
		//4) γ�����֣�������N�����ϰ���S���� 
		//5) ���ȣ���ʽ���ȶȷַ�.�ַַַ֡�
		//6) �������֣�����E�����������W������ 
		//7) ���λ���ٶȣ� 0.0 �� 1851.8 knots 
		//8) ���λ�Ʒ���000.0 �� 359.9�ȡ�ʵ��ֵ�� 
		//9) ���ڣ���ʽ�������������꣨ddmmyy���� 
		//10) �ż�������000.0 ��180.0�� 
		//11) ������ 
		//12) Checksum.(���λ)


          //cattleOrientationReport();  

		//������Ч�ͱ���
		if(keyBuff[2][0]=='A')
		{
			//����ʱ��
			char year[3]={0}, month[3]={0}, day[3]={0};
			char hour[3]={0}, min[3]={0}, sec[3]={0};

			//ʱ��
			memcpy( (void *)hour, (const void *)(&keyBuff[1][0]), 2 );
			memcpy( (void *)min, (const void *)(&keyBuff[1][2]), 2 );
			memcpy( (void *)sec, (const void *)(&keyBuff[1][4]), 2 );

			m_Info.UTC_time.hour=atoi(hour);
			m_Info.UTC_time.min=atoi(min);
			m_Info.UTC_time.sec=atoi(sec);

			//����
			memcpy( (void *)day, (const void *)(&keyBuff[9][0]), 2 );
			memcpy( (void *)month, (const void *)(&keyBuff[9][2]), 2 );
			memcpy( (void *)year, (const void *)(&keyBuff[9][4]), 2 );

			m_Info.UTC_time.year=atoi(year);
			m_Info.UTC_time.month=atoi(month);
			m_Info.UTC_time.day=atoi(day);

			//+8Ϊ��������ʱ��
			CalibrateTime();		
		}
                        //�ϴ����ݵ�Э����
          
            //SendGpsDataToCoor();
          if(is_time_to_report)
          {
            HalLcdWriteString("getting gps.....", HAL_LCD_LINE_4 ); 
            if(m_Info.fix==1 || m_Info.fix==2)
            {
              SendGpsDataToCoor();
              HalLcdWriteString("got the gps.....", HAL_LCD_LINE_4 ); 
              cattleOrientationReport();
              is_time_to_report = 0;
              GPS_POWER = 0;    
              m_Info.fix=0;   //��ʼ��Ϊδ��λ״̬
            }
  
          }
	}
	else if(0==strcmp( &keyBuff[0][3], "GGA" ))//���GPGGA��γ����
	{
		//$GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,M,<10>,M,<11>,<12>*xx<CR><LF>
		//<1> UTC ʱ�䣬��ʽΪhhmmss.sss��
		//<2> γ�ȣ���ʽΪddmm.mmmm(��һλ����Ҳ������)��
		//<3> γ�Ȱ���N ��S(��γ����γ)
		//<4> ���ȣ���ʽΪdddmm.mmmm(��һλ��Ҳ������)��
		//<5> ���Ȱ���E ��W(����������)
		//<6> ��λ����ָʾ��0=��λ��Ч��1=��λ��Ч��
		//<7>ʹ��������������00��12(��һ����Ҳ������)
		//<8>ˮƽ��ȷ��hdop��0.5��99.9
		//<9>�����뺣ƽ��ĸ߶ȣ�-9999.9��9999.9��Mָ��λ��
		//<10>���ˮ׼��߶ȣ�-9999.9��9999.9��Mָ��λ��
		//<11>���GPS��������(RTCMSC-104)���������RTCM���͵�������
		//<12>��ֲο���վ��ţ���0000��1023(��λ0Ҳ������)��

        if(GPS_DEBUG)
        {
   //         HalUARTWrite(0, frame, bufflen);
   //         HalUARTWrite(0, "\r\n", 2);

   //         sprintf(printfBuff, "%s\r\n",keyBuff[2]);
   //         HalUARTWrite(0, printfBuff, osal_strlen(printfBuff));

   //         sprintf(printfBuff, "%s\r\n",keyBuff[4]);
   //         HalUARTWrite(0, printfBuff, osal_strlen(printfBuff));
            
        }
		
		m_Info.latitude=atof(keyBuff[2]);//γ��

		//γ�Ȱ���
		if(keyBuff[3][0]=='S')
		{
			m_Info.SorN=0;
		}
		else if(keyBuff[3][0]=='N')
		{
			m_Info.SorN=1;
		}
		else
		{
			m_Info.valid=FALSE;
		}

		//����
		m_Info.longitude=atof(keyBuff[4]);//����

        if(GPS_DEBUG)
        {
//            sprintf(printfBuff, "%f\r\n",m_Info.latitude);
//            HalUARTWrite(0, printfBuff, osal_strlen(printfBuff));            

//            sprintf(printfBuff, "%f\r\n",m_Info.longitude);
//            HalUARTWrite(0, printfBuff, osal_strlen(printfBuff));            
        }


		//���Ȱ���
		if(keyBuff[5][0]=='E')
		{
			m_Info.EorW=1;
		}
		else if(keyBuff[5][0]=='W')
		{
			m_Info.EorW=0;
		}
		else
		{
			m_Info.valid=FALSE;
		}
		
		//GPS ״̬ 0:δ��λ, 1�ǲ�ֶ�λ,2��ֶ�λ,3���ڹ���
		if(keyBuff[6][0]=='1' || keyBuff[6][0]=='2')//��λ
		{
			m_Info.state=1;
		}
		else if(keyBuff[6][0]=='6')//���ڹ���
		{
			m_Info.state=2;
		}
		else//δ��λ
		{
			m_Info.state=0;
		}

		m_Info.usePlanet=(uint8)atof(keyBuff[7]);//ʹ�õ���������		
		m_Info.hdop=atof(keyBuff[8]);//ˮƽ����
		m_Info.altitude=atof(keyBuff[9]);//����
	}
	else if(0==strcmp( &keyBuff[0][3], "VTG" ))//ȡ�ٶȺͷ���
	{
		//$GPVTG,<1>,T,<2>,M,<3>,N,<4>,K,<5>*hh			
		//<1> ������Ϊ�ο���׼�ĵ��溽��(000~359�ȣ�ǰ���0Ҳ��������)
		//<2> �Դű�Ϊ�ο���׼�ĵ��溽��(000~359�ȣ�ǰ���0Ҳ��������)
		//<3> ��������(000.0~999.9�ڣ�ǰ���0Ҳ��������)
		//<4> ��������(0000.0~1851.8����/Сʱ��ǰ���0Ҳ��������)
		//<5> ģʽָʾ(��NMEA0183 3.00�汾�����A=������λ��D=��֣�E=���㣬N=������Ч

		//����
		m_Info.direction=atof(keyBuff[1]);

		//�ٶ�
		m_Info.speed=(uint16)atof(keyBuff[7]);
	}
}

void GpsDataParse(char* buff, int mLen)
{
	//�ҳ�gps��һ֡����,��'$'��ͷ����"\r\n"������
	for(int i=0; i<mLen; i++)
	{
		if(buff[i]=='$' )
		{
			if(frameDataLen>0)
			{
				ParseframeData(frameData, frameDataLen);
			}
			
			memset(frameData, 0, sizeof(frameData));
			frameDataLen=0;

			frameData[frameDataLen]=buff[i];
			frameDataLen++;
		}
		else if(buff[i]=='\r')
		{
            //����ҵ�һ֡���ݣ��Ϳ�ʼ����
			if(frameDataLen>0) ParseframeData(frameData, frameDataLen);

			memset(frameData, 0, sizeof(frameData));
			frameDataLen=0;
		}
		else if(buff[i]=='\n')
		{
            //����ҵ�һ֡���ݣ��Ϳ�ʼ����
			if(frameDataLen>0) ParseframeData(frameData, frameDataLen);
			
			memset(frameData, 0, sizeof(frameData));
			frameDataLen=0;
		}
		else
		{
			frameData[frameDataLen]=buff[i];
			frameDataLen++;	
		}
	}

}

uint8 GetYear()
{
    return m_Info.UTC_time.year;
}

uint8 GetMonth()
{
    return m_Info.UTC_time.month;
}

uint8 GetDay()
{
    return m_Info.UTC_time.day;
}

uint8 GetHour()
{
    
    return m_Info.UTC_time.hour;
}

uint8 GetMinute()
{
    return m_Info.UTC_time.min;
}

uint8 GetSecond()
{
    return m_Info.UTC_time.sec;
}

uint8 GetSorN()
{
    return m_Info.SorN;
}

uint8 GetEorW()
{
    return m_Info.EorW;
}

double GetLongitude()
{
    double longitude=m_Info.longitude/100;
    uint16 longitude1=(uint16)longitude;
    double longitude2=longitude-longitude1;
    return longitude1+(longitude2*100)/60;
}

double GetLongitude1()
{
    return m_Info.longitude;
}
double GetLatitude1()
{
    return m_Info.latitude;
}

double GetLatitude()
{
    double latitude=m_Info.latitude/100;
    uint16 latitude1=(uint16)latitude;
    double latitude2=latitude-latitude1;
    return latitude1+(latitude2*100)/60;
}

uint8 GetSpeed()
{
    return m_Info.speed;
}

double GetDirection()
{
    return m_Info.direction;
}

double GetAltitude()
{
    return m_Info.altitude;
}

uint8 GetFix()
{
    if(1==m_Info.fix || 2==m_Info.fix) return 1;

    return 0;
}

void GpsInit()
{
    m_Info.fix=0;
    m_Info.SorN=1; 
	m_Info.EorW=1; 
}

//----------------------------��gps���ݷ��͸�Э����------------------------
void  SendGpsDataToCoor()
{
	uint8 data[30]={0};
	uint32 longitude =(uint32)(GetLongitude()*10000000);
	uint32 latitude =(uint32)(GetLatitude()*10000000);
	uint16 speed =(uint16)(GetSpeed());
	uint16 direction=(uint16)(GetDirection());
	uint16 altitude =(uint16)GetAltitude();


	//�Ƿ�λ
	data[0]=GetFix();

    //ʱ��
	data[1] = GetYear();
	data[2] = GetMonth();
	data[3] = GetDay();
	data[4] = GetHour();
	data[5] = GetMinute();
	data[6] = GetSecond();

	//��γ�ȱ�־
	data[7]=0x00;
	data[7]|=GetSorN()?1:0;
	data[7]|=GetEorW()?2:0;

	//����
	data[8]=(uint8)(longitude&0xff);
	data[9]=(uint8)((longitude>>8)&0xff);
	data[10]=(uint8)((longitude>>16)&0xff);
	data[11]=(uint8)((longitude>>24)&0xff);

	//γ��
	data[12]=(uint8)(latitude&0xff);
	data[13]=(uint8)((latitude>>8)&0xff);
	data[14]=(uint8)((latitude>>16)&0xff);
	data[15]=(uint8)((latitude>>24)&0xff);

	//�ٶ�
	data[16]=(uint8)(speed&0xff);
	data[17]=(uint8)((speed>>8)&0xff);

	//����
	data[18]=(uint8)(direction&0xff);
	data[19]=(uint8)((direction>>8)&0xff);

	//����
	data[20]=(uint8)(altitude&0xff);
	data[21]=(uint8)((altitude>>8)&0xff);

    //LCD��ʾ
    gpsDisplay(data);
    //��GPS���ݷ���Э����
    //SerialApp_SendDataToCoordinator(data, 22,SERIALAPP_GPS_PKG);
    
}

void gpsSave(P_GpsOutData p_gps_data)
{
  uint8 location=0x00;
  p_gps_data->fix=GetFix();
  p_gps_data->UTC_time.year=GetYear();
  p_gps_data->UTC_time.month=GetMonth();
  p_gps_data->UTC_time.day=GetDay();
  p_gps_data->UTC_time.hour=GetHour();
  p_gps_data->UTC_time.min=GetMinute();
  p_gps_data->UTC_time.sec=GetSecond();
  
  location|=GetSorN()?1:0;
  location|=GetEorW()?2:0;
  
  p_gps_data->location=location;
  p_gps_data->longitude=(uint32)(GetLongitude()*10000000);
  p_gps_data->latitude=(uint32)(GetLatitude()*10000000);
  p_gps_data->speed=(uint16)(GetSpeed());
  p_gps_data->direction=(uint16)(GetDirection());
  p_gps_data->altitude=(uint16)GetAltitude();  
  
  p_gps_data->longitude = BigLittleSwap32(p_gps_data->longitude);
  p_gps_data->latitude = BigLittleSwap32(p_gps_data->latitude);
  p_gps_data->speed = BigLittleSwap16(p_gps_data->speed);
  p_gps_data->direction = BigLittleSwap16(p_gps_data->direction);
  p_gps_data->altitude = BigLittleSwap16(p_gps_data->altitude);
}
#endif

//-------------------------------��ʾgps��Ϣ
void gpsDisplay(uint8* gpsData)
{
    char buff[50]={0};

    if(gpsData==0) return;
    
    if(gpsData[0]>0)
    {
        //
        sprintf(buff, "  20%02d��%02d��%02d��",gpsData[1],gpsData[2],gpsData[3]);
        HalLcdWriteString( buff, HAL_LCD_LINE_1 );  

        sprintf(buff, "  %02dʱ%02d��%02d��",gpsData[4],gpsData[5],gpsData[6]);
        HalLcdWriteString( buff, HAL_LCD_LINE_2 );  


        unsigned long bb=BUILD_UINT32(gpsData[8],gpsData[9],gpsData[10],gpsData[11]);
        double longitude=((double)bb)/10000000.0;        
        bb=BUILD_UINT32(gpsData[12],gpsData[13],gpsData[14],gpsData[15]);
        double latitude=((double)bb)/10000000.0;

        if((gpsData[7]&0x01)==0x01)
        {
            //������

            if((gpsData[7]&0x02)==0x02)
            {
                //����
                sprintf(buff, "��γ:%f",latitude);
//                HalUARTWrite(0, (uint8 *)buff, osal_strlen(buff)); 
//                HalUARTWrite(0, "\n", 1); 
                HalLcdWriteString( buff, HAL_LCD_LINE_3 );  
                sprintf(buff, "����:%f",longitude);
//                HalUARTWrite(0, (uint8 *)buff, osal_strlen(buff)); 
//                HalUARTWrite(0, "\n", 1); 
                HalLcdWriteString( buff, HAL_LCD_LINE_4 );  
            }
            else
            {
                //����
                sprintf(buff, "��γ:%f",latitude);
                HalLcdWriteString( buff, HAL_LCD_LINE_3 );  
                sprintf(buff, "����:%f",longitude);                
                HalLcdWriteString( buff, HAL_LCD_LINE_4 );  
            }
        }
        else
        {
            //�ϰ���
            //if(GetEorW()>0)
            if((gpsData[7]&0x02)==0x02)
            {
                //����
                sprintf(buff, "��γ:%f",latitude);
                HalLcdWriteString( buff, HAL_LCD_LINE_3 );  
                sprintf(buff, "����:%f",longitude);
                HalLcdWriteString( buff, HAL_LCD_LINE_4 );  
            }
            else
            {
                //����
                sprintf(buff, "��γ:%f",latitude);
                HalLcdWriteString( buff, HAL_LCD_LINE_3 );  
                sprintf(buff, "����:%f",longitude);                
                HalLcdWriteString( buff, HAL_LCD_LINE_4 );  
            }
        }        
    }
    else
    {
        HalLcdWriteString( "GPSδ��λ", HAL_LCD_LINE_1 );  
    }      
}

