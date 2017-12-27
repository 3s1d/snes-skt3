/*
 * gpio.h
 *
 *  Created on: May 1, 2015
 *      Author: sid
 */

#ifndef GPIO_H_
#define GPIO_H_


#define FORECE_OFF		66	//104 s0dimm PIN, to disable rs232



#define DIRECTION_INPUT		"in\n"
#define DIRECTION_OUTPUT	"out\n"

#define HIGH			1
#define LOW			0

#define TRIGGER_EDGE_RISING	"rising"
#define TRIGGER_EDGE_FALLING	"falling"
#define TRIGGER_EDGE_BOTH	"both"
#define TRIGGER_EDGE_DISABLE	"none"

#define SYSFS_GPIO_DIR 		"/sys/class/gpio"

#define MAX_BUF 64

typedef void (gpioKeyCallback)(int state);

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, char *dir);
int gpio_set_value(unsigned int gpio, unsigned int value);
int gpio_get_value(unsigned int gpio, unsigned int *value);

int gpio_set_edge(unsigned int gpio, char *edge);
int gpio_fd_open(unsigned int gpio);
int gpio_fd_close(int fd);

#endif /* GPIO_H_ */
