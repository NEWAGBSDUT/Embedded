/*############################################################################
############################################################################*/
#ifndef BLMC_H_
#define BLMC_H_

extern volatile unsigned char Phase;
extern volatile unsigned char ShadowTCCR1A;
extern volatile unsigned char CompInterruptFreigabe;

extern void PWM_Init(void);
extern void SetPWM(void);
void Manuell(void);

#ifdef  _32KHZ
 #define PWM_C_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0A;}
 #define PWM_B_ON  {TCCR1A = 0xAD; TCCR2  = 0x49;DDRB = 0x0C;}
 #define PWM_A_ON  {TCCR1A = 0xAD; TCCR2  = 0x69;DDRB = 0x08;}
 #define PWM_OFF   {TCCR1A = 0x0D; TCCR2  = 0x49;PORTC &= ~0x0E;}
#endif

#ifdef  _16KHZ    // 1010 0001     0110 0001     0000 0010
#define PWM_C_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x02;}
//使 OC1A 引脚（接场效应管 NC+）输出 PWM 信号
//在 STEUER_C+输出 PWM 信号的同时将STEUER_A+和 STEUER_B+都关掉
#define PWM_B_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x04;}
#define PWM_A_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x08;}
/* 初始化T1输出比较寄存器 OCR1A 和 OCR1B；比较寄存器包含一个16位的值，连续地与计数器值(TCNT1)进行比较。
** 匹配可以用来产生输出比较中断，或者在OC1x引脚上产生输出波形,see "atmega8.pdf" on page 99
** 8位比较器连续地将TCNT2与输出比较寄存器(OCR2)进行比较
** 每当TCNT2等于OCR2时，比较器发出一个匹配信号,see "atmega8.pdf" on page 105
** TCCR1A 选择波形生成模型为：Phase Correct PWM Mode，see "atmega8.pdf" on page 97, 最大值为 255, PWM, Phase Correct, 8-bit
** TCCR2 选择同上
*/
#define PWM_OFF {OCR1A = 0; OCR1B = 0; OCR2 = 0;/* 输出比较寄存器 */TCCR1A = 0x01; TCCR2 = 0x41;/* 控制寄存器*/DDRB = 0x0E; PORTB &= ~0x0E;}
#endif

#define  STEUER_A_H {PWM_A_ON}
#define  STEUER_B_H {PWM_B_ON}
#define  STEUER_C_H {PWM_C_ON}
                    // 打开 A 下臂，关闭 B和C 下臂
#define  STEUER_A_L {PORTD &= ~0x30; PORTD |= 0x08;}
#define  STEUER_B_L {PORTD &= ~0x28; PORTD |= 0x10;}
#define  STEUER_C_L {PORTD &= ~0x18; PORTD |= 0x20;}
	
#define  STEUER_OFF {PORTD &= ~0x38; PWM_OFF; }
#define  FETS_OFF {PORTD &= ~0x38; PORTB &= ~0x0E; }

#define SENSE_A ADMUX = 0 | IntRef;//将 ADC0 管脚复用为模拟比较器的反向输入端
#define SENSE_B ADMUX = 1 | IntRef;
#define SENSE_C ADMUX = 2 | IntRef;

#define ClrSENSE            ACSR |= 0x10 //清零模拟比较器的中断标志
#define SENSE               ((ACSR & 0x10))
//判断宏，若模拟比较器中断标志置位，则此表达式的值为非零，否则表达式的值为零
#define SENSE_L             (!(ACSR & 0x20))
#define SENSE_H             ((ACSR & 0x20))
//这也是个判断宏当 MITTEL 电压高于 NULL_X 时，表达式的值为真
#define ENABLE_SENSE_INT    {CompInterruptFreigabe = 1;ACSR |= 0x0A; }
	// 使能模拟比较器中断。
#define DISABLE_SENSE_INT   {CompInterruptFreigabe = 0; ACSR &= ~0x08; }
    // 禁能模拟比较器中断

#define SENSE_FALLING_INT    ACSR &= ~0x01 // 设置模拟比较器为下降沿产生中断
#define SENSE_RISING_INT    ACSR |= 0x03 // 设置模拟比较器为上升沿产生中断
#define SENSE_TOGGLE_INT    ACSR &= ~0x03 // 设置模拟比较器为任何输出变化都可触发中断

#endif //BLMC_H_