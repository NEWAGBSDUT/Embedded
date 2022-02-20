#ifndef _UART_H
 #define _UART_H

extern unsigned volatile char UART_Norminal_Value;
extern unsigned volatile char UART_Send_Busy;
extern int debug_time;

extern void UART_Init (void);
extern int  UART_Putchar (char c);
extern void UART_SendUart(void);
extern void UART_DisplayDebugInfo(void);

struct UART_Debug_Out
{
 unsigned char Digital[2];
 unsigned int  Analog[8];
};
extern struct UART_Debug_Out UART_Debug_Info;

#if X3D_SIO == 1
 #define BAUD_RATE 38400                //Baud Rate für die Serielle Schnittstelle     
#else
 #define BAUD_RATE 57600                //Baud Rate für die Serielle Schnittstelle     
#endif

//Anpassen der seriellen Schnittstellen Register wenn ein ATMega128 benutzt wird
#if defined (__AVR_ATmega128__)
#       define USR UCSR0A
#       define UCR UCSR0B
#       define UDR UDR0
#       define UBRR UBRR0L
#       define EICR EICRB
#endif

#if defined (__AVR_ATmega8__)
#       define USR UCSRA
#       define UCR UCSRB
#       define UBRR UBRRL
#       define EICR EICRB
#   define INT_VEC_RX  SIG_UART_RECV
#   define INT_VEC_TX  SIG_UART_TRANS
#endif

#if defined (__AVR_ATmega32__)
#       define USR UCSRA
#       define UCR UCSRB
#       define UBRR UBRRL
#       define EICR EICRB
#   define INT_VEC_RX  SIG_UART_RECV
#   define INT_VEC_TX  SIG_UART_TRANS
#endif

#if defined (__AVR_ATmega644__)
#       define USR  UCSR0A
#       define UCR  UCSR0B
#       define UDR  UDR0
#       define UBRR UBRR0L
#       define EICR EICR0B
#   define TXEN TXEN0
#   define RXEN RXEN0
#   define RXCIE RXCIE0
#   define TXCIE TXCIE0
#   define U2X  U2X0
#   define UCSRB UCSR0B
#   define UDRE UDRE0
#   define INT_VEC_RX  SIG_USART_RECV
#   define INT_VEC_TX  SIG_USART_TRANS
#endif

#endif //_UART_H