/* SUSI Library
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 */

#include "susi.h"
#include <stdlib.h>

#define EC_PMC2_CMD			0x6C
#define EC_PMC2_DAT			0x68

/* Temp / Volt cmds */
#define EC_PMC2_CMD_TSYS		0xD9
#define EC_PMC2_CMD_TCPU_FLT		0xD7
#define EC_PMC2_CMD_TCPU_INT		0xD6
#define EC_PMC2_CMD_VCORE_FLT		0xD5
#define EC_PMC2_CMD_VCORE_INT		0xD4
#define EC_PMC2_CMD_V50_FLT		0xD3
#define EC_PMC2_CMD_V50_INT		0xD2
#define EC_PMC2_CMD_V33_FLT		0xD1
#define EC_PMC2_CMD_V33_INT		0xD0

/* Globals */

extern int smbus_fd;
extern int kernel_fd;
extern int susi_err;

/* -------------------------- External API --------------------------------- */

/* Check if available */
u8 SusiHWMAvailable(void)
{
	if (smbus_fd >= 0 && kernel_fd >= 0) 
		return 1;
	else {
		susi_err = -EAGAIN;
		return -1;
	}
}

/* Get Fan Speed - Not supported */
s8 SusiHWMGetFanSpeed(u16 type, u16 *retval, u16 *avail)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return -1;
	}

	if (!retval) {
		susi_err = -EINVAL;
		return 0;
	}

	if (avail)
		*avail = 0;

	*retval = 0;

	susi_err = -ENODEV;
	return 0;
}

/* Set Fan Speed - Not supported */
s8 SusiHWMSetFanSpeed(u16 type, u8 setval, u16 *avail)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (avail)
		*avail = 0;

	susi_err = -ENODEV;
	return 0;
}

/* Get Temperature sensor data */
s8 SusiHWMGetTemperature(u16 type, flt *retval, u16 *avail)
{
	u8 ipart = 0, fpart = 0;
	u8 str [20];

	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!retval) {
		susi_err = -EINVAL;
		return 0;
	}

	switch(type) {
		case TCPU:
			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_TCPU_INT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &ipart);
			debug("%s: Ipart: %d\n", __FUNC__, ipart);

			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_TCPU_FLT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &fpart);
			debug("%s: Fpart: %d\n", __FUNC__, fpart);

			sprintf(str, "%d.%.2d", ipart, fpart);
			*retval = (flt)atof(str);
			break;
		case TSYS:
			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_TSYS);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &fpart);
			*retval = (flt)fpart;
			break;
		default:
			susi_err = -EINVAL;
			return 0;
	}

	if (avail)
		*avail = 0x3;

	return 1;
}

/* Get Voltage sensor data */
s8 SusiHWMGetVoltage(u16 type, flt *retval, u16 *avail)
{
	u8 ipart = 0, fpart = 0;
	u8 str [20];

	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!retval) {
		susi_err = -EINVAL;
		return 0;
	}

	switch(type) {
		case VCORE:
			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_VCORE_INT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &ipart);

			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_VCORE_FLT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &fpart);

			sprintf(str, "%d.%.2d", ipart, fpart);
			*retval = (flt)atof(str);
			break;
		case V33:
			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_V33_INT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &ipart);

			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_V33_FLT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &fpart);

			sprintf(str, "%d.%.2d", ipart, fpart);
			*retval = (flt)atof(str);
			break;
		case V50:
			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_V50_INT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &ipart);

			SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_V50_FLT);
			sleep(1);

			SusiPortIOGetByte(EC_PMC2_DAT, &fpart);

			sprintf(str, "%d.%.2d", ipart, fpart);
			*retval = (flt)atof(str);
			break;
		default:
			susi_err = -EINVAL;
			return 0;
	}
	
	if (avail)
		*avail = VCORE | V33 | V50;

	return 1;
}
