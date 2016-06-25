#include "mmu.h"
#include "pmap.h"
#include "env.h"
#include "rpsio.h"
#include "error.h"

/* These variables are set by detect_memory() */
u_long npage = MAXPA / BY2PG;            /* Amount of memory(in pages) */

struct Page *pages = (struct Page *)KERNEL_PAGES;
struct Env *envs = (struct Env *)KERNEL_ENVS;

static struct Page_list page_free_list; /* Free list of physical pages */

/*Overview:
 *  Initialize page structure and memory free list.
 *  The `pages` array has one `struct Page` entry per physical page. Pages
 *  are reference counted, and free pages are kept on a linked list.
 * Hint:
 *  Use `LIST_INSERT_HEAD` to insert something to list.*/
void page_init(void)
{
    /* Step 1: Initialize page_free_list. */
    /* Hint: Use macro `LIST_INIT` defined in include/queue.h. */
    LIST_INIT(&page_free_list);

    /* Step 2: Align `freemem` up to multiple of BY2PG. */
    u_long freemem = ROUND(TIMESTACKTOP, BY2PG);

    /* Step 3: Mark all memory blow `freemem` as used(set `pp_ref`
     * filed to 1) */
    u_int used_amount = VPN(freemem);
    u_int i = 0;
    for ( ; i < used_amount; ++i)
    {
        pages[i].pp_ref = 0xffff;
    }

    /* Step 4: Mark the other memory as free. */
    for ( ; i < npage; ++i)
    {
        LIST_INSERT_HEAD(&page_free_list, &pages[i], pp_link);
    }
}


/*Overview:
 *  Allocates a physical page from free memory, and clear this page.
 *
 * Post-Condition:
 *  If failed to allocate a new page(out of memory(there's no free page)),
 *  return -E_NO_MEM.
 *  Else, set the address of allocated page to *pp, and returned 0.
 *
 * Note:
 *  Does NOT increment the reference count of the page - the caller must do
 *  these if necessary (either explicitly or via page_insert).
 *
 * Hint:
 *  Use LIST_FIRST and LIST_REMOVE defined in include/queue.h .*/
int page_alloc(struct Page **pp)
{
    struct Page *ppage_temp;

    /* Step 1: Get a page from free memory. If fails, return the error code.*/
    ppage_temp = LIST_FIRST(&page_free_list);
    if (ppage_temp == NULL)
    {
        return -E_NO_MEM;
    }
    LIST_REMOVE(ppage_temp, pp_link);

    /* Step 2: Initialize this page.
     * Hint: use `bzero`. */
    bzero(page2kva(ppage_temp), BY2PG);

    ppage_temp->pp_ref = 0;
    *pp = ppage_temp;

    return 0;
}


/*Overview:
 *  Release a page, mark it as free if it's `pp_ref` reaches 0.
 * Hint:
 *  When to free a page, just insert it to the page_free_list.*/
void page_free(struct Page *pp)
{
    /* Step 1: If there's still virtual address refers to this page, do nothing. */
    if (pp->pp_ref > 0)
    {
        return;
    }

    /* Step 2: If the `pp_ref` reaches to 0, mark this page as free and return. */
    LIST_INSERT_HEAD(&page_free_list, pp, pp_link);
}


/*Overview:
 *  Given `pgdir`, a pointer to a page directory, pgdir_walk returns a pointer
 *  to the page table entry (with permission PTE_R|PTE_V) for virtual address 'va'.
 *
 * Pre-Condition:
 *  The `pgdir` should be two-level page table structure.
 *
 * Post-Condition:
 *  If we're out of memory, return -E_NO_MEM.
 *  Else, we get the page table entry successfully, store the value of page table
 *  entry to *ppte, and return 0, indicating success.
 *
 * Hint:
 *  We use a two-level pointer to store page table entry and return a state code to indicate
 *  whether this function execute successfully or not.
 *  This function have something in common with function `boot_pgdir_walk`.*/
int pgdir_walk(Pte *pgdir, u_long va, int create, Pte **ppte)
{
    Pte *pgtable0_entryp;
    Pte *pgtable1_entryp;
    Pte *pgtable2_entryp;
    Pte *pgtable3_entryp;
    struct Page *ppage;

    /* Step 1: Get the corresponding page directory entry and page table. */
    pgtable0_entryp = (Pte *)KADDR(&pgdir[PT0X(va)]);

    /* Step 2: If the corresponding page table is not exist(valid) and parameter `create`
     * is set, create one. And set the correct permission bits for this new page
     * table.
     * When creating new page table, maybe out of memory. */
    pgtable1_entryp = (Pte *)KADDR((PTE_ADDR(*pgtable0_entryp))) + PT1X(va);
    if (!(*pgtable0_entryp & PTE_V) && create)
    {
        if (page_alloc(&ppage) < 0)
        {
            return -E_NO_MEM;
        }

        pgtable1_entryp = (Pte *)page2pa(ppage);
        *pgtable0_entryp = (u_long)pgtable1_entryp | PTE_V;
        pgtable1_entryp += PT1X(va);
        pgtable1_entryp = (Pte *)KADDR(pgtable1_entryp);
        ppage->pp_ref++;
    }
    else if (!(*pgtable0_entryp & PTE_V))
    {
        *ppte = NULL;
        return 0;
    }

    pgtable2_entryp = (Pte *)KADDR((PTE_ADDR(*pgtable1_entryp))) + PT2X(va);
    if (!(*pgtable1_entryp & PTE_V) && create)
    {
        if (page_alloc(&ppage) < 0)
        {
            return -E_NO_MEM;
        }
        pgtable2_entryp = (Pte *)page2pa(ppage);
        *pgtable1_entryp = (u_long)pgtable2_entryp | PTE_V;
        pgtable2_entryp += PT2X(va);
        pgtable2_entryp = (Pte *)KADDR(pgtable2_entryp);
        ppage->pp_ref++;
    }
    else if (!(*pgtable1_entryp & PTE_V))
    {
        *ppte = NULL;
        return 0;
    }
    
    pgtable3_entryp = (Pte *)KADDR((PTE_ADDR(*pgtable2_entryp))) + PT3X(va);
    if (!(*pgtable2_entryp & PTE_V) && create)
    {
        if (page_alloc(&ppage) < 0)
        {
            return -E_NO_MEM;
        }
        pgtable3_entryp = (Pte *)page2pa(ppage);
        *pgtable2_entryp = (u_long)pgtable3_entryp | PTE_V;
        pgtable3_entryp += PT3X(va);
        pgtable3_entryp = (Pte *)KADDR(pgtable3_entryp);
        ppage->pp_ref++;
    }
    else if (!(*pgtable2_entryp & PTE_V))
    {
        *ppte = NULL;
        return 0;
    }

    /* Step 3: Set the page table entry to `*ppte` as return value. */
    *ppte = pgtable3_entryp;

    return 0;
}


