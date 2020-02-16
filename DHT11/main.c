#include <stdio.h>//C标准输入输出库，用到了sprintf
#include "DHT11.H"//DHT11库
#include "UART.H"
#include <string.h>

/*******************************************************************************
* 函数名  : Timer0_Init
* 描述    : 定时器0初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void Timer0_Init(void)
{
    AUXR |= 0x80;		//定时器0为1T模式
	TMOD &= 0xF0;		
    TMOD |= 0x01;		//设置定时器为模式0(16位自动重装载)
    TL0 = T1MS;			//初始化计时值
    TH0 = T1MS >> 8;
    TR0 = 1;			//定时器0开始计时
    ET0 = 1;			//使能定时器0中断
    EA = 1;				//使能总中断							
}
/*******************************************************************************
* 函数名  : main
* 描述    : 主函数，用户程序从main函数开始运行
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void main(void)
{
	char p[] = "     ";
	char str[8] = {0};
	Timer0_Init();			//定时器0初始化配置
	UART1_Init_Config();	//UART1初始化配置
	while(1) 
	{	
		//温湿度
		Rec_DHT();//读取DHT11
		sprintf(p,"%d,%d",(int)Tem_Z,(int)Hum_Z);		
		UART1_SendBuffer(p,sizeof(p));

		if(Uart1_Write_Count != Uart1_Read_Count)//如果读指针不等写指针,则证明串口1接收到数据
		{
		
		}
		else if(Uart1_Finish == 1) Uart1_Finish = 0;

		delay_nms(1500);			
	}
}

/*******************************************************************************
* 函数名  : UART1_ISR
* 描述    : UART1中断服务函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART1_ISR(void) interrupt 4 using 1
{
	unsigned char ch;
	//接收数据
	if(RI)
	{
		RI = 0;//清除RI位
		ch=SBUF;
		UART1_Rx_Buffer[Uart1_Write_Count]=ch;	//将接收到的数据写入缓冲区
		UART1_Buffer_PntAdd(&Uart1_Write_Count);//写串口1缓冲区指针加1			
		if(Uart1_Write_Count == Uart1_Read_Count)//如果读、写缓冲区指针重叠,则读指针加1,这时将丢失1个字节数据
		{
			UART1_Buffer_PntAdd(&Uart1_Read_Count);//读串口1缓冲区指针加1
		}
		Uart1_Delay = 20;//串口1接收数据帧延时(ms)，延时时间到当1帧数据接收完成
	}
	//发送数据
	if (TI)			
	{
		TI = 0;		//清除TI位
		busy = 0;	//清忙标志(1忙,0空闲)
	}
}
/*******************************************************************************
* 函数名  : Timer0_ISR
* 描述    : 定时器0中断服务函数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 本程序用于检测1帧串口数据接收完成
*******************************************************************************/
void Timer0_ISR() interrupt 1 using 1
{
	if(Uart1_Delay>0)
	{
		Uart1_Delay--;
		if(Uart1_Delay==0)
		{
			//延时时间到再没有接收到新的串口数据，表示1帧数据接收完成
			if(Uart1_Write_Count != Uart1_Read_Count) Uart1_Finish=1;
		}
	}

    TL0 = T1MS;			//初始化计时值
    TH0 = T1MS >> 8;
}


