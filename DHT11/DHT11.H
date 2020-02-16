#include "STC12C5Axx.h"
#include  <intrins.h>
#define uchar unsigned char
#define uint unsigned int

uchar data_byte;
uchar Tem_Z,Tem_X,Hum_Z,Hum_X,Check;

sbit DHT = P0^2;   //定义数据线

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
   Delay20ms();   //延时18ms以上
   DHT=1;
	 Delay10us();
	 Delay10us();
	 Delay10us();
	 Delay10us();
   DHT=1;
}
//DHT11 接收一个字节数据
uchar receive_data()
{
	uchar i,temp,count;
	data_byte=0;
	for(i=0;i<8;i++)
	{
		count=2;
		while((!DHT)&&count++); //等待50us 低电平结束
		Delay10us(); //等待40us 低电平,后判断电平高低,高:DHT 为1,低:DHT 为0
		Delay10us();
		Delay10us();
		Delay10us();
		temp=0;
		if(DHT==1) temp=1; //DHT 为高:1,位为1
		count=2;
		while((DHT)&&count++); //等待DHT 高电平结束
		if(count==1) break; //DHT 为低:0,位为0
		data_byte<<=1; //字节左移
		data_byte|=temp;
	}
	return data_byte; //返回值
}
//DHT11 接收数据
void Rec_DHT()
{
	uchar flag;
	uchar T_H,T_L,H_H,H_L,check,num_check; //定义采集参数中间变量
	DHT11_start(); //主机发出开始信号
	EA=0;
	if(!DHT) //等待从机响应信号输出低电平
	{
		flag = 2;
		while((!DHT) && flag++); //判断从机是否发出80us 的低电平响应信号是否结束
		flag = 2;
		while(DHT && flag++); //判断从机是否发出80us 的高电平,若发出则开始采集数据
		H_H=receive_data(); //采集湿度整数部分
		H_L=receive_data(); //采集湿度小数部分
		T_H=receive_data(); //采集温度整数部分
		T_L=receive_data(); //采集温度小数部分
		check=receive_data(); //采集校验位
		DHT = 1;
		num_check=H_H+H_L+T_H+T_L;
		if(num_check==check) //进行校验
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

//毫秒延时**************************
void delay_nms(unsigned int k)	
{						
	unsigned int i;				
	for(i=0;i<k;i++)
	{			
		Delay1ms();
	}						
}
