/* SUSI Library
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 */

#include "susi.h"
#include <sys/io.h>

/* Globals */

extern int smbus_fd;
extern int kernel_fd;
extern int susi_err;

/* -------------------------- External API --------------------------------- */

/* Check if available */
u8 SusiPortIOAvailable(void)
{
	if (smbus_fd >= 0 && kernel_fd >= 0) 
		return 1;
	else {
		susi_err = -EAGAIN;
		return -1;
	}
}

/* Read byte from port */
s8 SusiPortIOGetByte(u16 port, u8 *data)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!data) {
		susi_err = -EINVAL;
		return 0;
	}

	*data = inb(port);
	return 1;
}

/* Read short from port */
s8 SusiPortIOGetWord(u16 port, u16 *data)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!data) {
		susi_err = -EINVAL;
		return 0;
	}

	*data = inw(port);
	return 1;
}

/* Read long from port */
s8 SusiPortIOGetLong(u16 port, u32 *data)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!data) {
		susi_err = -EINVAL;
		return 0;
	}

	*data = inl(port);
	return 1;
}

/* Out byte to port */
s8 SusiPortIOSetByte(u16 port, u8 data)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	outb(data, port);
	return 1;
}

/* Out word to port */
s8 SusiPortIOSetWord(u16 port, u16 data)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	outw(data, port);
	return 1;
}

/* Out long to port */
s8 SusiPortIOSetLong(u16 port, u32 data)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	outl(data, port);
	return 1;
}

