/* SUSI Library - GPIO Support
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 * Written specifically for TREK-550. Platform checking
 * and corresponding changes are to be added if support
 * for more platforms are needed.
 */

#include "susi.h"

#define MAX_GPIOS	20

#define MAX_USER_DIS	4
#define MAX_USER_DOS	4
#define MAX_USER_GPIOS	8

#define GPIO1X_MIN	10
#define GPIO1X_MAX	18
#define GPIO2X_MIN	20
#define GPIO2X_MAX	28
#define GPIO3X_MIN	30
#define GPIO3X_MAX	34

#define F75111_ADDR	0x9C	/* F75111 SMBus Address		*/
#define F75111_REG03	0x03	/* Config and Function Select 	*/
#define F75111_REG10	0x10	/* GPIO1x Output Control Reg 	*/
#define F75111_REG11	0x11	/* GPIO1x Output Data Reg 	*/
#define F75111_REG12	0x12	/* GPIO1x Input Data Reg 	*/
#define F75111_REG1B	0x1B	/* GPIO1x Output Driving Enable */
#define F75111_REG20	0x20	/* GPIO2x Output Control Reg 	*/
#define F75111_REG21	0x21	/* GPIO2x Output Data Reg 	*/
#define F75111_REG22	0x22	/* GPIO2x Input Data Reg 	*/
#define F75111_REG2B	0x2B	/* GPIO2x Output Driving Enable */
#define F75111_REG40	0x40	/* GPIO3x Output Control Reg 	*/
#define F75111_REG41	0x41	/* GPIO3x Output Data Reg 	*/
#define F75111_REG42	0x42	/* GPIO3x Input Data Reg 	*/

/* GPIOs available to user */

static u8 gpios [8] = {16, 17, 20, 21, 25, 26, 27, 15};

/* Globals */

extern int smbus_fd;
extern int kernel_fd;
extern int susi_err;

/* -------------------------- Internal API --------------------------------- */

/* Count ones in a byte - (Internal) */
static inline u8 __c1s(u8 byte)
{
	u8 count = 0;

	for (; byte; count++)
		byte &= byte - 1;

	return count;
}

/* Read GPIO Control Regs - (Internal) */
static s8 __read_gpio_ctrls(u8 *gpio10, u8 *gpio20, u8 *gpio30)
{

	/* Read GPIO1x Output Control Reg */
	if (SusiSMBusReadByte(F75111_ADDR, F75111_REG10, gpio10) < 0)
		return -1;

	/* Read GPIO2x Output Control Reg */
	if (SusiSMBusReadByte(F75111_ADDR, F75111_REG20, gpio20) < 0)
		return -1;

	/* Read GPIO3x Output Control Reg */
	if (SusiSMBusReadByte(F75111_ADDR, F75111_REG40, gpio30) < 0)
		return -1;

	return 0;
}

/* Count GPIOs - (Internal) */
static s8 __cnt_gpios(u32 *incnt, u32 *outcnt)
{
	u8 gpio10 = 0, gpio20 = 0, gpio30 = 0;

	if (smbus_fd < 0 || !incnt || !outcnt)
		return -1;

	if (__read_gpio_ctrls(&gpio10, &gpio20, &gpio30) < 0)
		return -1;

	*outcnt = __c1s(gpio10) + __c1s(gpio20) + __c1s(gpio30);
	*incnt = MAX_GPIOS - *outcnt;

	return 0;
}

/* Set GPIO direction - (Internal) */
s8 __set_gpio_direction(u8 pin, u8 dir)
{
	u8 gpio = 0;

	if (smbus_fd < 0 || kernel_fd < 0 || pin < GPIO1X_MIN ||
	    pin > GPIO3X_MAX - 1 || (dir != 0 && dir != 1))
		return -EINVAL;

	debug("%s: Set pin %d to %d\n", __FUNC__, pin, dir);

	/* GPIO1x range */
	if (pin < GPIO1X_MAX) {

		pin -= GPIO1X_MIN;

		/* Check GPIO12 special case */
		if (pin == 2) {
			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG03, &gpio) < 0)
				return -1;

			if ((gpio & 0x18) >> 2)
				return -EINVAL;
		}

		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG10, &gpio) < 0)
			return -1;

		if (dir)
			gpio &= ~(1 << pin);
		else
			gpio |= (1 << pin);

		if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG10, gpio) < 0)
			return -1;

	/* GPIO2x range */
	} else if (pin >= GPIO2X_MIN && pin < GPIO2X_MAX) {
		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG20, &gpio) < 0)
			return -1;

		pin -= GPIO2X_MIN;

		if (dir)
			gpio &= ~(1 << pin);
		else
			gpio |= (1 << pin);

		if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG20, gpio) < 0)
			return -1;

	/* GPIO3x range */
	} else if (pin >= GPIO3X_MIN && pin < GPIO3X_MAX) {
		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG40, &gpio) < 0)
			return -1;

		pin -= GPIO3X_MIN;

		if (dir)
			gpio &= ~(1 << pin);
		else
			gpio |= (1 << pin);

		if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG40, gpio) < 0)
			return -1;
	} else 
		return -EINVAL;

	return 0;
}

