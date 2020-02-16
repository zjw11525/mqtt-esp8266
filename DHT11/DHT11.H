#include "STC12C5Axx.h"
#include  <intrins.h>
#define uchar unsigned char
#define uint unsigned int

uchar data_byte;
uchar Tem_Z,Tem_X,Hum_Z,Hum_X,Check;

sbit DHT = P0^2;   //����������

void Delay10us()		//STC12@11.0592MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 24;
	while (--i);
}
void Delay20ms()		//STC12@11.0592MHz
{
	unsigned char i, j, k;

	i = 1;
	j = 216;
	k = 35;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void DHT11_start()
{
   DHT=1;
   Delay10us();
   DHT=0;
   Delay20ms();   //��ʱ18ms����
   DHT=1;
	 Delay10us();
	 Delay10us();
	 Delay10us();
	 Delay10us();
   DHT=1;
}
//DHT11 ����һ���ֽ�����
uchar receive_data()
{
	uchar i,temp,count;
	data_byte=0;
	for(i=0;i<8;i++)
	{
		count=2;
		while((!DHT)&&count++); //�ȴ�50us �͵�ƽ����
		Delay10us(); //�ȴ�40us �͵�ƽ,���жϵ�ƽ�ߵ�,��:DHT Ϊ1,��:DHT Ϊ0
		Delay10us();
		Delay10us();
		Delay10us();
		temp=0;
		if(DHT==1) temp=1; //DHT Ϊ��:1,λΪ1
		count=2;
		while((DHT)&&count++); //�ȴ�DHT �ߵ�ƽ����
		if(count==1) break; //DHT Ϊ��:0,λΪ0
		data_byte<<=1; //�ֽ�����
		data_byte|=temp;
	}
	return data_byte; //����ֵ
}
//DHT11 ��������
void Rec_DHT()
{
	uchar flag;
	uchar T_H,T_L,H_H,H_L,check,num_check; //����ɼ������м����
	DHT11_start(); //����������ʼ�ź�
	EA=0;
	if(!DHT) //�ȴ��ӻ���Ӧ�ź�����͵�ƽ
	{
		flag = 2;
		while((!DHT) && flag++); //�жϴӻ��Ƿ񷢳�80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
		flag = 2;
		while(DHT && flag++); //�жϴӻ��Ƿ񷢳�80us �ĸߵ�ƽ,��������ʼ�ɼ�����
		H_H=receive_data(); //�ɼ�ʪ����������
		H_L=receive_data(); //�ɼ�ʪ��С������
		T_H=receive_data(); //�ɼ��¶���������
		T_L=receive_data(); //�ɼ��¶�С������
		check=receive_data(); //�ɼ�У��λ
		DHT = 1;
		num_check=H_H+H_L+T_H+T_L;
		if(num_check==check) //����У��
		{
			Hum_Z =H_H;
			Hum_X =H_L;
			Tem_Z =T_H;
			Tem_X =T_L;
		}
	}
	EA=1;
}
void Delay1ms()		//STC12@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

//������ʱ**************************
void delay_nms(unsigned int k)	
{						
	unsigned int i;				
	for(i=0;i<k;i++)
	{			
		Delay1ms();
	}						
}