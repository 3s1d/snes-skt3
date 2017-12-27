/*
 * i2c.h
 *
 *  Created on: May 2, 2015
 *      Author: sid
 */

#ifndef I2C_H_
#define I2C_H_

#define I2C_DEV0			"/dev/i2c-0"
#define I2C_DEV1			"/dev/i2c-1"

#define I2C0				0
#define I2C1				1

/*todo wrong file */
#define I2C_ADDR_PWRMNG			0x03
#define I2C_ADDR_PWRMNG_BOOTL		0x04

#define I2C_CMD_REBOOT			3

#define I2C_CMD_UPDATE_SD_ALL		4
#define I2C_CMD_UPDATE_SD_ROOTFS	5
#define I2C_CMD_UPDATE_SD_UBOOT		6

#define I2C_CMD_UPDATE_USB_ALL		7
#define I2C_CMD_UPDATE_USB_ROOTFS	8
#define I2C_CMD_UPDATE_USB_UBOOT	9

int i2cOpen(void);
int i2cClose(void);

int i2cWriteRead(int bus, int address, unsigned char *txBuf, int txLen, unsigned char *rxBuf, int rxLen);

int i2cWrite(int bus, int address, unsigned char *txBuf, int txLen);

int i2cRead(int bus, int address, unsigned char *rxBuf, int rxLen);

#endif /* I2C_H_ */
