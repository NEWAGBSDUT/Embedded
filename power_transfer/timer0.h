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
// 计时器0 初始化用来作为延时计时器使用

extern volatile unsigned int  CountMilliseconds;
extern volatile unsigned char Timer0Overflow;


void         Timer0_Init (void);
// 毫秒级延时
void         Delay_ms (unsigned int);
// 设置延时时间
unsigned int SetDelay (unsigned int t);
// 毫秒级延时封装
char         CheckDelay (unsigned int t);