enum {
  STOP             = 0,
  CK               = 1,
  CK8              = 2,
  CK64             = 3,
  CK256            = 4,
  CK1024           = 5,
  T0_FALLING_EDGE  = 6,
  T0_RISING_EDGE   = 7
};


#if defined (__AVR_ATmega8__)
#define TIMER_TEILER          CK8
//#define TIMER_RELOAD_VALUE    125
#endif
#if defined (__AVR_ATmega8A__)
#define TIMER_TEILER          CK8
//#define TIMER_RELOAD_VALUE    125
#endif
// ��ʱ��0 ��ʼ��������Ϊ��ʱ��ʱ��ʹ��

extern volatile unsigned int  CountMilliseconds;
extern volatile unsigned char Timer0Overflow;


void         Timer0_Init (void);
// ���뼶��ʱ
void         Delay_ms (unsigned int);
// ������ʱʱ��
unsigned int SetDelay (unsigned int t);
// ���뼶��ʱ��װ
char         CheckDelay (unsigned int t);