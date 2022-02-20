/*****************************************************************************
 
*****************************************************************************/
#include "main.h"
volatile unsigned int  CountMilliseconds = 0;
volatile unsigned char Timer0Overflow;
 


SIGNAL(SIG_OVERFLOW0)  // T0时钟溢出中断,T0时钟按照 1MHz不停计数，从0-255（256us）中断触发一次
{
 static unsigned char cnt;
 Timer0Overflow++; // T0 中断计数加1
 if(!cnt--)
  {
   cnt = 3;
   CountMilliseconds += 1; // 每过 256*4us 次加 1ms
   if(I2C_Timeout) I2C_Timeout--;
   if(PPM_Timeout) PPM_Timeout--;  else  PPM_SIGNAL_TIME = 0;
   if(UART_Timeout) UART_Timeout--;
  }
}


void Timer0_Init(void)
{
 TCCR0  = TIMER_TEILER;//设 T0 为 clk 的 8 分频（也即定时计数器的计数频率为 1MHz）
// TCNT0 = -TIMER_RELOAD_VALUE;  // reload
 TIM0_START; // 使能 T0 的溢出中断功能
 TIMER2_INT_ENABLE; // 使能 T2 的溢出中断功能
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
// 延时 w ms
*/
void Delay_ms(unsigned int w)
{
 unsigned int akt;
 akt = SetDelay(w);
 while (!CheckDelay(akt));
}