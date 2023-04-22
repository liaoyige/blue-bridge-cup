#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "stm32g4xx_hal.h"
#include "main.h"

void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);
void EEPROM_Write(uint8_t address, uint8_t Info);
uint8_t EEPROM_Read(uint8_t address);

#endif