/*Overview:
 *  Map the physical page 'pp' at virtual address 'va'.
 *  The permissions (the low 12 bits) of the page table entry should be set to 'perm|PTE_V'.
 *
 * Post-Condition:
 *  Return 0 on success
 *  Return -E_NO_MEM, if page table couldn't be allocated
 *
 * Hint:
 *  If there is already a page mapped at `va`, call page_remove() to release this mapping.
 *  The `pp_ref` should be incremented if the insertion succeeds.*/
int page_insert(Pte *pgdir, struct Page *pp, u_long va, u_int perm)
{
    u_long PERM;
    Pte *pgtable_entry;
    PERM = perm | PTE_V | ATTRINDX_NORMAL | ATTRIB_SH_INNER_SHAREABLE | AF;
    /* Step 1: Get corresponding page table entry. */
    pgdir_walk(pgdir, va, 0, &pgtable_entry);
    
    if ((pgtable_entry != 0) && ((*pgtable_entry & PTE_V) != 0))
    {
        if (pa2page(PTE_ADDR(*pgtable_entry)) != pp)
        {
            page_remove(pgdir, va);
        }
        else
        {
            tlb_invalidate(va);
            *pgtable_entry = (PTE_ADDR(page2pa(pp)) | PERM);
            return 0;
        }
    }

    /* Step 2: Update TLB. */
    tlb_invalidate(va);

    /* Step 3: Do check, re-get page table entry to validate the insertion. */
    if (pgdir_walk(pgdir, va, 1, &pgtable_entry) != 0)
    {
        return -E_NO_MEM;    // panic ("page insert failed .\n");
    }

    *pgtable_entry = (PTE_ADDR(page2pa(pp)) | PERM);
    pp->pp_ref++;
    return 0;
}


/*Overview:
 *  Look up the Page that virtual address `va` map to.
 *
 * Post-Condition:
 *  Return a pointer to corresponding Page, and store it's page table entry to *ppte.
 *  If `va` doesn't mapped to any Page, return NULL.*/
struct Page *page_lookup(Pte *pgdir, u_long va, Pte **ppte)
{
    struct Page *ppage;
    Pte *pte;

    /* Step 1: Get the page table entry. */
    pgdir_walk(pgdir, va, 0, &pte);

    /* Hint: Check if the page table entry doesn't exist or is not valid. */
    if (pte == 0)
    {
        return 0;
    }
    if ((*pte & PTE_V) == 0)
    {
        return 0;    //the page is not in memory.
    }

    /* Step 2: Get the corresponding Page struct. */

    /* Hint: Use function `pa2page`, defined in include/pmap.h . */
    ppage = pa2page(PTE_ADDR(*pte));
    if (ppte)
    {
        *ppte = pte;
    }

    return ppage;
}


/* Overview:
 *  Decrease the `pp_ref` value of Page `*pp`, if `pp_ref` reaches to 0, free this page.*/
void page_decref(struct Page *pp)
{
    if (pp->pp_ref == 0)
    {
        page_free(pp);
    }
    else if (--pp->pp_ref == 0)
    {
        page_free(pp);
    }
}


/*Overview:
 *  Unmaps the physical page at virtual address `va`.*/
void page_remove(Pte *pgdir, u_long va)
{
    Pte *pagetable_entry;
    struct Page *ppage;

    /* Step 1: Get the page table entry, and check if the page table entry is valid. */
    ppage = page_lookup(pgdir, va, &pagetable_entry);

    if (ppage == 0)
    {
        return;
    }

    /* Step 2: Decrease `pp_ref` and decide if it's necessary to free this page. */
    /* Hint: When there's no virtual address mapped to this page, release it. */
    page_decref(ppage);

    /* Step 3: Update TLB. */
    *pagetable_entry = 0;
    tlb_invalidate(va);
}

void bcopy(const void *src, void *dst, size_t len)
{
    void *max;

    max = dst + len;

    // copy machine words while possible
    while (dst + 3 < max)
    {
        *(int *)dst = *(int *)src;
        dst += 4;
        src += 4;
    }

    // finish remaining 0-3 bytes
    while (dst < max)
    {
        *(char *)dst = *(char *)src;
        dst += 1;
        src += 1;
    }
}

void bzero(u_long b, size_t len)
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
