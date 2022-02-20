#include "main.h"

unsigned int  PWM = 0;
unsigned int  Strom = 0,RuheStrom; //ca. in 0,1A
unsigned char Strom_max = 0;
unsigned char Mittelstrom = 0;
unsigned int  Drehzahl = 0;  // in 100UPM  60 = 6000
unsigned int  KommutierDelay = 10;

unsigned int  I2C_Timeout = 0; // 指示 I2C 是否最近收到数据
unsigned int  UART_Timeout = 0; // 指示串口是否最近收到数据
unsigned int  SollDrehzahl = 0;
unsigned int  IstDrehzahl = 0;
unsigned int  DrehZahlTabelle[256];//vorberechnete Werte zur Drehzahlerfassung
unsigned char CountTimes = 1;
unsigned char If_Get_PWM = 0;
unsigned char MotorStops = 1;
unsigned char MaxPWM = MAX_PWM;
unsigned int  Count_Phase_times = 0; // 指定时间内的换相次数
unsigned int  Current_Speed2UART = 0;  // 发送到串口的当前转速
unsigned char IF_TEST_STROM = 1;
unsigned char MotorAdresse = 1;

unsigned char HwVersion;
unsigned char IntRef = 0;
unsigned int  MinUpmPulse;

void DebugAusgaben(void)
{
    UART_Debug_Info.Analog[0] = Strom;
    UART_Debug_Info.Analog[1] = Mittelstrom;
    UART_Debug_Info.Analog[2] = Current_Speed2UART;
    UART_Debug_Info.Analog[3] = PPM_Signal;
    UART_Debug_Info.Analog[4] = OCR2;
//    UART_Debug_Info.Analog[5] = PWM;
}
//############################################################################
//

//############################################################################
// 微秒级延时
void Wait(unsigned char dauer)
//############################################################################
{
   dauer = (unsigned char)TCNT0 + dauer;
   while((TCNT0 - dauer) & 0x80);
}

void RotBlink(unsigned char anz)
{
   sei(); // Interrupts ein
   while(anz--)
   {
       PORTC |= ROT;
       Delay_ms(300);    
       PORTC &= ~ROT;
       Delay_ms(300);    
   }
   Delay_ms(1000);
}

void GrnBlink(unsigned char anz)
{
   sei();
   while(anz--)
   {  
	   GRN_OFF;
       Delay_ms(300);   
	   GRN_ON;
       Delay_ms(300); 
   }
   Delay_ms(1000);
}

//############################################################################
// 启动算法
char MotorStartAlgorithm(unsigned char pwm)
//############################################################################
{
    unsigned long timer = 300,i;
    DISABLE_SENSE_INT; // 关闭模拟比较器
    PWM = 5;
    SetPWM();//将这个全局变量“PWM”设到 OCR1A, OCR1B 和 OCR2 中去
    Manuell();

    MinUpmPulse = SetDelay(300);
    while(!CheckDelay(MinUpmPulse))
    {
        ADC_FastConvert();
        if(Strom > 120) // 如果发现电流大于 12A
        {
            STEUER_OFF; // 关掉所有 MOSFET
            RotBlink(10); // 红灯闪 10 下
            return(0);
        }
    }
    PWM = pwm;
    while(1)
    {
        for(i=0;i<timer; i++)
        {
            if(!UART_Send_Busy)  UART_SendUart();
            else UART_DisplayDebugInfo();
            Wait(100);  // warten
        }
        DebugAusgaben();
        ADC_FastConvert();
        if(Strom > 60) // 如果大于 6A
          {
            STEUER_OFF; // 关掉所有 MOSFET
            RotBlink(10); // 红灯闪 10 下
            return(0);
          }  
         
        timer-= (timer/15+1);
        if(timer < 25) { if(TEST_MANUELL) timer = 25; else return(1); }
        Manuell();
        Phase++;
        Phase %= 6;
        ADC_Convert();
        PWM = pwm;
        SetPWM();
        if(SENSE)
            {
            PORTD ^= GRUEN;
            }
    }
}

#define TEST_STROMGRENZE 120
unsigned char DelayM(unsigned int timer)
{
    while(timer--)
    {
        ADC_FastConvert();
        if(Strom > (TEST_STROMGRENZE + RuheStrom))
        {
            FETS_OFF; // 关闭所有场效应管
            return(1);
        }
    }
    return(0);  
}

