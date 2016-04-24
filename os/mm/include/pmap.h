#ifndef _PMAP_H_
#define _PMAP_H_

#include "types.h"
#include "queue.h"
#include "mmu.h"
#include "rpsio.h"
#include "rpslib.h"

LIST_HEAD(Page_list, Page);
typedef LIST_ENTRY(Page) Page_LIST_entry_t;

struct Page
{
    Page_LIST_entry_t pp_link;	/* free list link */

    // Ref is the count of pointers (usually in page table entries)
    // to this page.  This only holds for pages allocated using
    // page_alloc.  Pages allocated at boot time using pmap.c's "alloc"
    // do not have valid reference count fields.

    u_short pp_ref;
};

extern struct Page *pages;

/********** functions for memory management(see implementation in /mm/pmap.c). ***********/

extern struct Page *pages;
extern void tlb_invalidate(u_long va);

void detect_memory();
void vm_init();
void page_init();
void page_check();
int page_alloc(struct Page **pp);
void page_free(struct Page *pp);
void page_decref(struct Page *pp);
int pgdir_walk(Pte *pgdir, u_long va, int create, Pte **ppte);
int page_insert(Pte *pgdir, struct Page *pp, u_long va, u_int perm);
struct Page *page_lookup(Pte *pgdir, u_long va, Pte **ppte);
void page_remove(Pte *pgdir, u_long va) ;
void boot_map_segment(Pte *pgdir, u_long va, u_long size, u_long pa, int perm);

static inline u_long page2ppn(struct Page *pp)
{
    return pp - pages;
}

/* Get the physical address of Page 'pp'. */
static inline u_long page2pa(struct Page *pp)
{
    return page2ppn(pp) << PGSHIFT;
}

static inline u_long va2pa(Pte* pgdir, u_long va)
{
    Pte* ppte;
    pgdir_walk(pgdir, va, 0, &ppte);
    if (!ppte || !(*ppte & PBE_V))
    {
        return ~0;
    }
    return (u_long)((Pte *)PTE_ADDR(*ppte));
}

/* Get the Page struct whose physical address is 'pa'. */
static inline struct Page* pa2page(u_long pa)
{
    if (PPN(pa) >= npage)
    {
        panic("pa2page called with invalid pa: %x", pa);
    }

    return &pages[PPN(pa)];
}

/* Get the kernel virtual address of Page 'pp'.*/
static inline u_long page2kva(struct Page *pp)
{
    return page2pa(pp);
}

#endif /* _PMAP_H_ */
