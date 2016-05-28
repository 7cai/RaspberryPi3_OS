#include <rpsio.h>
#include <mmu.h>
#include <pmap.h>
#include <error.h>
#include <env.h>
#include <kerelf.h>
#include <sched.h>

extern struct Env *envs;              // All environments
struct Env *curenv;                   // the current env

static struct Env_list env_free_list; // Free list

extern int load_elf(u_char *, int, u_long *, void *, int (*map)(u_long, u_int32_t, u_char *, u_int32_t, void *));

extern char *syscall_vector;

/* Overview:
 *  This function is for making an unique ID for every env.
 *
 * Pre-Condition:
 *  Env e is exist.
 *
 * Post-Condition:
 *  return e's envid on success.
 */
u_int mkenvid(struct Env *e)
{
    static u_long next_env_id = 0;

    /*Hint: lower bits of envid hold e's position in the envs array. */
    u_int idx = e - envs;

    /*Hint:  high bits of envid hold an increasing number. */
    return (++next_env_id << (1 + LOG2NENV)) | idx;
}


/* Overview:
 *  Converts an envid to an env pointer.
 *  If envid is 0 , set *penv = curenv;otherwise set *penv = envs[ENVX(envid)];
 *
 * Pre-Condition:
 *  Env penv is exist,checkperm is 0 or 1.
 *
 * Post-Condition:
 *  return 0 on success,and sets *penv to the environment.
 *  return -E_BAD_ENV on error,and sets *penv to NULL.
 */
int envid2env(u_int envid, struct Env **penv, int checkperm)
{
    struct Env *e;

    /* Hint:
     *  If envid is zero, return the current environment.*/
    if (envid == 0)
    {
        *penv = curenv;
        return 0;
    }

    e = &envs[ENVX(envid)];

    if ((e->env_status == ENV_FREE) || (e->env_id != envid))
    {
        *penv = 0;
        return -E_BAD_ENV;
    }

    /* Hint:
     *  Check that the calling environment has legitimate permissions
     *  to manipulate the specified environment.
     *  If checkperm is set, the specified environment
     *  must be either the current environment.
     *  or an immediate child of the current environment. */
    if (checkperm && (e != curenv) && (e->env_parent_id != curenv->env_id))
    {
        *penv = 0;
        return -E_BAD_ENV;
    }
    *penv = e;
    return 0;
}


/* Overview:
 *  Mark all environments in 'envs' as free and insert them into the env_free_list.
 *  Insert in reverse order,so that the first call to env_alloc() return envs[0].
 *
 * Hints:
 *  You may use these defines to make it:
 *      LIST_INIT,LIST_INSERT_HEAD
 */
void env_init(void)
{
    curenv = NULL;

    int i;
    /*Step 1: Initial env_free_list. */
    LIST_INIT(&env_free_list);

    /*Step 2: Travel the elements in 'envs', init every element(mainly initial its status, mark it as free)
     * and inserts them into the env_free_list as reverse order. */
    for (i = NENV - 1; i >= 0; i--)
    {
        envs[i].env_status = ENV_FREE;
        LIST_INSERT_HEAD(&env_free_list, &envs[i], env_link);
    }

    struct Page *p;
    if (page_alloc(&p) < 0)
    {
        panic("env_setup_vm - page_alloc error\n");
    }
    p->pp_ref++;
}


/* Overview:
 *  Initialize the kernel virtual memory layout for environment e.
 *  Allocate a page directory, set e->ttbr1 and e->env_cr3 accordingly,
 *  and initialize the kernel portion of the new environment's address space.
 *  Do NOT map anything into the user portion of the environment's virtual address space.
 */
static int env_setup_vm(struct Env *e)
{
    int r;
    struct Page *p = NULL;
    Pte *pgdir;

    /*Step 1: Allocate a page for the page directory and add its reference.
     * pgdir is the page directory of Env e. */
    if ((r = page_alloc(&p)) < 0)
    {
        panic("env_setup_vm - page_alloc error\n");
        return r;
    }
    p->pp_ref++;
    pgdir = (Pte *)page2pa(p);

    e->env_ttbr0 = pgdir;
    return 0;
}


/* Overview:
 *  Allocates and Initializes a new environment.
 *  On success, the new environment is stored in *new.
 *
 * Pre-Condition:
 *  If the new Env doesn't have parent, parent_id should be zero.
 *  env_init has been called before this function.
 *
 * Post-Condition:
 *  return 0 on success, and set appropriate values for Env new.
 *  return -E_NO_FREE_ENV on error, if no free env.
 *
 * Hints:
 *  You may use these functions and defines:
 *      LIST_FIRST,LIST_REMOVE,mkenvid (Not All)
 *  You should set some states of Env:
 *      id , status , the sp register, CPU status , parent_id
 *      (the value of PC should NOT be set in env_alloc)
 */