/*
void ShowSense(void)
{
 if(SENSE_H) { PORTC |= ROT; } else { PORTC &= ~ROT;}

}
*/
#define HIGH_A_EIN PORTB |= 0x08
#define HIGH_B_EIN PORTB |= 0x04
#define HIGH_C_EIN PORTB |= 0x02
#define LOW_A_EIN  PORTD |= 0x08
#define LOW_B_EIN  PORTD |= 0x10
#define LOW_C_EIN  PORTD |= 0x20

unsigned char Delay(unsigned int timer)
{
 while(timer--)
  {
//   if(SENSE_H) { PORTC |= ROT; } else { PORTC &= ~ROT;}
  }
 return(0);  
}
// 启动 MOSFET 自检程序，并弄点声音给你听听
void MotorTon(void)
//############################################################################
{
    unsigned char ADR_TAB[5] = {0,0,2,1,3};
    unsigned int timer = 300,i;
    unsigned int t = 0;
    unsigned char anz = 0,MosfetOkay = 0, grenze = 50;

    PORTC &= ~ROT; // 关掉红灯
    Delay_ms(300 * ADR_TAB[MotorAdresse]);
    DISABLE_SENSE_INT; // 关闭模拟比较器中断，停止过零换相检测
    cli();//关闭所有中断
    UART_Putchar('\n');
    STEUER_OFF; // 关闭所有场效应管
    Strom_max = 0; // 最大电流
	
    DelayM(50);
    RuheStrom = Strom_max;
	
    if(UDR == ' ') {t = 65535; grenze = 40; UART_Putchar('_');} else t = 1000; // Ausführlicher Test
    Strom = 0;
	for(i=0;i<t;i++)
	{
	    LOW_A_EIN; // 打开 A 相的下单臂（即 NA-管）
	    DelayM(1);		
	    if(Strom > grenze + RuheStrom) {anz = 1; UART_Putchar('A'); FETS_OFF; break;}		
	    FETS_OFF; // 关闭所有场效应管
	    Delay(5);
	    HIGH_A_EIN; // 打开 A 相的上单臂（即 NA+管）
	    DelayM(1);
	    FETS_OFF;
	    if(Strom > grenze + RuheStrom) {anz = 2; UART_Putchar('a'); FETS_OFF; break;}
	    Delay(5);
		
	}
	if(anz) while(1) RotBlink(anz);  // 若有场效应管击穿，则红灯不停闪烁
	Delay(10000);
	
    Strom = 0;
    for(i=0;i<t;i++)
    {
        LOW_B_EIN;
        DelayM(1);
		if(Strom > grenze + RuheStrom) {anz = 3; UART_Putchar('B'); FETS_OFF; break;}
        FETS_OFF;
        Delay(5);
        HIGH_B_EIN;
        DelayM(1);
        FETS_OFF;
        if(Strom > grenze + RuheStrom) {anz = 4; UART_Putchar('b'); FETS_OFF; break;}
        Delay(5);
		
    }
	if(anz) while(1) RotBlink(anz);  // 若有场效应管击穿，则红灯不停闪烁
	Delay(10000);

	Strom = 0;
	for(i=0;i<t;i++)
	{
	    LOW_C_EIN;
	    DelayM(1);
		if(Strom > grenze + RuheStrom) {anz = 5; UART_Putchar('C'); FETS_OFF; break;}
	    FETS_OFF;
	    Delay(5);
	    HIGH_C_EIN;
	    DelayM(1);
	    FETS_OFF;
	    if(Strom > grenze + RuheStrom) {anz = 6; UART_Putchar('c'); FETS_OFF; break;}
	    Delay(5);
		
	}
    if(anz) while(1) RotBlink(anz);  // 若有场效应管击穿，则红灯不停闪烁
    
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ High-Mosfets auf Schalten testen
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    SENSE_A; // 0 | IntRef IntRef = 0xc0；1100 0000  转换 ADC0 电流值
	Strom = 0;
	
#define TONDAUER 40000    
#define SOUND_E  1  // 1
#define SOUND1_A 300
#define SOUND2_A 330
#define SOUND3_A 360
#define CHECK_STROM_MAX 128
#define CHECK_STROM_MIN 50

    FETS_OFF; // 关闭所有场效应管
    LOW_B_EIN; // Low B ein
    LOW_C_EIN; // Low C ein
    for(i=0; i< (TONDAUER / SOUND2_A) ; i++)
    {
        HIGH_A_EIN; // Test A
        Delay(SOUND_E);
		Strom = MessAD(0);
		
        PORTB = 0;
        Delay(SOUND1_A);
		
		if(Strom > CHECK_STROM_MIN) { MosfetOkay |= 0x01; } else { MosfetOkay &= ~0x01;};
    }    
	FETS_OFF;
	
    LOW_A_EIN; // Low A ein
    LOW_C_EIN; // Low C ein
    for(i=0; i<(TONDAUER / SOUND1_A); i++)
    {
        HIGH_B_EIN; // Test B
        Delay(SOUND_E);
		Strom = MessAD(1);
		
        PORTB = 0;
        Delay(SOUND1_A);
		
				if(Strom > CHECK_STROM_MIN) { MosfetOkay |= 0x02; } else { MosfetOkay &= ~0x02;};
    }
    FETS_OFF;
	
    LOW_A_EIN; // Low A ein
    LOW_B_EIN; // Low B ein
    for(i=0; i<(TONDAUER / SOUND3_A); i++)
    {
        HIGH_C_EIN; // Test C
        Delay(SOUND_E);
		Strom = MessAD(2);
		
        PORTB = 0;
        Delay(SOUND2_A);
		
		if(Strom > CHECK_STROM_MIN) { MosfetOkay |= 0x04; } else { MosfetOkay &= ~0x04;};
    }
    FETS_OFF;
	
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+ Low-Mosfets auf Schalten testen
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    LOW_A_EIN; // Low A ein
    for(i=0; i< (TONDAUER / SOUND2_A) ; i++)
    {
        HIGH_B_EIN; // Test B
        Delay(SOUND_E);
		Strom = MessAD(0);

        PORTB = 0;
        Delay(SOUND2_A);
		
		if(Strom > CHECK_STROM_MAX) { MosfetOkay &= ~0x08;} else { MosfetOkay |= 0x08;};
    }
	FETS_OFF;
//++++++++++++++++++++++++++++++++++++
    LOW_C_EIN; // Low C ein
    for(i=0; i<(TONDAUER / SOUND1_A); i++)
    {
        HIGH_B_EIN; // Test B
        Delay(SOUND_E);
		Strom = MessAD(2);
        
			
        PORTB = 0;
        Delay(SOUND3_A);
		
		if(Strom > CHECK_STROM_MAX) { MosfetOkay &= ~0x20;} else { MosfetOkay |= 0x20;};
    }
	FETS_OFF;
//++++++++++++++++++++++++++++++++++++    
    LOW_B_EIN; // Low B ein
    for(i=0; i<(TONDAUER / SOUND3_A); i++)
    {
        HIGH_C_EIN; // Test C
        Delay(SOUND_E);
		Strom = MessAD(1);
        
        PORTB = 0;
        Delay(SOUND3_A);
		
		if(Strom > CHECK_STROM_MAX) { MosfetOkay &= ~0x10;} else { MosfetOkay |= 0x10;};
    }
    FETS_OFF;
//++++++++++++++++++++++++++++++++++++
//LOW_B_EIN;
//HIGH_A_EIN;
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    sei();//Globale Interrupts Einschalten

    Delay_ms(300 * (4-ADR_TAB[MotorAdresse]));    
    if(!(MosfetOkay & 0x01))  { anz = 1; UDR='A'; } else
    if(!(MosfetOkay & 0x02))  { anz = 2; UDR='B'; } else
    if(!(MosfetOkay & 0x04))  { anz = 3; UDR='C'; } else
    if(!(MosfetOkay & 0x08))  { anz = 4; UDR='a'; } else
    if(!(MosfetOkay & 0x10))  { anz = 5; UDR='b'; } else
    if(!(MosfetOkay & 0x20))  { anz = 6; UDR='c'; }
		
    if(anz) {while(1) {GrnBlink(anz);}}  // 若有场效应管无法导通，红灯闪烁
    //RotBlink(anz);
	//PORTC |= ROT;
    UART_Putchar('.');
}
//############################################################################
//
/*
Sollwert 在德语中是“标称值”的意思，Ermittlung是“查明”的意思。
合在一起，这个函数的功能就是查明外部到底给电调的是什么 PWM 值，查明这个值后，将其作为返回值返回。
这个值有可能是 I2C 给的，也有可能是串口给的，也有可能是 PPM 给的；查看的优先级为 I2C > 串口 > PPM 信号
*/
unsigned char GetPWM(void)
////############################################################################
{
    static unsigned int sollwert = 0;
    unsigned int ppm;
    if(!I2C_Timeout)   // 
    {
        if(UART_Timeout)  // 如果是 串口信号
        {
             sollwert =  (MAX_PWM * (unsigned int) UART_Norminal_Value) / 200;  // skalieren auf 0-200 = 0-255
             ICP_INT_DISABLE;
             PORTC &= ~ROT; // closed
        }
        else if(PPM_SIGNAL_TIME > 20)  // 收到ppm脉冲
        { /*MAX_PWM         255 MIN_PWM 3  ROT    0x08   */
           ppm = PPM_Signal;
           if(ppm > 300) ppm =   0;  // ungültiges Signal
           if(ppm > 200) ppm = 200;
           if(ppm <= MIN_PPM) sollwert = 0;
           else
               {
                  sollwert = (int) MIN_PWM + ((MAX_PWM - MIN_PWM) * (ppm - MIN_PPM)) / (190 - MIN_PPM);
               }
                PORTC &= ~ROT; // closed
        }
        else   // 
        {
           if(!TEST_SCHUB) { if(sollwert) sollwert--; }  
           PORTC |= ROT; // open
        }
    }
    else // 获取到 I2C 总线数据
    {

        sollwert = I2C_Receive_Buffer;
        PORTC &= ~ROT; // 关闭红灯
        ICP_INT_DISABLE; // 关闭 PPM 输入捕获中断
    }
    if(sollwert > MAX_PWM) sollwert = MAX_PWM;
    return(sollwert);
}

