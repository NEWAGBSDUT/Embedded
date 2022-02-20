/*############################################################################

############################################################################*/

#include "main.h"

//############################################################################
//Init ADC
void ADC_Init(void)
//############################################################################
{
  ADCSRA = 0xA6;  // 1010 0110//7����adcת��5��������������ģʽ�������������������ݼĴ��������������Զ�����adcת��������ֹͣ�Զ�ת��
  ADMUX = 7 | IntRef; // ����1.2�汾��·IntRef = 0xc0��1100 0111 7:6ʹ���ڲ�2.56V��Ϊ�ο���ѹ��AREF���Ž��ⲿ���� //1.1IntRef = 0��0000 0111ʹ��AREF��ӵ�ѹΪ�ο���ѹ���ⲿ����ģ���ź�����ADC7����
  ADCSRA |= 0x40; // 1110 0110��6������adcת��������ʼ��һ��25clockת������ʼ��ת����
}

//############################################################################
//Strom Analogwerte lesen
void ADC_Convert(void)
//############################################################################
{
 unsigned int i=0;
 unsigned char sense;
  sense = ADMUX;   // �����ֳ�
  ADMUX  =  0x06 | IntRef;  // ���� ADC6 ���ŵ���ֵ
  SFIOR  =  0x00;  // Analog Comperator aus
  ADCSRA =  0xD3;  // Converter ein, single
  ADCSRA |= 0x10;  // Ready löschen
  ADMUX  =  0x06 | IntRef;  // Kanal 6
  ADCSRA |= 0x40;  // Start
  while (((ADCSRA & 0x10) == 0));
  ADMUX = sense;   // zurück auf den Sense-Kanal
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
    sense    = ADMUX;   // ���浱ǰֵ
    channel |= IntRef; // 1.2�汾IntRef = 0xc0��1100 0000//1.1 0000 0000
    ADMUX    =  channel;  // ʹ���ڲ�2.56v�ο���ѹ������ ADC6 ���ŵ���ֵ
    SFIOR    =  0x00;  // ��ֹģ��Ƚ������Ȩ��
    ADCSRA   =  0xD3;  // 1101 0011 ʹ��ADC����ʼADCת�������ֶ����ADIFλ������֮ǰ����ת��	
    ADCSRA  |=  0x10;  // 1100 0011 ����ADIF
	
    ADMUX    =  channel;  // 
    ADCSRA  |=  0x40;  // ��ʼת��
	
    while (((ADCSRA & 0x10) == 0));// �ȴ�ת�����
    ADMUX  = sense;   // �ָ��ֳ�ֵ
    ADCSRA = 0x00;  
    SFIOR  = 0x08;  // �ָ�ģ��Ƚ�������Ȩ��
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