int env_alloc(struct Env **new, u_int parent_id)
{
    int r;
    struct Env *e;

    /*Step 1: Get a new Env from env_free_list*/
    e = LIST_FIRST(&env_free_list);
    if (e == NULL)
    {
        return -E_NO_FREE_ENV;
    }

    /*Step 2: Call certain function(has been implemented) to init kernel memory layout for this new Env.
     * The function mainly maps the kernel address to this new Env address. */
    if ((r = env_setup_vm(e)) < 0)
    {
        return r;
    }

    /*Step 3: Initialize every field of new Env with appropriate values*/
    e->env_id = mkenvid(e);
    e->env_parent_id = parent_id;
    e->env_status = ENV_RUNNABLE;

    /*Step 4: focus on initializing env_tf structure, located at this new Env.
     * especially the sp register(no way!), CPU status. */
    e->env_tf.spsr = 0;
    e->env_tf.sp = USTACKTOP;

    /*Step 5: Remove the new Env from Env free list*/
    LIST_REMOVE(e, env_link);

    *new = e;
    return 0;
}


/* Overview:
 *   This is a call back function for kernel's elf loader.
 * Elf loader extracts each segment of the given binary image.
 * Then the loader calls this function to map each segment
 * at correct virtual address.
 *
 *   `bin_size` is the size of `bin`. `sgsize` is the
 * segment size in memory.
 *
 * Pre-Condition:
 *   va aligned 4KB and bin can't be NULL.
 *
 * Post-Condition:
 *   return 0 on success, otherwise < 0.
 */
static int load_icode_mapper(u_long va, u_int32_t sgsize,
                             u_char *bin, u_int32_t bin_size, void *user_data)
{
    struct Env *env = (struct Env *)user_data;
    struct Page *p = NULL;
    u_long i;
    int r;
    u_long offset = va - ROUNDDOWN(va, BY2PG);

    /*Step 1: load all content of bin into memory. */
    if (offset)
    {
        /* Hint: You should alloc a page and increase the reference count of it. */
        if ((r = page_alloc(&p)) < 0)
        {
            return r;
        }
        p->pp_ref++;

        char *src = (char *)((u_long)bin);
        char *dest = (char *)(page2kva(p) + offset);
        bcopy(src, dest, BY2PG - offset);
        char *temp_va = (char *)(va - offset);
        page_insert(env->env_ttbr0, p, (u_long)temp_va, ATTRIB_AP_RW_ALL);
    }

    for (i = offset; i < bin_size; i += BY2PG)
    {
        /* Hint: You should alloc a page and increase the reference count of it. */
        if ((r = page_alloc(&p)) < 0)
        {
            return r;
        }
        p->pp_ref++;
        if (bin_size - i >= BY2PG)
        {
            bcopy(bin + i, (void *)page2kva(p), BY2PG);
        }
        else
        {
            bcopy(bin + i, (void *)page2kva(p), bin_size - i);
        }

        page_insert(env->env_ttbr0, p, va + i, ATTRIB_AP_RW_ALL);
    }

    /*Step 2: alloc pages to reach `sgsize` when `bin_size` < `sgsize`.
     * i has the value of `bin_size` now. */
    while (i < sgsize)
    {
        if ((r = page_alloc(&p)) < 0)
        {
            return r;
        }
        p->pp_ref++;

        page_insert(env->env_ttbr0, p, va + i, ATTRIB_AP_RW_ALL);

        i += BY2PG;
    }

    return 0;
}


/* Overview:
 *  Sets up the the initial stack and program binary for a user process.
 *  This function loads the complete binary image by using elf loader,
 *  into the environment's user memory. The entry point of the binary image
 *  is given by the elf loader. And this function maps one page for the
 *  program's initial stack at virtual address USTACKTOP - BY2PG.
 *
 * Hints:
 *  All mappings are read/write including those of the text segment.
 *  You may use these :
 *      page_alloc, page_insert, page2pa , e->ttbr1 and load_elf.
 */
static void load_icode(struct Env *e, u_char *binary, u_int size)
{
    /* Hint:
     *  You must figure out which permissions you'll need
     *  for the different mappings you create.
     *  Remember that the binary image is an a.out format image,
     *  which contains both text and data.
     */
    struct Page *p = NULL;
    u_long entry_point;
    u_long r;

    /*Step 1: alloc a page. */
    if ((r = page_alloc(&p)) < 0)
    {
        panic("page alloc error: %d", r);
    }
    p->pp_ref++;

    /*Step 2: Use appropriate perm to set initial stack for new Env. */
    /*Hint: The user-stack should be writable? */
    page_insert(e->env_ttbr0, p, USTACKTOP - BY2PG, ATTRIB_AP_RW_ALL);

    /*Step 3:load the binary by using elf loader. */
    load_elf(binary, size, &entry_point, e, load_icode_mapper);

    /***Your Question Here***/
    /*Step 4:Set CPU's PC register as appropriate value. */
    e->env_tf.elr = entry_point;
}


