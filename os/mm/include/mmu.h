#ifndef _MMU_H_
#define _MMU_H_

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
#define MAXPA   (512 * 1024 * 1024)
#define BY2PG   4096            // bytes to a page
#define PDMAP	(4*1024)	// bytes mapped by a third page table entry

// index of page table entry
#define PT0X(va)    ((((u_long)(va))>>39) & 0x01FF)
#define PT1X(va)    ((((u_long)(va))>>30) & 0x01FF)
#define PT2X(va)    ((((u_long)(va))>>21) & 0x01FF)
#define PT3X(va)    ((((u_long)(va))>>12) & 0x01FF)

// gets addr of pte from pte with modifier
#define PTE_ADDR(pte)   ((u_long)(pte) & 0xFFFFFFFFF000)

// page number field of address
#define PPN(va)     (((u_long)(va))>>12)
#define VPN(va)     PPN(va)
#define PGSHIFT		12

#define VA2PFN(va)  (((u_long)(va)) & 0xFFFFFFFFF000) // va 2 PFN for EntryLo0/1
#define PTE2PT      512

/* Page Table/Directory Entry flags
 *   these are defined by the hardware
 */
#define PTE_V       0x0003  // Table Entry Valid bit
#define PBE_V       0x0001  // Block Entry Valid bit
#define PTE_R       0x0000  // Dirty bit ,'0' means only read ,otherwise make interrupt
// #define PTE_G       0x0100  // Global bit
// #define PTE_D       0x0002  // fileSystem Cached is dirty
// #define PTE_COW     0x0001  // Copy On Write
// #define PTE_UC      0x0800  // unCached
// #define PTE_LIBRARY 0x0004  // share memmory

/*
 * Part 2.  Our conventions.
 */

/*
 o           4G -----------> +----------------------------+------------0x     0001 0000 0000
 o                           |                            |
 o                           |            ...             |
 o                           |                            |
 o                           +----------------------------+------------0x          0480 8000
 o                           |           PAGES            |
 o              UPAGES-----> +----------------------------+------------0x          0440 8000
 o                           |         Page Table         |
 o   UVPT, EL3STACKTOP-----> +----------------------------+------------0x          0400 8000
 o                           |          EL3 stack         |
 o           KSTACKTOP-----> +----------------------------+------------0x          03C0 8000
 o                           |            stack           |
 o                           |                            |
 o                           |         Kernel Text        |
 o            KERNBASE-----> +----------------------------+------------0x               8000
 o                           |          reserved          |
 a           0 ------------> +----------------------------+------------0x               0000
 o
*/

#define KERNBASE 0x8000

#define KSTACKTOP   EL3STACKTOP - 0x400000
#define EL3STACKTOP 0x04008000
#define UVPT        EL3STACKTOP
#define UPAGES      UVPT + 0x400000

#define KSTKSIZE (60*1024*1024)

#ifndef __ASSEMBLER__

/*
 * Part 3.  Our helper functions.
 */
#include "types.h"
void bcopy(const void *, void *, size_t);
void bzero(void *, size_t);

extern char bootstacktop[], bootstack[];

extern u_long npage;

typedef u_long Pte;

extern volatile Pte *vpt0[];
extern volatile Pte *vpt1[];
extern volatile Pte *vpt2[];
extern volatile Pte *vpt3[];

#define assert(x)   \
    do {    if (!(x)) panic("assertion failed: %s", #x); } while (0)

#define TRUP(_p)                        \
    ({                              \
        register typeof((_p)) __m_p = (_p);         \
        (u_int) __m_p > ULIM ? (typeof(_p)) ULIM : __m_p;   \
    })

extern void tlb_out(u_int entryhi);

#endif //!__ASSEMBLER__
#endif // !_MMU_H_
