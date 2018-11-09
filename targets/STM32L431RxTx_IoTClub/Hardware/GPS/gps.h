#ifndef __gps_h__
#define __gps_h__
#include "stm32l4xx_hal.h"
#include "string.h"
#include "stdio.h"

/***************************************************\
*GPS NMEA-0183Э����Ҫ�����ṹ�嶨��
*������Ϣ
\***************************************************/
__packed typedef struct
{
	uint32_t latitude_bd;					//γ��   ������100000����ʵ��Ҫ����100000
	uint8_t nshemi_bd;						//��γ/��γ,N:��γ;S:��γ	
	uint32_t longitude_bd;			    	//���� ������100000��,ʵ��Ҫ����100000
	uint8_t ewhemi_bd;						//����/����,E:����;W:����
}gps_msg;
/***************************************************\
*������������
\***************************************************/
uint8_t NMEA_Comma_Pos(uint8_t *buf,uint8_t cx);
uint32_t NMEA_Pow(uint8_t m,uint8_t n);
int NMEA_Str2num(uint8_t *buf,uint8_t*dx);

/***************************************************\
*��ʼ��GPS
\***************************************************/
void GPS_Init(void);

/***************************************************\
*GPS���ݽӿ�
\***************************************************/
void NMEA_BDS_GPRMC_Analysis(gps_msg *gpsmsg,uint8_t *buf);
#endif