/* Overview:
 *  Allocates a new env with env_alloc, loads te named elf binary into
 *  it with load_icode. This function is ONLY called during kernel
 *  initialization, before running the first user_mode environment.
 *
 * Hints:
 *  this function wrap the env_alloc and load_icode function.
 */
void env_create(u_char *binary, int size)
{
    struct Env *e;
    /*Step 1: Use env_alloc to alloc a new env. */
    env_alloc(&e, 0);

    /*Step 2: Use load_icode() to load the named elf binary. */
    load_icode(e, binary, size);
}


/* Overview:
 *  Frees env e and all memory it uses.
 */
void env_free(struct Env *e)
{
    u_int Pteno0, Pteno1, Pteno2, Pteno3;
    Pte *Pte1, *Pte2, *Pte3;

    /* Hint: Note the environment's demise.*/
    _printf("[%08x] free env %08x\n", curenv ? curenv->env_id : 0, e->env_id);

    /* Hint: Flush all mapped pages in the user portion of the address space */
    for (Pteno0 = 0; Pteno0 < PTE2PT; Pteno0++)
    {
        /* Hint: only look at mapped page tables. */
        if (!(e->env_ttbr0[Pteno0] & PTE_V))
        {
            continue;
        }
        Pte1 = (Pte *)e->env_ttbr0[Pteno0];
        /* Hint: Unmap all PTEs in this page table. */
        for (Pteno1 = 0; Pteno1 <= PTE2PT; Pteno1++)
        {
            if (!(Pte1[Pteno1] & PTE_V))
            {
                continue;
            }
            Pte2 = (Pte *)Pte1[Pteno1];

            for (Pteno2 = 0; Pteno2 <= PTE2PT; Pteno2++)
            {
                if (!(Pte2[Pteno2] & PTE_V))
                {
                    continue;
                }
                Pte3 = (Pte *)Pte2[Pteno2];

                for (Pteno3 = 0; Pteno3 <= PTE2PT; Pteno3++)
                {
                    if (!(Pte3[Pteno3] & PTE_V))
                    {
                        continue;
                    }

                    page_remove(e->env_ttbr0, PTE_ADDR(Pte3[Pteno3]));
                }

                Pte2[Pteno2] = 0;
                page_decref(pa2page((u_long)Pte3));
            }

            Pte1[Pteno1] = 0;
            page_decref(pa2page((u_long)Pte2));
        }
        /* Hint: free the page table itself. */
        e->env_ttbr0[Pteno0] = 0;
        page_decref(pa2page((u_long)Pte1));
    }
    /* Hint: free the page directory. */
    e->env_ttbr0 = 0;
    page_decref(pa2page((u_long)e->env_ttbr0));
    /* Hint: return the environment to the free list. */
    e->env_status = ENV_FREE;
    LIST_INSERT_HEAD(&env_free_list, e, env_link);
}


/* Overview:
 *  Frees env e, and schedules to run a new env
 *  if e is the current env.
 */
void env_destroy(struct Env *e)
{
    /* Hint: free e. */
    env_free(e);

    /* Hint: schedule to run a new environment. */
    if (curenv == e)
    {
        curenv = NULL;
        _printf("i am killed ... \n");
        sched_yield();
    }
}


extern void load_ttbr0_context(u_long ttbr0);

/* Overview:
 *  Restores the register values in the Trapframe with the
 *  env_pop_tf, and context switch from curenv to env e.
 *
 * Post-Condition:
 *  Set 'e' as the curenv running environment.
 *
 * Hints:
 *  You may use these functions:
 *      env_pop_tf and lcontext.
 */
void env_run(struct Env *e)
{
    /*Step 1: save register state of curenv. */

    /* Hint: if there is a environment running,you should do
     *  context switch.You can imitate env_destroy() 's behaviors.*/
    struct Trapframe *old;
    old = (struct Trapframe *)(TIMESTACKTOP - sizeof(struct Trapframe));

    if (curenv)
    {
        bcopy(old, &curenv->env_tf, sizeof(struct Trapframe));
        curenv->env_tf.elr = old->elr;
    }

    /*Step 2: Set 'curenv' to the new environment. */
    curenv = e;
    bcopy(&curenv->env_tf, old, sizeof(struct Trapframe));

    /*Step 3: Use lcontext() to switch to its address space. */
    load_ttbr0_context((u_long)curenv->env_ttbr0);
    tlb_invalidate();
}
