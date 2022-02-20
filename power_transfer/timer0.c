/*****************************************************************************
 
*****************************************************************************/
#include "main.h"
volatile unsigned int  CountMilliseconds = 0;
volatile unsigned char Timer0Overflow;
 


SIGNAL(SIG_OVERFLOW0)  // T0ʱ������ж�,T0ʱ�Ӱ��� 1MHz��ͣ��������0-255��256us���жϴ���һ��
{
 static unsigned char cnt;
 Timer0Overflow++; // T0 �жϼ�����1
 if(!cnt--)
  {
   cnt = 3;
   CountMilliseconds += 1; // ÿ�� 256*4us �μ� 1ms
   if(I2C_Timeout) I2C_Timeout--;
   if(PPM_Timeout) PPM_Timeout--;  else  PPM_SIGNAL_TIME = 0;
   if(UART_Timeout) UART_Timeout--;
  }
}


void Timer0_Init(void)
{
 TCCR0  = TIMER_TEILER;//�� T0 Ϊ clk �� 8 ��Ƶ��Ҳ����ʱ�������ļ���Ƶ��Ϊ 1MHz��
// TCNT0 = -TIMER_RELOAD_VALUE;  // reload
 TIM0_START; // ʹ�� T0 ������жϹ���
 TIMER2_INT_ENABLE; // ʹ�� T2 ������жϹ���
}


unsigned int SetDelay(unsigned int t)
{
  return(CountMilliseconds + t - 1);                                            
}

char CheckDelay (unsigned int t)
{
  return(((t - CountMilliseconds) & 0x8000) >> 8);
}
/*
// ��ʱ w ms
*/
void Delay_ms(unsigned int w)
{
 unsigned int akt;
 akt = SetDelay(w);
 while (!CheckDelay(akt));
}