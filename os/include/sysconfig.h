#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_

#define MAXPA           (512 * 1024 * 1024)
#define KSTKSIZE        (60 * 1024 * 1024)

#define EL2STACKTOP     0x04000000
#define KSTACKTOP       (EL3STACKTOP - 0x400000)
#define UTOP            (UPAGES + 0x300000)
#define UPAGES          (UVPT + 0x1000)
#define UVPT            0x02000000
#define KERNBASE        0x00080000

#endif // _SYSCONFIG_H_

