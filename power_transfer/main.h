/*
#ifndef x //�Ȳ���x�Ƿ񱻺궨���
#define x
�����1 //���xû�б��궨���������x������������ 1
#else
�����2 //���x�Ѿ�����������������2����䣬�����ӡ������ 1
#endif//��ֹif
*/ // һ��������ֹ��ΰ�����ͬͷ�ļ����µ��ض������


#ifndef _MAIN_H
 #define _MAIN_H

//#define MOTORADRESSE      0  // Adresse (1-4)     0 = Motoradresse ��ber L?tjumper auf BL-Ctrl V1.1

#define MANUELL_PWM       10   // zur manuellen Vorgabe der PWM  Werte: 0-255
#define TEST_MANUELL      0   // zum Testen der Hardware ohne Kommutierungsdetektion  Werte: 0-255 (PWM)
#define TEST_SCHUB        0       // Erzeugt ein Schubmuster        Werte: 0 = normal 1 = Test
#define POLANZAHL        12   // Anzahl der Pole (Magnete) Wird nur zur Drehzahlausgabe ��ber Debug gebraucht
#define MAX_PWM         255
#define MIN_PWM           3
#define MIN_PPM          10   // ab hier (PPM-Signal) schaltet der Regler erst ein
#define FILTER_PPM        7   // wie stark soll das PPM-Signal gefiltert werden (Werte: 0-30)?

#define SIO_DEBUG         0   // Testwertausgaben auf der seriellen Schnittstelle
#define X3D_SIO           0       // serielles Protokoll des X3D (38400Bd) Achtung: dann muss SIO_DEBUG = 0 sein

#define _16KHZ    // Schaltfrequenz  -- die gew��nschte einkommentieren
#define FDD_IRLR 1

#ifdef _16KHZ
#if FDD_IRLR == 1                  // bessere Fets = mehr Strom zulassen
 #define MAX_STROM        200  // ab ca. 20A PWM ausschalten
 #define LIMIT_STROM      120  // ab ca. 12A PWM begrenzen
#else
 #define MAX_STROM        130  // ab ca. 13A PWM ausschalten
 #define LIMIT_STROM       65  // ab ca. 6,5A PWM begrenzen
#endif
#endif

#ifdef _32KHZ
#define MAX_STROM        130  // ab ca. 13A PWM ausschalten
#define LIMIT_STROM       50  // ab ca. 5,0A PWM begrenzen
#endif

#define SYSCLK  8000000L        //ʯӢʱ��Ƶ�� 8MHz

#define GRN_ON  {if(HwVersion < 12) PORTD |= GRUEN; else PORTD &= ~GRUEN;}
#define GRN_OFF {if(HwVersion > 11) PORTD |= GRUEN; else PORTD &= ~GRUEN;}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/boot.h>
#include <avr/wdt.h> // ���Ź�ͷ�ļ�
/*<!-- avr��WDT �Ļ������������������

WDTʹ�ܶ�ʱ�������wdt_enable()���������ԴӼ�ʮms����s֮����������������ʱ�����Ƭ����û�и�λWDT��ʱ��ʱ����Ƭ���ͻ���и�λ������һ����˵��Ҫ���ù��͵�ʱ������
WDT�������wdt_disable()��Ϊ��ֹ�ٴ���д����ʱ��λ�󷴸�����wdt��λ����Ҫ�ڳ�ʼ��������ʼλ�ý���wdt������wdt��ʱ������ù���ʱ��Ϊ��Ҫ��������ܻᵼ���޷��ٴ���д��
WDT��ʱ����λ���wdt_reset()�������WDT�Ķ�ʱ�����û��ʹ�ø���临λ��ʱ������WDT���Զ���λ��Ƭ����������λ�ø��ݾ������ĺ�ʱ������������Ҫ�ദ���롣
 -->*/
//<!-- ����ӵĴ��� -->
#include <inttypes.h>
#include <avr/sleep.h>
#include <util/delay.h>
//<!-- ����ӵĴ��� -->

#include "I2C.h"
#include "old_macros.h"
#include "analog.h"
#include "uart.h"
#include "timer0.h"
#include "PWM.h"

#include "PPM_Decode.h"

extern unsigned int I2C_Timeout;
extern unsigned int UART_Timeout;
extern unsigned int  PWM;
extern unsigned int  Strom;     //ca. in 0,1A
extern unsigned char Strom_max;
extern unsigned char Mittelstrom;
extern unsigned int  Count_Phase_times;
extern unsigned char If_Get_PWM;
extern unsigned char MotorStops;
extern unsigned char IF_TEST_STROM;
extern unsigned char MaxPWM;
extern unsigned char MotorAdresse; //����extern�ؼ��֣�ʹ���ں�߶���ı���

extern unsigned char HwVersion;
extern unsigned char IntRef;
#define ROT    0x08
#define GRUEN  0x80    

#define ADR1   0x40    // 
#define ADR2   0x80    //

#if defined(__AVR_ATmega8__)
#  define OC1   PB1
#  define DDROC DDRB
#  define OCR   OCR1A
#  define PWM10 WGM10
#  define PWM11 WGM11
#endif


#endif //_MAIN_H