#include "gps.h"
#include "usart.h"
/***************************************************\
*�������ܣ���buf����õ���cx���������ڵ�λ��
*����ֵ��
*����ֵ��0~0xFE������������λ�õı���
*				 0xFF���������ڵ�cx������
\***************************************************/

uint8_t NMEA_Comma_Pos(uint8_t *buf,uint8_t cx)
{
	uint8_t *p = buf;
	while(cx)
	{
		if(*buf=='*'||*buf<' '||*buf>'z')return 0xFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;
}
/***************************************************\
*�������ܣ�����m��n�η�ֵ
*����ֵ������m��ָ��n
*����ֵ��m^n
\***************************************************/
uint32_t NMEA_Pow(uint8_t m,uint8_t n)
{
	uint32_t result = 1;
	while(n--)result *= m;
	return result;
}
/***************************************************\
*�������ܣ�str����ת��Ϊ��int�����֣���','����'*'����
*����ֵ��buf�����ִ洢��
*				 dx��С����λ�������ظ����ú���
*����ֵ��ת�������ֵ
\***************************************************/
int NMEA_Str2num(uint8_t *buf,uint8_t*dx)
{
	uint8_t *p = buf;
	uint32_t ires = 0,fres = 0;
	uint8_t ilen = 0,flen = 0,i;
	uint8_t mask = 0;
	int res;
	while(1)
	{
		if(*p=='-'){mask |= 0x02;p++;}//˵���и���
		if(*p==','||*p=='*')break;//����������
		if(*p=='.'){mask |= 0x01;p++;}//����С����
		else if(*p>'9'||(*p<'0'))//���ֲ���0��9֮�ڣ�˵���зǷ��ַ�
		{
			ilen = 0;
			flen = 0;
			break;
		}
		if(mask&0x01)flen++;//С�����λ��
		else ilen++;//str���ȼ�һ
		p++;//��һ���ַ�
	}
	if(mask&0x02)buf++;//�Ƶ���һλ����ȥ����
	for(i=0;i<ilen;i++)//�õ�������������
	{
		ires += NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;//���ȡ��λС��
	*dx = flen;
	for(i=0;i<flen;i++)//�õ�С����������
	{
		fres +=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	}
	res = ires*NMEA_Pow(10,flen)+fres;
	if(mask&0x02)res = -res;
	return res;
}
/***************************************************\
*�������ܣ�����GPRMC��Ϣ
*����ֵ��gpsx,NMEA��Ϣ�ṹ��
*				 buf�����յ���GPS���ݻ������׵�ַ
\***************************************************/
void NMEA_BDS_GPRMC_Analysis(gps_msg *gpsmsg,uint8_t *buf)
{
	uint8_t *p4,dx;			 
	uint8_t posx;     
	uint32_t temp;	   
	float rs;  
	p4=(uint8_t*)strstr((const char *)buf,"$GPRMC");//"$GPRMC",������&��GPRMC�ֿ������,��ֻ�ж�GPRMC.
	posx=NMEA_Comma_Pos(p4,3);								//�õ�γ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p4+posx,&dx);		 	 
		gpsmsg->latitude_bd=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsmsg->latitude_bd=gpsmsg->latitude_bd*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p4,4);								//��γ���Ǳ�γ 
	if(posx!=0XFF)gpsmsg->nshemi_bd=*(p4+posx);					 
 	posx=NMEA_Comma_Pos(p4,5);								//�õ�����
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p4+posx,&dx);		 	 
		gpsmsg->longitude_bd=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsmsg->longitude_bd=gpsmsg->longitude_bd*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	posx=NMEA_Comma_Pos(p4,6);								//������������
	if(posx!=0XFF)gpsmsg->ewhemi_bd=*(p4+posx);		  
}