int main()
{
   char altPhase = 0;
   int test = 0;
   unsigned int Blink,TestschubTimer;
   unsigned int Blink2,MittelstromTimer,Test_Speed_Timer,MotorGestopptTimer;
   // 初始化单片机引脚
   DDRC   = 0x08; // PC3引脚输出
   PORTC  = 0x08; // PC3引脚输出 高电平
   DDRD   = 0x3A; // PD1,3,4,5 输出,PD3,4,5控制三相无刷电机
   PORTD  = 0x00; //无输出值
   DDRB   = 0x0E; // PB1,2,3 输出,控制三相无刷电机
   PORTB  = 0x31; // PB0,4,5引脚输入高电平（吸收电流） 0011 0001
   
#if (MOTORADRESSE == 0) // 未定义，根据跳线获取电调地址
    PORTB |= (ADR1 + ADR2);   // 1111 0001
    for(test=0;test<500;test++);
    if(PINB & ADR1)
         {
           if (PINB & ADR2) MotorAdresse = 1;
            else MotorAdresse = 2;
         }
     else
         {
           if (PINB & ADR2) MotorAdresse = 3;
            else MotorAdresse = 4;
         }
    HwVersion = 11; // 1.1版本电路
#else // 用于兼容 1.0 版本
    MotorAdresse  = MOTORADRESSE;
    HwVersion = 10; // 1.0版本
#endif

   if(PIND & 0x80) {HwVersion = 12; IntRef = 0xc0; } // 使用 v1.2 版本电路
   DDRD  = 0xBA; // 1011 1010；PD1,3,4,5,7 输出   
   // 管脚初始化、电调地址确认、硬件版本确认完毕，
   UART_Init(); // 初始化串口通信，UART是一种通用串行数据总线，用于异步通信。该总线双向通信，可以实现全双工传输和接收。
                //// 在嵌入式设计中，UART用于主机与辅助设备通信，如汽车音响与外接AP之间的通信
   Timer0_Init();
   ADC_Init();
   sei(); // 打开全局中断
   
   PWM_Init(); // 初始化 PWM
   
   InitIC2_Slave(0x50); // 初始化 TWI 总线通信
   InitPPM();

   Blink             = SetDelay(101);    // 0+t-1 设置延时时间
   Blink2            = SetDelay(102);
   MinUpmPulse       = SetDelay(103);
   MittelstromTimer  = SetDelay(254);
   Test_Speed_Timer = SetDelay(1005);
   TestschubTimer    = SetDelay(1006);
   int breakflag = 0;
   while(!CheckDelay(MinUpmPulse)) // 延时103ms
   {
        // 延时中的每次循环都会调用 GetPWM()函数，一旦发现有信号输入立刻停止延时，继续往下运行
		if(GetPWM()) break;
   }
   GRN_ON;
   //unsigned char anz = 3;
   //if(anz) {while(1) {GrnBlink(anz);}}
   PWM = 0;

   SetPWM();

   SFIOR = 0x08;  // analog comparator multiplexer enable模拟比较器多路复用启用
   ADMUX = 1; // 模拟信号由 adc1 输入

   MinUpmPulse = SetDelay(10);
   UART_Debug_Info.Analog[1] = 1;
   PPM_Signal = 0;
	
   if(!GetPWM()) MotorTon();
  
   PORTB = 0x31; // Pullups wieder einschalten

   if(TEST_MANUELL)    MotorStartAlgorithm(TEST_MANUELL);  // kommt von dort nicht wieder

   while (1)
        {
        if(!TEST_SCHUB)   PWM = GetPWM();        
        if(MANUELL_PWM)   PWM = MANUELL_PWM;
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(Phase != altPhase)   // es gab eine Kommutierung im Interrupt
            {
            MotorStops = 0;  // init 1
            CountTimes = 0;    // init 1
            MinUpmPulse = SetDelay(250);  // Timeout, falls ein Motor stehen bleibt
            altPhase = Phase;
            }
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        if(!PWM)    // Sollwert == 0
            {
            If_Get_PWM = 0;      // kein Startversuch init 0
            CountTimes = 0; // init 1
            
            if(CheckDelay(MotorGestopptTimer))
                {
                DISABLE_SENSE_INT;
                MotorStops = 1;  
                STEUER_OFF;
                }
            }
        else
            {
            if(MotorStops) If_Get_PWM = 1;        // Startversuch
            MotorGestopptTimer = SetDelay(1500);
            }
			
        if(MotorStops && !TEST_SCHUB) PWM = 0;
        SetPWM();
        // +++++++++++++++++++++++++++++++以下为第二部分++++++++++++++++++++++++++++++++++++++++
        if(!CountTimes++) // init 1
            {
            if(MotorStops)
                {
                ADC_FastConvert();
                }
            if(SIO_DEBUG)
                {
                DebugAusgaben();  // welche Werte sollen angezeigt werden?
                if(!UART_Send_Busy)  UART_SendUart();
                else UART_DisplayDebugInfo();
                }
            if(CheckDelay(MittelstromTimer))  
                {
                MittelstromTimer = SetDelay(50); // alle 50ms
                if(Mittelstrom <  Strom) Mittelstrom++;// Mittelwert des Stroms bilden
                else if(Mittelstrom >  Strom) Mittelstrom--;
                if(Strom > MAX_STROM) MaxPWM -= MaxPWM / 32;              
                if((Mittelstrom > LIMIT_STROM))// Strom am Limit?
                    {
                    if(MaxPWM) MaxPWM--;// dann die Maximale PWM herunterfahren
                    PORTC |= ROT;
                    }
                else
                    {
                    if(MaxPWM < MAX_PWM) MaxPWM++;
                    }
                }
            if(CheckDelay(Test_Speed_Timer))   // Ist-Drehzahl bestimmen
                {
                Test_Speed_Timer = SetDelay(10);
                Current_Speed2UART = Count_Phase_times;//(6 * Count_Phase_times) / (POLANZAHL / 2);
                Count_Phase_times = 0;
                IF_TEST_STROM = 1;
                }

#if TEST_SCHUB == 1
            {
            if(CheckDelay(TestschubTimer))
                {
                TestschubTimer = SetDelay(1500);
                    switch(test)
                        {
                        case 0: PWM = 50; test++; break;
                        case 1: PWM = 130; test++; break;
                        case 2: PWM = 60;  test++; break;
                        case 3: PWM = 140; test++; break;
                        case 4: PWM = 150; test = 0; break;
                        default: test = 0;
                        }
                }
            }
#endif

            if((CheckDelay(MinUpmPulse) && Current_Speed2UART == 0) || If_Get_PWM)
                {
                MotorStops = 1;
                DISABLE_SENSE_INT;
                MinUpmPulse = SetDelay(100);
                if(If_Get_PWM)
                    {
                    PORTC &= ~ROT; // closed
                    Strom_max = 0;
                    If_Get_PWM = 0;// reset start flag
                    if(MotorStartAlgorithm(10))
                        {
							GRN_ON;
                        MotorStops = 0;
                        Phase--;
                        PWM = 1;
                        SetPWM();
                        SENSE_TOGGLE_INT;
                        ENABLE_SENSE_INT;
                        MinUpmPulse = SetDelay(20);
                        while(!CheckDelay(MinUpmPulse)){}
                        PWM = 15;
                        SetPWM();
                        MinUpmPulse = SetDelay(300);
                        while(!CheckDelay(MinUpmPulse))
                            {
                            if(Strom > LIMIT_STROM/2)
                                {
                                STEUER_OFF;
                                RotBlink(10);
                                If_Get_PWM = 1;
                                }
                            }
                        Test_Speed_Timer = SetDelay(50);
                        altPhase = 7;
                        }
                    else if(GetPWM()) {If_Get_PWM = 1;}
                    }
                }
            }
        }
}