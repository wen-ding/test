/* SUSI Library
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 */

#include "susi.h"
#include <fcntl.h>
#include <pthread.h>
#include <sys/io.h>

#define DEV_FILE		"/dev/bsp"
#define SMBUS_FILE		"/dev/i2c-0"

#define GPIO_USB		0xA

extern s8 __set_gpio_direction(u8 pin, u8 dir);
extern s8 __write_gpio(u8 pin, u8 status);

/* Globals */

int kernel_fd = -1;
int smbus_fd = -1;
int susi_err = 0;

/* -------------------------- External API --------------------------------- */

/* Get Version */
void SusiGetVersion(u16 *major, u16 *minor)
{
	if (major)
		*major = SUSI_LIB_VER_MJ;
	if (minor)
		*minor = SUSI_LIB_VER_MR;
}

/* Initialization */
s8 SusiInit(void)
{
	susi_err = 0;

	if (kernel_fd >= 0 || smbus_fd >= 0) {
		susi_err = -EEXIST;
		return 0;
	}

	/* Open kernel helper */
	if ((kernel_fd = open(DEV_FILE, O_RDWR)) < 0) {
		susi_err = -errno;
		return 0;
	}

	/* Open SMBus adapter */
	if ((smbus_fd = open(SMBUS_FILE, O_RDONLY)) < 0) {
		susi_err = -errno;
		return 0;
	}

	/* Request I/O Privileges */
	if (iopl(3) < 0) {
		SusiUnInit();	
		susi_err = -errno;
		return 0;
	}

	return 1;
}

/* De-init */
s8 SusiUnInit(void)
{
	close(kernel_fd);
	close(smbus_fd);

	kernel_fd = -1;
	smbus_fd = -1;
	susi_err = 0;

	return 1;
}

/* Last Error */
s32 SusiGetLastError(void)
{
	return susi_err;
}

/* Misc API */
s8 SusiUSBHubCtrl(u8 enable)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (enable != 0 && enable != 1) {
		susi_err = -EINVAL;
		return 0;
	}

	if (enable) {
		if (__set_gpio_direction(GPIO_USB, GPIO_OUTPUT) < 0)
			return 0;

		if (__write_gpio(GPIO_USB, GPIO_HIGH) < 0)
			return 0;
	} else {
		if (__write_gpio(GPIO_USB, GPIO_LOW) < 0)
			return 0;

		if (__set_gpio_direction(GPIO_USB, GPIO_INPUT) < 0)
			return 0;
	}

	return 1;
}

/* ------------------------ Unsupported API -------------------------------- */

s8 SusiVCAvailable(void)
{
	susi_err = -ENODEV;
	return -1;
}

s32 SusiIICAvailable(void)
{
	susi_err = -ENODEV;
	return -1;
}

s32 SusiCoreAvailable(void)
{
	susi_err = -ENODEV;
	return -1;
}