/* Read GPIO Status - (Internal) */
static s8 __read_gpio(u8 pin, u8 *status)
{
	u8 gpio = 0;

	if (smbus_fd < 0 || kernel_fd < 0 || !status || pin < GPIO1X_MIN || 
	    pin > GPIO3X_MAX - 1)
		return -EINVAL;

	/* GPIO1x range */
	if (pin < GPIO1X_MAX) {

		pin -= GPIO1X_MIN;

		/* Check GPIO12 special case */
		if (pin == 2) {
			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG03, &gpio) < 0)
				return -1;

			if ((gpio & 0x18) >> 2)
				return -EINVAL;
		}

		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG10, &gpio) < 0)
			return -1;

		if (gpio & (1 << pin)) {
			/* Output */

			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG11, &gpio) < 0)
				return -1;

			*status = gpio & (1 << pin);
		} else {
			/* Input */

			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG12, &gpio) < 0)
				return -1;

			*status = gpio & (1 << pin);
		}

	/* GPIO2x range */
	} else if (pin >= GPIO2X_MIN && pin < GPIO2X_MAX) {

		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG20, &gpio) < 0)
			return -1;

		pin -= GPIO2X_MIN;

		if (gpio & (1 << pin)) {
			/* Output */

			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG21, &gpio) < 0)
				return -1;

			*status = gpio & (1 << pin);
		} else {
			/* Input */

			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG22, &gpio) < 0)
				return -1;

			*status = gpio & (1 << pin);
		}

	/* GPIO3x range */
	} else if (pin >= GPIO3X_MIN && pin < GPIO3X_MAX) {
		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG40, &gpio) < 0)
			return -1;

		pin -= GPIO3X_MIN;

		if (gpio & (1 << pin)) {
			/* Output */

			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG41, &gpio) < 0)
				return -1;

			*status = gpio & (1 << pin);
		} else {
			/* Input */

			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG42, &gpio) < 0)
				return -1;

			*status = gpio & (1 << pin);
		}
	} else 
		return -EINVAL;

	return 0;
}

/* Write GPIO status - (Internal) */
s8 __write_gpio(u8 pin, u8 status)
{
	u8 gpio = 0;

	if (smbus_fd < 0 || kernel_fd < 0 || pin < GPIO1X_MIN || 
	    pin > GPIO3X_MAX - 1 || (status != 0 && status != 1))
		return -EINVAL;

	debug("%s: Set pin %d to %d\n", __FUNC__, pin, status);

	/* GPIO1x range */
	if (pin < GPIO1X_MAX) {

		pin -= GPIO1X_MIN;

		/* Check GPIO12 special case */
		if (pin == 2) {
			if (SusiSMBusReadByte(F75111_ADDR, F75111_REG03, &gpio) < 0)
				return -1;

			if ((gpio & 0x18) >> 2)
				return -EINVAL;
		}

		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG10, &gpio) < 0)
			return -1;

		if (gpio & (1 << pin)) {
			/* Output */

			if (status) {
				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG11, &gpio) < 0)
					return -1;

				gpio |= (1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG11, gpio) < 0)
					return -1;

				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG1B, &gpio) < 0)
					return -1;

				gpio |= (1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG1B, gpio) < 0)
					return -1;
			} else {
				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG11, &gpio) < 0)
					return -1;

				gpio &= ~(1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG11, gpio) < 0)
					return -1;

				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG1B, &gpio) < 0)
					return -1;

				gpio &= ~(1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG1B, gpio) < 0)
					return -1;
			}
		} else
			return -EINVAL;

	/* GPIO2x range */
	} else if (pin >= GPIO2X_MIN && pin < GPIO2X_MAX) {

		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG20, &gpio) < 0)
			return -1;

		pin -= GPIO2X_MIN;

		if (gpio & (1 << pin)) {
			/* Output */

			if (status) {
				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG21, &gpio) < 0)
					return -1;

				gpio |= (1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG21, gpio) < 0)
					return -1;

				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG2B, &gpio) < 0)
					return -1;

				gpio |= (1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG2B, gpio) < 0)
					return -1;
			} else {
				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG21, &gpio) < 0)
					return -1;

				gpio &= ~(1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG21, gpio) < 0)
					return -1;

				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG2B, &gpio) < 0)
					return -1;

				gpio &= ~(1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG2B, gpio) < 0)
					return -1;
			}
		} else
			return -EINVAL;

	/* GPIO3x range */
	} else if (pin >= GPIO3X_MIN && pin < GPIO3X_MAX) {
		if (SusiSMBusReadByte(F75111_ADDR, F75111_REG40, &gpio) < 0)
			return -1;

		pin -= GPIO3X_MIN;

		if (gpio & (1 << pin)) {
			/* Output */

			if (status) {
				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG41, &gpio) < 0)
					return -1;

				gpio |= (1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG41, gpio) < 0)
					return -1;
			} else {
				if (SusiSMBusReadByte(F75111_ADDR, F75111_REG41, &gpio) < 0)
					return -1;

				gpio &= ~(1 << pin);

				if (SusiSMBusWriteByte(F75111_ADDR, F75111_REG41, gpio) < 0)
					return -1;
			}
		} else
			return -EINVAL;
	} else
		return -EINVAL;

	return 0;
}

