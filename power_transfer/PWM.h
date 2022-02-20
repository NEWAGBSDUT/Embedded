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
//ʹ OC1A ���ţ��ӳ�ЧӦ�� NC+����� PWM �ź�
//�� STEUER_C+��� PWM �źŵ�ͬʱ��STEUER_A+�� STEUER_B+���ص�
#define PWM_B_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x04;}
#define PWM_A_ON  {TCCR1A = 0xA1; TCCR2  = 0x61; DDRB = 0x08;}
/* ��ʼ��T1����ȽϼĴ��� OCR1A �� OCR1B���ȽϼĴ�������һ��16λ��ֵ���������������ֵ(TCNT1)���бȽϡ�
** ƥ�����������������Ƚ��жϣ�������OC1x�����ϲ����������,see "atmega8.pdf" on page 99
** 8λ�Ƚ��������ؽ�TCNT2������ȽϼĴ���(OCR2)���бȽ�
** ÿ��TCNT2����OCR2ʱ���Ƚ�������һ��ƥ���ź�,see "atmega8.pdf" on page 105
** TCCR1A ѡ��������ģ��Ϊ��Phase Correct PWM Mode��see "atmega8.pdf" on page 97, ���ֵΪ 255, PWM, Phase Correct, 8-bit
** TCCR2 ѡ��ͬ��
*/
#define PWM_OFF {OCR1A = 0; OCR1B = 0; OCR2 = 0;/* ����ȽϼĴ��� */TCCR1A = 0x01; TCCR2 = 0x41;/* ���ƼĴ���*/DDRB = 0x0E; PORTB &= ~0x0E;}
#endif

#define  STEUER_A_H {PWM_A_ON}
#define  STEUER_B_H {PWM_B_ON}
#define  STEUER_C_H {PWM_C_ON}
                    // �� A �±ۣ��ر� B��C �±�
#define  STEUER_A_L {PORTD &= ~0x30; PORTD |= 0x08;}
#define  STEUER_B_L {PORTD &= ~0x28; PORTD |= 0x10;}
#define  STEUER_C_L {PORTD &= ~0x18; PORTD |= 0x20;}
	
#define  STEUER_OFF {PORTD &= ~0x38; PWM_OFF; }
#define  FETS_OFF {PORTD &= ~0x38; PORTB &= ~0x0E; }

#define SENSE_A ADMUX = 0 | IntRef;//�� ADC0 �ܽŸ���Ϊģ��Ƚ����ķ��������
#define SENSE_B ADMUX = 1 | IntRef;
#define SENSE_C ADMUX = 2 | IntRef;

#define ClrSENSE            ACSR |= 0x10 //����ģ��Ƚ������жϱ�־
#define SENSE               ((ACSR & 0x10))
//�жϺ꣬��ģ��Ƚ����жϱ�־��λ����˱��ʽ��ֵΪ���㣬������ʽ��ֵΪ��
#define SENSE_L             (!(ACSR & 0x20))
#define SENSE_H             ((ACSR & 0x20))
//��Ҳ�Ǹ��жϺ굱 MITTEL ��ѹ���� NULL_X ʱ�����ʽ��ֵΪ��
#define ENABLE_SENSE_INT    {CompInterruptFreigabe = 1;ACSR |= 0x0A; }
	// ʹ��ģ��Ƚ����жϡ�
#define DISABLE_SENSE_INT   {CompInterruptFreigabe = 0; ACSR &= ~0x08; }
    // ����ģ��Ƚ����ж�

#define SENSE_FALLING_INT    ACSR &= ~0x01 // ����ģ��Ƚ���Ϊ�½��ز����ж�
#define SENSE_RISING_INT    ACSR |= 0x03 // ����ģ��Ƚ���Ϊ�����ز����ж�
#define SENSE_TOGGLE_INT    ACSR &= ~0x03 // ����ģ��Ƚ���Ϊ�κ�����仯���ɴ����ж�

#endif //BLMC_H_