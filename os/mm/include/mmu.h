#ifndef _MMU_H_
#define _MMU_H_

#include "sysconfig.h"

/*
 * This file contains:
 *
 *  Part 1.  ARM definitions.
 *  Part 2.  Our conventions.
 *  Part 3.  Our helper functions.
 */

/*
 * Part 1.  ARM definitions.
 */
#define BY2PG    4096       // bytes to a page
#define PDMAP    (4 * 1024) // bytes mapped by a third page table entry

// index of page table entry
#define PT0X(va)         ((((u_long)(va)) >> 39) & 0x01FF)
#define PT1X(va)         ((((u_long)(va)) >> 30) & 0x01FF)
#define PT2X(va)         ((((u_long)(va)) >> 21) & 0x01FF)
#define PT3X(va)         ((((u_long)(va)) >> 12) & 0x01FF)

// gets addr of pte from pte with modifier
#define PTE_ADDR(pte)    ((u_long)(pte) & 0xFFFFFFFFF000)

// page number field of address
#define PPN(va)          (((u_long)(va)) >> 12)
#define VPN(va)          PPN(va)
#define PGSHIFT      12

#define VA2PFN(va)       (((u_long)(va)) & 0xFFFFFFFFF000) // va 2 PFN for EntryLo0/1
#define PTE2PT                       512

/* Page Table/Directory Entry flags
 *   these are defined by the hardware
 */
#define PTE_V                        0x3 << 0 // Table Entry Valid bit
#define PBE_V                        0x1 << 0 // Block Entry Valid bit
#define PTE_R                        0x0 << 0 // Dirty bit ,'0' means only read ,otherwise make interrupt
#define ATTRIB_AP_RW_EL1             0x0 << 6
#define ATTRIB_AP_RW_ALL             0x1 << 6
#define ATTRIB_AP_RO_EL1             0x2 << 6
#define ATTRIB_AP_RO_ALL             0x3 << 6
#define ATTRIB_SH_NON_SHAREABLE      0x0 << 8
#define ATTRIB_SH_OUTER_SHAREABLE    0x2 << 8
#define ATTRIB_SH_INNER_SHAREABLE    0x3 << 8
#define AF                           0x1 << 10
#define PXN                          0x0 << 53
#define UXN                          0x1UL << 54

#define ATTRINDX_NORMAL              0 << 2 // inner/outer write-back non-transient, non-allocating
#define ATTRINDX_DEVICE              1 << 2 // Device-nGnRE
#define ATTRINDX_COHERENT            2 << 2 // Device-nGnRnE

/*
 * Part 2.  Our conventions.
 */

/*
 *            4G -----------> +----------------------------+------------0x     0001 0000 0000
 *                            |                            |
 *                            |            ...             |
 *                            |                            |
 *                            |----------------------------+------------0x          0130 0000
 *                            |           ENVS             |
 *                            |----------------------------+------------0x          0130 0000
 *                            |           PAGES            |
 *         EL2STACKTOP -----> +----------------------------+------------0x          0100 0000
 *                            |          EL2 stack         |
 *           KSTACKTOP -----> +----------------------------+------------0x          00F0 0000
 *                            |         Kernel stack       |
 *                            |----------------------------|
 *                            |         Kernel Text        |
 *            KERNBASE -----> +----------------------------+------------0x             8 0000
 *                            |          reserved          |
 *            0 ------------> +----------------------------+------------0x               0000
 *
 */

/*
 * Part 3.  Our helper functions.
 */
#include "types.h"
void bcopy(const void *, void *, size_t);
void bzero(u_long, size_t);

extern char bootstacktop[], bootstack[];

extern u_long npage;

typedef u_long Pte;

#define assert(x)                                          \
    do { if (!(x)) { panic("assertion failed: %s", # x); } \
    } while (0)

#endif // !_MMU_H_
