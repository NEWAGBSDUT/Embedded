/*############################################################################
Slaveadr = 0x52 = Vorne, 0x54 = Hinten, 0x56 = Rechts, 0x58 = Links
############################################################################*/

#include <avr/io.h>
#include <util/twi.h>
#include "main.h"

unsigned char I2C_Receive_Buffer;
unsigned char Byte_Counter=0;

//############################################################################
//I2C (TWI) Interface Init
void InitIC2_Slave(uint8_t adr)
//############################################################################
{
    TWAR = adr + (2*MotorAdresse); // 设置 TWI单元 的 7 位从地址
	
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA); // 设置 TWI 控制寄存器 0100 0101
	       // 7、写入逻辑1 清除该位，用于启动TWI，注意每次清除前要完成上一个操作的所有操作再清除
		   // 2，启用TWI，0，打开中断，6、TWI启用确认
}

//############################################################################
// TWI 中断
// 只要设置了TWINT标志，SCL线就保持低电平。这允许应用软件在允许TWI传输继续之前完成它的任务
ISR (TWI_vect)// Table 68. Status Codes for Slave Receiver Mode
////############################################################################
{
    switch (TWSR & 0xF8)//Bits 7:3 – TWS: TWI Status
        {
        case SR_SLA_ACK:  
            TWCR |= (1<<TWINT);
            Byte_Counter=0;
            return;
        // Daten Empfangen
        case SR_PREV_ACK: // 获取到主机数据
            I2C_Receive_Buffer = TWDR;
            I2C_Timeout = 500;
            TWCR |= (1<<TWINT);
            return;
        // Daten Senden
        case SW_SLA_ACK:  
            if (Byte_Counter==0)
                {
                TWDR = Mittelstrom;
                Byte_Counter++;
                }
            else
                {
                TWDR = MaxPWM;
                }
            TWCR |= (1<<TWINT);
            return;
        // Daten Senden
        case SW_DATA_ACK:
            if (Byte_Counter==0)
                {
                TWDR = Mittelstrom;
                Byte_Counter++;
                }
            else
                {
                TWDR = MaxPWM;
                }
            TWCR |= (1<<TWINT);
            return;
        // Bus-Fehler zurücksetzen
        case TWI_BUS_ERR_2:
            TWCR |=(1<<TWSTO) | (1<<TWINT);
        // Bus-Fehler zurücksetzen  
        case TWI_BUS_ERR_1:
            TWCR |=(1<<TWSTO) | (1<<TWINT);
        }
    TWCR =(1<<TWEA) | (1<<TWINT) | (1<<TWEN) | (1<<TWIE); // TWI Reset
}