/*
 * i2c.c
 *
 *  Created on: May 2, 2015
 *      Author: sid
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "i2c.h"


// I2C Linux device handle
int i2cFd[2] = {-1, -1};

//pthread_mutex_t lock_i2c[2] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

// open the Linux device
int i2cOpen(void)
{
	/* primary i2c */
	i2cFd[0] = open(I2C_DEV0, O_RDWR);
	if (i2cFd[0] < 0)
		return -1;

	/* secondary i2c */
	i2cFd[1] = open(I2C_DEV1, O_RDWR);
	if (i2cFd[1] < 0)
		return -1;

	return 0;
}

// close the Linux device
int i2cClose(void)
{
	close(i2cFd[0]);
	close(i2cFd[1]);

	return 0;
}


#if DEBUG
void debug_i2cmsg(struct i2c_msg msg)
{
	debug_print("[ %02x", msg.addr);
	fprintf(stderr, msg.flags==I2C_M_RD?"R ":"W ");
	for(int i=0; i<msg.len; i++)
	{
		fprintf(stderr, "%02x ",msg.buf[i]);
	}
	fprintf(stderr, "]\n");
}
void debug_i2cmsg_rx(struct i2c_msg msg, int ret)
{
	debug_print("[ %02x", msg.addr);
	fprintf(stderr, msg.flags==I2C_M_RD?"R ":"W ");
	for(int i=0; i<msg.len; i++)
	{
		fprintf(stderr, "%02x ",msg.buf[i]);
	}
	fprintf(stderr, "], %i\n", ret);
}
#endif

int i2cWriteRead(int bus, int address, unsigned char *txBuf, int txLen, unsigned char *rxBuf, int rxLen)
{
	int ret;

	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msgs[2];

	msgs[0].addr = (__u16)address;
	msgs[0].buf = txBuf;
	msgs[0].flags = 0;
	msgs[0].len = txLen;
	msgs[1].addr = (__u16)address;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = rxLen;
	msgs[1].buf = rxBuf;

	data.msgs = msgs;
	data.nmsgs = 2;

	#if DEBUG
	debug_i2cmsg(msgs[0]);
	#endif

	//pthread_mutex_lock(&lock_i2c[bus]);
	ret = ioctl(i2cFd[bus], I2C_RDWR, &data);
	//pthread_mutex_unlock(&lock_i2c[bus]);

	#if DEBUG
	debug_i2cmsg_rx(msgs[1], ret);
	#endif

	return ret;
}

int i2cWrite(int bus,int address, unsigned char *txBuf, int txLen)
{
	int ret;

	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msgs[1];

	msgs[0].addr = (__u16)address;
	msgs[0].buf = txBuf;
	msgs[0].flags = 0;
	msgs[0].len = txLen;

	data.msgs = msgs;
	data.nmsgs = 1;

	#if DEBUG
	debug_i2cmsg(msgs[0]);
	#endif

	//pthread_mutex_lock(&lock_i2c[bus]);
	ret = ioctl(i2cFd[bus], I2C_RDWR, &data);
	//pthread_mutex_unlock(&lock_i2c[bus]);

	#if DEBUG
	debug_print("ret=%i\n", ret);
	#endif

	return ret;
}

int i2cRead(int bus, int address, unsigned char *rxBuf, int rxLen)
{
	int ret;

	struct i2c_rdwr_ioctl_data data;
	struct i2c_msg msgs[1];

	msgs[0].addr = (__u16)address;
	msgs[0].buf = rxBuf;
	msgs[0].flags = I2C_M_RD;
	msgs[0].len = rxLen;

	data.msgs = msgs;
	data.nmsgs = 1;

	//pthread_mutex_lock(&lock_i2c[bus]);
	ret = ioctl(i2cFd[bus], I2C_RDWR, &data);
	//pthread_mutex_unlock(&lock_i2c[bus]);

	#if DEBUG
	debug_i2cmsg_rx(msgs[0], ret);
	#endif

	return ret;
}
