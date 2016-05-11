#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_

#define MAXPA          (512 * 1024 * 1024)

#define EL2STACKTOP    (0x01000000)
#define KSTACKTOP      (EL2STACKTOP - 0x100000)
#define KERNBASE       (0x00080000)

#endif // _SYSCONFIG_H_
