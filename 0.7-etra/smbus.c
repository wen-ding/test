/* SUSI Library
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 */

#include <linux/ioctl.h>
#include "i2c-dev.h"
#include "susi.h"

/* Globals */

extern int smbus_fd;
extern int kernel_fd;
extern int susi_err;

/* -------------------------- External API --------------------------------- */

/* Check if SMBus is available */
u8 SusiSMBusAvailable(void)
{
	if (smbus_fd >= 0 && kernel_fd >= 0) 
		return 1;
	else {
		susi_err = -EAGAIN;
		return -1;
	}
}

/* Quick Write */
s8 SusiSMBusWriteQuick(u8 address)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	u8 bval = address & 0x01;

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Quick cmd */
	susi_err = i2c_smbus_write_quick(smbus_fd, bval);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	return susi_err >= 0 ? 1 : 0;
}

/* Quick Read */
s8 SusiSMBusReadQuick(u8 address)
{
	return SusiSMBusWriteQuick(address);
}

/* Receive Byte */
s8 SusiSMBusReceiveByte(u8 address, u8 *value)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!value) {
		susi_err = -EINVAL;
		return 0;
	}

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Read byte */
	susi_err = i2c_smbus_read_byte(smbus_fd);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	if (susi_err >= 0)
		*value = (u8)susi_err;

	return susi_err >= 0 ? 1 : 0;
}

/* Send Byte */
s8 SusiSMBusSendByte(u8 address, u8 value)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Write byte */
	susi_err = i2c_smbus_write_byte(smbus_fd, value);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	return susi_err >= 0 ? 1 : 0;
}

/* Read Byte */
s8 SusiSMBusReadByte(u8 address, u8 offset, u8 *value)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!value) {
		susi_err = -EINVAL;
		return 0;
	}

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Read byte data */
	susi_err = i2c_smbus_read_byte_data(smbus_fd, offset);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	if (susi_err >= 0)
		*value = (u8)susi_err;

	return susi_err >= 0 ? 1 : 0;
}

/* Write Byte */
s8 SusiSMBusWriteByte(u8 address, u8 offset, u8 value)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Write byte data */
	susi_err = i2c_smbus_write_byte_data(smbus_fd, offset, value);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	return susi_err >= 0 ? 1 : 0;
}

/* Read Word */
s8 SusiSMBusReadWord(u8 address, u8 offset, u16 *value)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!value) {
		susi_err = -EINVAL;
		return 0;
	}

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Read word data */
	susi_err = i2c_smbus_read_word_data(smbus_fd, offset);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	if (susi_err >= 0)
		*value = (u16)susi_err;

	return susi_err >= 0 ? 1 : 0;
}

/* Write Word */
s8 SusiSMBusWriteWord(u8 address, u8 offset, u16 value)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	debug("%s: Setting slave address: 0x%x\n", __FUNC__, address);

	/* Set device address */
	if ((susi_err = ioctl(smbus_fd, I2C_SLAVE, address >> 1)) < 0)
		return 0;

	/* Write word data */
	susi_err = i2c_smbus_write_word_data(smbus_fd, offset, value);

	if (susi_err < 0)
		susi_err = -errno;

	debug("%s: Returned %d\n", __FUNC__, susi_err);

	return susi_err >= 0 ? 1 : 0;
}

/* Check Address */
s8 SusiSMBusScanDevice(u8 address)
{
	susi_err = -ENODEV;
	return 0;	
}
