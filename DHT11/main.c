#include <stdio.h>//C��׼��������⣬�õ���sprintf
#include "DHT11.H"//DHT11��
#include "UART.H"
#include <string.h>

/*******************************************************************************
* ������  : Timer0_Init
* ����    : ��ʱ��0��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void Timer0_Init(void)
{
    AUXR |= 0x80;		//��ʱ��0Ϊ1Tģʽ
	TMOD &= 0xF0;		
    TMOD |= 0x01;		//���ö�ʱ��Ϊģʽ0(16λ�Զ���װ��)
    TL0 = T1MS;			//��ʼ����ʱֵ
    TH0 = T1MS >> 8;
    TR0 = 1;			//��ʱ��0��ʼ��ʱ
    ET0 = 1;			//ʹ�ܶ�ʱ��0�ж�
    EA = 1;				//ʹ�����ж�							
}
/*******************************************************************************
* ������  : main
* ����    : ���������û������main������ʼ����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void main(void)
{
	char p[] = "     ";
	char str[8] = {0};
	Timer0_Init();			//��ʱ��0��ʼ������
	UART1_Init_Config();	//UART1��ʼ������
	while(1) 
	{	
		//��ʪ��
		Rec_DHT();//��ȡDHT11
		sprintf(p,"%d,%d",(int)Tem_Z,(int)Hum_Z);		
		UART1_SendBuffer(p,sizeof(p));

		if(Uart1_Write_Count != Uart1_Read_Count)//�����ָ�벻��дָ��,��֤������1���յ�����
		{
		
		}
		else if(Uart1_Finish == 1) Uart1_Finish = 0;

		delay_nms(1500);			
	}
}

/*******************************************************************************
* ������  : UART1_ISR
* ����    : UART1�жϷ�����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void UART1_ISR(void) interrupt 4 using 1
{
	unsigned char ch;
	//��������
	if(RI)
	{
		RI = 0;//���RIλ
		ch=SBUF;
		UART1_Rx_Buffer[Uart1_Write_Count]=ch;	//�����յ�������д�뻺����
		UART1_Buffer_PntAdd(&Uart1_Write_Count);//д����1������ָ���1			
		if(Uart1_Write_Count == Uart1_Read_Count)//�������д������ָ���ص�,���ָ���1,��ʱ����ʧ1���ֽ�����
		{
			UART1_Buffer_PntAdd(&Uart1_Read_Count);//������1������ָ���1
		}
		Uart1_Delay = 20;//����1��������֡��ʱ(ms)����ʱʱ�䵽��1֡���ݽ������
	}
	//��������
	if (TI)			
	{
		TI = 0;		//���TIλ
		busy = 0;	//��æ��־(1æ,0����)
	}
}
/*******************************************************************************
* ������  : Timer0_ISR
* ����    : ��ʱ��0�жϷ�����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���������ڼ��1֡�������ݽ������
*******************************************************************************/
void Timer0_ISR() interrupt 1 using 1
{
	if(Uart1_Delay>0)
	{
		Uart1_Delay--;
		if(Uart1_Delay==0)
		{
			//��ʱʱ�䵽��û�н��յ��µĴ������ݣ���ʾ1֡���ݽ������
			if(Uart1_Write_Count != Uart1_Read_Count) Uart1_Finish=1;
		}
	}

    TL0 = T1MS;			//��ʼ����ʱֵ
    TH0 = T1MS >> 8;
}

