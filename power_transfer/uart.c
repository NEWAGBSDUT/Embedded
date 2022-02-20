// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// + Regler für Brushless-Motoren
// + ATMEGA8 mit 8MHz
// + (c) 01.2007 Holger Buss
// + Nur für den privaten Gebrauch / NON-COMMERCIAL USE ONLY
// + Keine Garantie auf Fehlerfreiheit
// + Kommerzielle Nutzung nur mit meiner Zustimmung
// + Der Code ist für die Hardware BL_Ctrl V1.0 entwickelt worden
// + www.mikrocontroller.com
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "main.h"
#include "uart.h"

#define MAX_SEND_BUFF     100
#define MAX_RECEIVE_BUFF  100

unsigned volatile char UART_Norminal_Value = 0;
unsigned volatile char UDR_temp = 0;
unsigned volatile char send_buffer[MAX_SEND_BUFF];
unsigned volatile char receive_buffer[MAX_RECEIVE_BUFF];

unsigned volatile char UART_Send_Busy = 1;

unsigned char MotorTest[4] = {0,0,0,0};
unsigned volatile char recevie_bytes = 0;

struct UART_Debug_Out UART_Debug_Info;

int debug_time;

void UART_SendUart(void)
{
   static unsigned int ptr = 0;
   unsigned char tmp_tx;
   if(!(UCSRA & 0x40)) return;
   if(!UART_Send_Busy)
   {
       ptr++;
       tmp_tx = send_buffer[ptr];
       if((tmp_tx == '\r') || (ptr == MAX_SEND_BUFF))
       {
           ptr = 0;
           UART_Send_Busy = 1;
       }
       USR |= (1<<TXC);
       UDR = tmp_tx;
   }
   else ptr = 0;
}
// --------------------------------------------------------------------------
void Decode64(unsigned char *ptrOut, unsigned char len, unsigned char ptrIn,unsigned char max)
{
 unsigned char a,b,c,d;
 unsigned char ptr = 0;
 unsigned char x,y,z;
 while(len)
  {
   a = receive_buffer[ptrIn++] - '=';
   b = receive_buffer[ptrIn++] - '=';
   c = receive_buffer[ptrIn++] - '=';
   d = receive_buffer[ptrIn++] - '=';
   if(ptrIn > max - 2) break;

   x = (a << 2) | (b >> 4);
   y = ((b & 0x0f) << 4) | (c >> 2);
   z = ((c & 0x03) << 6) | d;

   if(len--) ptrOut[ptr++] = x; else break;
   if(len--) ptrOut[ptr++] = y; else break;
   if(len--) ptrOut[ptr++] = z; else break;
  }
}
SIGNAL(INT_VEC_TX)
{
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++ 接收完数据中断
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SIGNAL(INT_VEC_RX)
{ 
#if  X3D_SIO == 1	// 波特率 选择 标志
   static unsigned char serPacketCounter = 100;
   UDR_temp = UDR;
   if(UDR_temp == 0xF5)     // Startzeichen
   {
       serPacketCounter = 0;
   }
   else
   {
       if(++serPacketCounter == MotorAdresse) // (1-4)
       {
           UART_Norminal_Value = UDR_temp;
           UART_Timeout = 200; // werte für 200ms gültig
       }
       else
       {
           if(serPacketCounter > 100) serPacketCounter = 100;
       }
   }
#else // 当前选择 57600 波特率,执行以下代码 X3D_SIO = 0
   static unsigned int  crc;
   static unsigned char crc1,crc2,buf_ptr;
   static unsigned char UartState = 0;
   unsigned char CrcOkay = 0;
 
   UDR_temp = UDR;
   if(buf_ptr >= MAX_RECEIVE_BUFF)    UartState = 0;
   if(UDR_temp == '\r' && UartState == 2)
   {
	   UART_Putchar('y');
       UartState = 0;
       crc -= receive_buffer[buf_ptr-2];
       crc -= receive_buffer[buf_ptr-1];
       crc %= 4096;
       crc1 = '=' + crc / 64;
       crc2 = '=' + crc % 64;
       CrcOkay = 0;
       if((crc1 == receive_buffer[buf_ptr-2]) && (crc2 == receive_buffer[buf_ptr-1])) CrcOkay = 1; else { CrcOkay = 0; };
       if(CrcOkay)
       {
           recevie_bytes = buf_ptr;     
           receive_buffer[buf_ptr] = '\r';
           if((receive_buffer[2] == 'R')) wdt_enable(WDTO_250MS); // Reset- 看门狗
           UART_Putchar(receive_buffer[2]);         
           if (receive_buffer[2] == 't')
           { 
               Decode64((unsigned char *) &MotorTest[0],sizeof(MotorTest),3,recevie_bytes);
               UART_Norminal_Value = MotorTest[MotorAdresse - 1];
               UART_Timeout = 500;
           }
       }
   }
  else
  switch(UartState)
  {
   case 0:
          if(UDR_temp == '#') UartState = 1;// 已经获取到开始标志： #
                  buf_ptr = 0;
                  receive_buffer[buf_ptr++] = UDR_temp;
                  crc = UDR_temp;
          break;
   case 1:
                  UartState++;
                  receive_buffer[buf_ptr++] = UDR_temp;
                  crc += UDR_temp;
                  break;
   case 2:
                  receive_buffer[buf_ptr] = UDR_temp;
                  if(buf_ptr < MAX_RECEIVE_BUFF) buf_ptr++;
                  else UartState = 0;
                  crc += UDR_temp;
                  break;
   default:
          UartState = 0;
          break;
  }
#endif

};
// --------------------------------------------------------------------------
void AddCRC(unsigned int num1)//7  send length=10
{
   unsigned int tmpCRC = 0,i;
   for(i = 0; i < num1;i++)
   {
       tmpCRC += send_buffer[i];
   }
   tmpCRC %= 4096;
   send_buffer[i++] = '=' + tmpCRC / 64;
   send_buffer[i++] = '=' + tmpCRC % 64;
   send_buffer[i++] = '\r';
   UART_Send_Busy = 0;
   UDR = send_buffer[0];
}
// --------------------------------------------------------------------------
void SendOutData(unsigned char cmd,unsigned char modul, unsigned char *snd, unsigned char len)
{
 unsigned int  pt = 0;
 unsigned char a,b,c;
 unsigned char ptr = 0;

 send_buffer[pt++] = '#';               // start logo
 send_buffer[pt++] = modul;             // adress (a=0; b=1,...)
 send_buffer[pt++] = cmd;               // Commond

 while(len)
  {
   if(len) { a = snd[ptr++]; len--;} else a = 0;
   if(len) { b = snd[ptr++]; len--;} else b = 0;
   if(len) { c = snd[ptr++]; len--;} else c = 0;
   send_buffer[pt++] = '=' + (a >> 2);
   send_buffer[pt++] = '=' + (((a & 0x03) << 4) | ((b & 0xf0) >> 4));
   send_buffer[pt++] = '=' + (((b & 0x0f) << 2) | ((c & 0xc0) >> 6));
   send_buffer[pt++] = '=' + ( c & 0x3f);
  }
 AddCRC(pt);
}
//############################################################################
// 串口发送函数
// 参数：要发送的字符
int UART_Putchar (char c)
//############################################################################
{
	if (c == '\n')
		UART_Putchar('\r');
        
    loop_until_bit_is_set(USR, UDRE); // 等待 usart 数据寄存器清空
         
    UDR = c; // usart 数据寄存器清空后 将数据写入8位数据寄存器
       
    return (0);
}
//############################################################################
//初始化usart并启动连续界面，
// usart 中三个完全独立的中断，TX发送完成、TX发送数据寄存器空、RX接收完成
void UART_Init (void)
//############################################################################
{	
	UCR  =  (1 << TXEN) | (1 << RXEN);/* UCSRB Enable receiver and transmitter */ 
    USR |=  (1<<U2X); /* UCSRA Double the USART transmission speed*/
    UCR |=  (1<<RXCIE);/*RX Complete Interrupt Enable*/
    UBRR =  (SYSCLK / (BAUD_RATE * 8L) -1 );	// 初始化 波特率 寄存器 低8位 UBRRL（0x10: 16）page155,max:1Mbps
	
    fdevopen (UART_Putchar, NULL); //用fdevopen()给printf()重新定义输入输出流，下面给一下把浮点数从串口输出的程序
    debug_time = SetDelay(200);
	// 启动时输出
    UART_Putchar ('B');UART_Putchar ('K');UART_Putchar ('_');UART_Putchar ('L');UART_Putchar ('E');UART_Putchar ('A');UART_Putchar ('R');UART_Putchar ('N');UART_Putchar (':');
    UART_Putchar ('V');UART_Putchar ('1');UART_Putchar ('.');UART_Putchar ('2');
    UART_Putchar ('\n');UART_Putchar('H');UART_Putchar('W');UART_Putchar (':');UART_Putchar('1');UART_Putchar('.');UART_Putchar ((0x30-10) + HwVersion);
    UART_Putchar ('\n');UART_Putchar ('A');UART_Putchar ('D');UART_Putchar ('R'); UART_Putchar (':'); UART_Putchar (0x30 + MotorAdresse);
	UART_Putchar ('\n');UART_Putchar ('R');UART_Putchar ('e');UART_Putchar ('b');UART_Putchar ('u');UART_Putchar ('i');UART_Putchar ('l');
	UART_Putchar ('d');UART_Putchar (':');UART_Putchar ('Y');UART_Putchar ('D');UART_Putchar ('T');
}
//---------------------------------------------------------------------------------------------
void UART_DisplayDebugInfo(void)  
{
   if((CheckDelay(debug_time) && UART_Send_Busy))
   {
       SendOutData('D',MotorAdresse,(unsigned char *) &UART_Debug_Info,sizeof(UART_Debug_Info));
       debug_time = SetDelay(50);
   }
}