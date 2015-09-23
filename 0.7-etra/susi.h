/* SUSI Library
 * (C) Advantech 2010
 *
 * See the SUSI Linux API document for API details.
 *
 */

#ifndef __SUSI_H__
#define __SUSI_H__

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

/* Library Version */
#define SUSI_LIB_VER_MJ		0x00
#define SUSI_LIB_VER_MR		0x07

/* Digital IO Masks */
#define ESIO_SMASK_PIN_FULL	0x01
#define ESIO_SMASK_CONFIGURABLE	0x02
#define ESIO_DMASK_DIRECTION	0x20

/* GPIO Values */
#define GPIO_HIGH		0x01
#define GPIO_LOW		0x00
#define GPIO_OUTPUT		0x00
#define GPIO_INPUT		0x01

/* Temperature and Fan Speed Flags */
#define TCPU			0x01
#define TSYS			0x02
#define FCPU			0x01
#define FSYS			0x02

/* Voltage Flags */
#define VCORE			(1 << 0)
#define V25			(1 << 1)
#define V33			(1 << 2)
#define V50			(1 << 3)
#define V120			(1 << 4)
#define VSB			(1 << 5)
#define VBAT			(1 << 6)
#define VN50			(1 << 7)
#define VN120			(1 << 8)
#define VTT			(1 << 9)

#define DEBUG 			0

#if (DEBUG == 1)
	#define __FUNC__ __FUNCTION__
	#define debug printf 
#else
	#define debug(...)
#endif

/* Types */

typedef signed char	s8;
typedef unsigned char	u8;
typedef signed short	s16;
typedef unsigned short	u16;
typedef signed int	s32;
typedef unsigned int	u32;
typedef float		flt;
typedef void *		ptr;

#ifdef __cplusplus
extern "C" {
#endif

/* Library API */

void SusiGetVersion(u16 *major, u16 *minor);
s8 SusiUnInit(void);
s8 SusiInit(void);
s32 SusiGetLastError(void);

/* SMBus API */
u8 SusiSMBusAvailable(void);

s8 SusiSMBusWriteQuick(u8 address);
s8 SusiSMBusReadQuick(u8 address);
s8 SusiSMBusReceiveByte(u8 address, u8 *value);
s8 SusiSMBusSendByte(u8 address, u8 value);
s8 SusiSMBusReadByte(u8 address, u8 offset, u8 *value);
s8 SusiSMBusWriteByte(u8 address, u8 offset, u8 value);
s8 SusiSMBusReadWord(u8 address, u8 offset, u16 *value);
s8 SusiSMBusWriteWord(u8 address, u8 offset, u16 value);
s8 SusiSMBusScanDevice(u8 address);

/* GPIO API */
u8 SusiIOAvailable(void);

s8 SusiIOCountEx(u32 *incnt, u32 *outcnt);
s8 SusiIOQueryMask(u32 flag, u32 *mask);
s8 SusiIOSetDirection(u8 pin, u8 dir, u32 *pinmask);
s8 SusiIOSetDirectionMulti(u32 targetmask, u32 *pinmask);
s8 SusiIOReadEx(u8 pin, u8 *status);
s8 SusiIOReadMultiEx(u32 targetmask, u32 *statusmask);
s8 SusiIOWriteEx(u8 pin, u8 status);
s8 SusiIOWriteMultiEx(u32 targetmask, u32 statusmask);

/* Hardware Monitoring API */
u8 SusiHWMAvailable(void);
s8 SusiHWMGetFanSpeed(u16 type, u16 *retval, u16 *avail);
s8 SusiHWMSetFanSpeed(u16 type, u8 setval, u16 *avail);
s8 SusiHWMGetTemperature(u16 type, flt *retval, u16 *avail);
s8 SusiHWMGetVoltage(u16 type, flt *retval, u16 *avail);

/* Watchdog API */
u8 SusiWDAvailable(void);
s8 SusiWDGetRange(u32 *min, u32* max, u32* step);
s8 SusiWDSetConfig(u32 delay, u32 timeout);
s8 SusiWDTrigger(void);
s8 SusiWDDisable(void);

/* Port I/O API */
u8 SusiPortIOAvailable(void);
s8 SusiPortIOGetByte(u16 port, u8 *data);
s8 SusiPortIOGetWord(u16 port, u16 *data);
s8 SusiPortIOGetLong(u16 port, u32 *data);
s8 SusiPortIOSetByte(u16 port, u8 data);
s8 SusiPortIOSetWord(u16 port, u16 data);
s8 SusiPortIOSetLong(u16 port, u32 data);

/* Misc API */
s8 SusiUSBHubCtrl(u8 enable);
s8 SusiVCAvailable(void);
s32 SusiIICAvailable(void);
s32 SusiCoreAvailable(void);

#ifdef __cplusplus
}
#endif

#endif /* __SUSI_H__ */
