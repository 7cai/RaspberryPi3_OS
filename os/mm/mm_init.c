#include "mmu.h"
#include "pmap.h"
#include "env.h"
#include "error.h"

Pte *boot_pgdir;
/* These variables are set by detect_memory() */
u_long freemem;

/* Overview:
 *  Initialize basemem and npage.
 *  Set basemem to be 64MB, and calculate corresponding npage value.*/
void detect_memory()
{
    /* Initialize `freemem`. The first virtual address that the
     * linker did *not* assign to any kernel code or global variables. */
    freemem = (u_long)KSTACKTOP;
}

/* Overview:
 *  Allocate `n` bytes physical memory with alignment `align`, if `clear` is set, clear the
 *  allocated memory.
 *  This allocator is used only while setting up virtual memory system.
 *
 * Post-Condition:
 *  If we're out of memory, should panic, else return this address of memory we have allocated.*/
static void *boot_alloc(u_int n, u_int align, int clear)
{
    u_long alloced_mem;

    /* Step 1: Round up `freemem` up to be aligned properly */
    freemem = ROUND(freemem, align);

    /* Step 2: Save current value of `freemem` as allocated chunk. */
    alloced_mem = freemem;

    /* Step 3: Increase `freemem` to record allocation. */
    freemem += n;

    /* Step 4: Clear allocated chunk if parameter `clear` is set. */
    if (clear)
    {
        boot_bzero(alloced_mem, n);
    }

    // We're out of memory, PANIC !!
    if (freemem >= MAXPA)
    {
        return (void *)-E_NO_MEM;
    }

    /* Step 5: return allocated chunk. */
    return (void *)alloced_mem;
}


// /* Overview:
//  *  Get the page table entry for virtual address `va` in the given
//  *  page directory `pgdir`.
//  *  If the page table is not exist and the parameter `create` is set to 1,
//  *  then create it.*/
static Pte *boot_pgdir_walk(Pte *pgdir, u_long va, int create)
{
    Pte *pgtable0_entryp;
    Pte *pgtable1_entryp;
    Pte *pgtable2_entryp;
    Pte *pgtable3_entryp;

    /* Step 1: Get the corresponding page directory entry and page table. */

    /* Hint: Use PTE_ADDR to get the page table from page directory
     * entry value. */
    pgtable0_entryp = (Pte *)(&pgdir[PT0X(va)]);

    /* Step 2: If the corresponding page table is not exist and parameter `create`
     * is set, create one. And set the correct permission bits for this new page
     * table. */
    pgtable1_entryp = (Pte *)(PTE_ADDR(*pgtable0_entryp)) + PT1X(va);
    if (!(*pgtable0_entryp & PTE_V) && create)
    {
        pgtable1_entryp = (Pte *)boot_alloc(BY2PG, BY2PG, 1);
        *pgtable0_entryp = (u_long)pgtable1_entryp | PTE_V;
        pgtable1_entryp += PT1X(va);
    }
    else if (!(*pgtable0_entryp & PTE_V))
    {
        return NULL;
    }

    pgtable2_entryp = (Pte *)(PTE_ADDR(*pgtable1_entryp)) + PT2X(va);
    if (!(*pgtable1_entryp & PTE_V) && create)
    {
        pgtable2_entryp = (Pte *)boot_alloc(BY2PG, BY2PG, 1);
        *pgtable1_entryp = (u_long)pgtable2_entryp | PTE_V;
        pgtable2_entryp += PT2X(va);
    }
    else if (!(*pgtable1_entryp & PTE_V))
    {
        return NULL;
    }

    pgtable3_entryp = (Pte *)(PTE_ADDR(*pgtable2_entryp)) + PT3X(va);
    if (!(*pgtable2_entryp & PTE_V) && create)
    {
        pgtable3_entryp = (Pte *)boot_alloc(BY2PG, BY2PG, 1);
        *pgtable2_entryp = (u_long)pgtable3_entryp | PTE_V;
        pgtable3_entryp += PT3X(va);
    }
    else if (!(*pgtable2_entryp & PTE_V))
    {
        return NULL;
    }

    /* Step 3: Get the page table entry for `va`, and return it. */
    Pte *res = pgtable3_entryp;
    return res;
}


// /*Overview:
//  *  Map [va, va+size) of virtual address space to physical [pa, pa+size) in the page
//  *  table rooted at pgdir.
//  *  Use permission bits `perm|PTE_V` for the entries.
//  *  Use permission bits `perm` for the entries.
//  *
//  * Pre-Condition:
//  *  Size is a multiple of BY2PG.*/
void boot_map_segment(Pte *pgdir, u_long va, u_long size, u_long pa, int perm)
{
    Pte *pgtable_entry;

    /* Step 1: Check if `size` is a multiple of BY2PG. */
    if (size % BY2PG != 0)
    {
        //panic("size %ld is unaligned!", size);
    }

    /* Step 2: Map virtual address space to physical address. */
    /* Hint: Use `boot_pgdir_walk` to get the page table entry of virtual address `va`. */
    do
    {
        // boot_pgdir_walk_new(pgdir, va, 1, &pgtable_entry);
        pgtable_entry = boot_pgdir_walk(pgdir, va, 1);
        *pgtable_entry = (PTE_ADDR(pa) | perm | PTE_V | ATTRIB_AP_RW_EL1 | ATTRIB_SH_INNER_SHAREABLE | AF | UXN);
        va += BY2PG;
        pa += BY2PG;
    } while (size -= BY2PG);
}


/* Overview:
 *      Set up two-level page table.
 *
 * Hint:  You can get more details about `UPAGES` and `UENVS` in include/mmu.h. */
void vm_init()
{
    Pte *pgdir;
    u_int n;

    detect_memory();

    // /* Step 1: Allocate a page for page directory(first level page table). */
    pgdir = boot_alloc(BY2PG, BY2PG, 1);

    /* Step 2: Allocate proper size of physical memory for global array `pages`,
     * for physical memory management. Then, map virtual address `UPAGES` to
     * physical address `pages` allocated before. For consideration of alignment,
     * you should round up the memory size before map. */
    //pages = (struct Page *)boot_alloc(npage * sizeof(struct Page), BY2PG, 1);

    //envs = (struct Env *)boot_alloc(NENV * sizeof(struct Env), BY2PG, 1);

    n = ROUND(MAXPA, BY2PG);
    boot_map_segment(pgdir, 0, n, 0, ATTRINDX_NORMAL);
    boot_map_segment(pgdir, n, n, n, ATTRINDX_DEVICE);
}

void boot_bzero(u_long b, size_t len)
{
    u_long max = b + len;

    // zero machine words while possible
    while (b + 7 < max)
    {
        *(long *)b = 0;
        b += 8;
    }

    // finish remaining 0-3 bytes
    while (b < max)
    {
        *(char *)b++ = 0;
    }
}