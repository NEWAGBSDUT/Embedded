/*############################################################################

############################################################################*/

#include "main.h"

//############################################################################
//Init ADC
void ADC_Init(void)
//############################################################################
{
  ADCSRA = 0xA6;  // 1010 0110//7、打开adc转换5、进入自由运行模式，持续采样并更新数据寄存器，由上升沿自动触发adc转换，清零停止自动转换
  ADMUX = 7 | IntRef; // 若是1.2版本电路IntRef = 0xc0；1100 0111 7:6使用内部2.56V作为参考电压，AREF引脚接外部电容 //1.1IntRef = 0；0000 0111使用AREF外接电压为参考电压，外部输入模拟信号连接ADC7引脚
  ADCSRA |= 0x40; // 1110 0110；6、启动adc转换，并开始第一个25clock转换（初始化转换）
}

//############################################################################
//Strom Analogwerte lesen
void ADC_Convert(void)
//############################################################################
{
 unsigned int i=0;
 unsigned char sense;
  sense = ADMUX;   // 保存现场
  ADMUX  =  0x06 | IntRef;  // 测试 ADC6 引脚电流值
  SFIOR  =  0x00;  // Analog Comperator aus
  ADCSRA =  0xD3;  // Converter ein, single
  ADCSRA |= 0x10;  // Ready l枚schen
  ADMUX  =  0x06 | IntRef;  // Kanal 6
  ADCSRA |= 0x40;  // Start
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   // zur眉ck auf den Sense-Kanal
  i = ADCW * 4;
//  if(i > 300) i = 300;
  Strom = (i + Strom * 7) / 8;
  if (Strom_max < Strom) Strom_max = Strom;
  ADCSRA = 0x00;  
  SFIOR = 0x08;  // Analog Comperator ein
}
//############################################################################
//Strom Analogwerte lesen
unsigned int MessAD(unsigned char channel)
//############################################################################
{
    unsigned char sense;
    sense    = ADMUX;   // 保存当前值
    channel |= IntRef; // 1.2版本IntRef = 0xc0；1100 0000//1.1 0000 0000
    ADMUX    =  channel;  // 使用内部2.56v参考电压，测试 ADC6 引脚电流值
    SFIOR    =  0x00;  // 禁止模拟比较器相关权限
    ADCSRA   =  0xD3;  // 1101 0011 使能ADC并开始ADC转换，并手动清除ADIF位，清理之前所有转换	
    ADCSRA  |=  0x10;  // 1100 0011 重置ADIF
	
    ADMUX    =  channel;  // 
    ADCSRA  |=  0x40;  // 开始转换
	
    while (((ADCSRA & 0x10) == 0));// 等待转换完成
    ADMUX  = sense;   // 恢复现场值
    ADCSRA = 0x00;  
    SFIOR  = 0x08;  // 恢复模拟比较器功能权限
    return(ADCW);
}
//############################################################################
//Strom Analogwerte lesen
void ADC_FastConvert(void)
//############################################################################
{
    unsigned int i=0;
    i = MessAD(6) * 4;

    if(i > 200) i = 200;
    Strom = i;//(i + Strom * 1) / 2;
    if (Strom_max < Strom) Strom_max = Strom;
    ADCSRA = 0x00;
    SFIOR = 0x08;  // Analog Comperator ein
}