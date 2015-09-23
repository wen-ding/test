/* SUSI Library
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 */

#include "susi.h"

#define EC_PMC2_CMD			0x6C
#define EC_PMC2_DAT			0x68

/* Watchdog cmds */
#define EC_PMC2_CMD_WDT_START		0xF0
#define EC_PMC2_CMD_WDT_STOP		0xF1
#define EC_PMC2_CMD_WDT_TRIGGER		0xF2
#define EC_PMC2_CMD_WDT_SET_TIME	0xF3

/* Globals */

extern int smbus_fd;
extern int kernel_fd;
extern int susi_err;

/* -------------------------- External API --------------------------------- */

/* Check if available */
u8 SusiWDAvailable(void)
{
	if (smbus_fd >= 0 && kernel_fd >= 0) 
		return 1;
	else {
		susi_err = -EAGAIN;
		return -1;
	}
}

/* Get settings - Not supported */
s8 SusiWDGetRange(u32 *min, u32* max, u32* step)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (!min || !max || !step) {
		susi_err = -EINVAL;
		return 0;
	}

	susi_err = -ENODEV;
	return 0;
}

/* Start WD timer */
s8 SusiWDSetConfig(u32 delay, u32 timeout)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	if (delay < 0 || timeout < 0) {
		susi_err = -EINVAL;
		return 0;
	}

	usleep(delay * 1000);

	SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_WDT_STOP);
	sleep(1);

	SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_WDT_SET_TIME);
	sleep(1);

	SusiPortIOSetByte(EC_PMC2_DAT, timeout / 1000);
	sleep(1);

	SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_WDT_START);
	sleep(1);
	
	return 1;
}

/* Reset timer */
s8 SusiWDTrigger(void)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_WDT_TRIGGER);
	sleep(1);

	return 1;
}

/* Disable WD */
s8 SusiWDDisable(void)
{
	if (smbus_fd < 0 || kernel_fd < 0) {
		susi_err = -EAGAIN;
		return 0;
	}

	SusiPortIOSetByte(EC_PMC2_CMD, EC_PMC2_CMD_WDT_STOP);
	sleep(1);

	return 1;
}

