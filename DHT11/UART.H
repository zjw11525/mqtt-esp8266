#include "STC12C5Axx.h"
#include  <intrins.h>
#define uchar unsigned char
#define uint unsigned int

#define FOSC 11059200L			
#define T1MS (65536-FOSC/1000)	
#define BAUD 9600				
#define UART1_Rx_Buff_LEN 100	//串口1数据缓冲区数据长度

bit busy=0;											//串口1发送数据忙标志位
unsigned char UART1_Rx_Buffer[UART1_Rx_Buff_LEN];	//串口1接收保存缓冲区
unsigned char Uart1_Delay=0;						//串口1接收数据帧延时(ms)，延时时间到当1帧数据接收完成
unsigned char Uart1_Write_Count=0;					//写串口1缓冲区指针
unsigned char Uart1_Read_Count=0;					//读串口1缓冲区指针
unsigned char Uart1_Finish=0;						//当前一帧数据接收完成标志

/*******************************************************************************
* 函数名  : UART1_Init_Config
* 描述    : UART1初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART1_Init_Config(void)
{
    SCON = 0x50;					//8位UART,可变波特率,允许串行接收
	TMOD &= 0x0F;					
	TMOD |= 0x20;					//定时器1为8位自动重载模式
    TH1 = TL1 = -(FOSC/12/32/BAUD);	//设置波特率重装值
    TR1 = 1;						//启动定时器1计数
    ES = 1;							//使能串口中断
    EA = 1;							//使能总中断
}
/*******************************************************************************
* 函数名  : UART1_Buffer_PntAdd
* 描述    : 读、写串口1缓冲区指针加1 
* 输入    : *pnt(指向串口1读、写串口1缓冲区指针)
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART1_Buffer_PntAdd(unsigned char *pnt)
{
	*pnt+=1;
	if(*pnt >= UART1_Rx_Buff_LEN) *pnt=0;
}
/*******************************************************************************
* 函数名  : UART1_SendData
* 描述    : UART1发送一个字节
* 输入    : dat：待发送数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART1_SendData(unsigned char dat)
{
    while (busy);	//等待前面的数据发送完成
    busy = 1;		//串口1发送数据忙标志位(1忙,0空闲)
    SBUF = dat;		//写数据到UART数据寄存器
}

/*******************************************************************************
* 函数名  : UART1_SendBuffer
* 描述    : UART1发送一个缓冲区数据
* 输入    : *buff：待发送缓冲区首地址,len：待发送数据长度
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void UART1_SendBuffer(unsigned char *buff,unsigned int len)
{
	unsigned int i=0;

	if(len<=0) return;

	do
	{
		UART1_SendData(buff[i++]);//发送当前字符
	}while(i<len);
}