/* -------------------------- External API --------------------------------- */

/* Check if GPIO is available */
u8 SusiIOAvailable(void)
{
	if (smbus_fd >= 0 && kernel_fd >= 0) 
		return 1;
	else {
		susi_err = -EAGAIN;
		return -1;
	}
}

/* Count GPIOs */
s8 SusiIOCountEx(u32 *incnt, u32 *outcnt)
{
	if (smbus_fd < 0 || kernel_fd < 0) { 
		susi_err = -EAGAIN;
		return 0;
	}

	if (!incnt || !outcnt) {
		susi_err = -EINVAL;
		return 0;
	}

	/* 4 Input / Output GPIOS */
	*incnt = *outcnt = 4;

	return 1;
}

/* Query various masks */
s8 SusiIOQueryMask(u32 flag, u32 *mask)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!mask) {
		susi_err = -EINVAL;
		return 0;
	}

	/* Check mask requested */
	switch (flag) {
		case ESIO_SMASK_PIN_FULL:
			*mask = 0xFF;
			break;
		case ESIO_SMASK_CONFIGURABLE:
			*mask = 0;
			break;
		case ESIO_DMASK_DIRECTION:
			*mask = 0xF; 
			break; 
		default:
			susi_err = -EINVAL;
			return 0;
	}

	return 1;
}

/* Set GPIO direction */
s8 SusiIOSetDirection(u8 pin, u8 dir, u32 *pinmask)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (pin < 0 || pin > MAX_USER_GPIOS - 1 || (dir != 0 && dir != 1)) {
		susi_err = -EINVAL;
		return 0;
	}

	debug("%s: Set pin %d to %d\n", __FUNC__, pin, dir);

	switch (pin) {
		case 0:
		case 1:
		case 2:
		case 3:
			if (!dir) {
				susi_err = -EINVAL;
				return 0;
			}
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			if (dir) {
				susi_err = -EINVAL;
				return 0;
			}
			break;
		default:
			susi_err = -EINVAL;
			return 0;
	}

	/* Return mask */
	if (pinmask)
		if (!SusiIOQueryMask(ESIO_DMASK_DIRECTION, pinmask))
			return 0;

	return 1;
}

/* Set multiple GPIO directions */
s8 SusiIOSetDirectionMulti(u32 targetmask, u32 *pinmask)
{
	u8 i = 0;

	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!pinmask) {
		susi_err = -EINVAL;
		return 0;
	}

	/* Run through mask */
	for (; i < MAX_USER_GPIOS; i++)
		if (targetmask & (1 << i))
			if (!SusiIOSetDirection(i, 
			   ((*pinmask & (1 << i)) == (1 << i)), NULL))
				return 0;

	/* Return mask */
	if (!SusiIOQueryMask(ESIO_DMASK_DIRECTION, pinmask))
		return 0;

	return 1;
}


/* Read GPIO Status */
s8 SusiIOReadEx(u8 pin, u8 *status)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!status || pin < 0 || pin > MAX_USER_GPIOS - 1) {
		susi_err = -EINVAL;
		return 0;
	}

	susi_err = __read_gpio(gpios [pin], status);

	return (susi_err >= 0) ? 1 : 0;
}

/* Read multiple GPIOs */
s8 SusiIOReadMultiEx(u32 targetmask, u32 *statusmask)
{
	u8 i = 0, status = 0;

	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!statusmask) {
		susi_err = -EINVAL;
		return 0;
	}

	*statusmask = 0;

	/* Run through mask */
	for (; i < MAX_USER_GPIOS; i++)
		if (targetmask & (1 << i)) {
			if (!SusiIOReadEx(i, &status))
				return 0;
			
			if (status)
				*statusmask |= (1 << i);
		}

	return 1; 
}

/* Write GPIO Status */
s8 SusiIOWriteEx(u8 pin, u8 status)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!status || pin < MAX_USER_DOS || pin > MAX_USER_GPIOS - 1) {
		susi_err = -EINVAL;
		return 0;
	}

	susi_err = __write_gpio(gpios [pin], status);

	return (susi_err >= 0) ? 1 : 0;
}

/* Write multiple GPIOs */
s8 SusiIOWriteMultiEx(u32 targetmask, u32 statusmask)
{
	u8 i = 0;

	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	/* Run through mask */
	for (; i < MAX_USER_GPIOS; i++)
		if (targetmask & (1 << i))
			if (!SusiIOWriteEx(i, (statusmask & (1 << i))))
				return 0;

	return 1;
}
