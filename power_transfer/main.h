/*
#ifndef x //先测试x是否被宏定义过
#define x
程序段1 //如果x没有被宏定义过，定义x，并编译程序段 1
#else
程序段2 //如果x已经定义过了则编译程序段2的语句，“忽视”程序段 1
#endif//终止if
*/ // 一般用来防止多次包含相同头文件导致的重定义错误


#ifndef _MAIN_H
 #define _MAIN_H

//#define MOTORADRESSE      0  // Adresse (1-4)     0 = Motoradresse über L?tjumper auf BL-Ctrl V1.1

#define MANUELL_PWM       10   // zur manuellen Vorgabe der PWM  Werte: 0-255
#define TEST_MANUELL      0   // zum Testen der Hardware ohne Kommutierungsdetektion  Werte: 0-255 (PWM)
#define TEST_SCHUB        0       // Erzeugt ein Schubmuster        Werte: 0 = normal 1 = Test
#define POLANZAHL        12   // Anzahl der Pole (Magnete) Wird nur zur Drehzahlausgabe über Debug gebraucht
#define MAX_PWM         255
#define MIN_PWM           3
#define MIN_PPM          10   // ab hier (PPM-Signal) schaltet der Regler erst ein
#define FILTER_PPM        7   // wie stark soll das PPM-Signal gefiltert werden (Werte: 0-30)?

#define SIO_DEBUG         0   // Testwertausgaben auf der seriellen Schnittstelle
#define X3D_SIO           0       // serielles Protokoll des X3D (38400Bd) Achtung: dann muss SIO_DEBUG = 0 sein

#define _16KHZ    // Schaltfrequenz  -- die gewünschte einkommentieren
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

#define SYSCLK  8000000L        //石英时钟频率 8MHz

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
#include <avr/wdt.h> // 看门狗头文件
/*<!-- avr的WDT 的基本功能语句有三个：

WDT使能定时间隔设置wdt_enable()，参数可以从几十ms到几s之间调整，当超过这个时间而单片机又没有复位WDT定时器时，单片机就会进行复位操作，一般来说不要设置过低的时间间隔。
WDT禁用语句wdt_disable()，为防止再次烧写程序时复位后反复进入wdt复位，需要在初始化程序的最开始位置禁用wdt，这在wdt定时间隔设置过短时尤为重要，否则可能会导致无法再次烧写。
WDT定时器复位语句wdt_reset()，如果在WDT的定时间隔内没有使用该语句复位定时器，则WDT会自动复位单片机，该语句的位置根据具体程序的耗时决定，可能需要多处加入。
 -->*/
//<!-- 新添加的代码 -->
#include <inttypes.h>
#include <avr/sleep.h>
#include <util/delay.h>
//<!-- 新添加的代码 -->

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
extern unsigned char MotorAdresse; //利用extern关键字，使用在后边定义的变量

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