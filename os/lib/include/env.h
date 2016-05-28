#ifndef _ENV_H_
#define _ENV_H_

#include "types.h"
#include "queue.h"
#include "trap.h"
#include "mmu.h"

#define LOG2NENV         10
#define NENV             (1 << LOG2NENV)
#define ENVX(envid)      ((envid) & (NENV - 1))

// Values of env_status in struct Env
#define ENV_FREE         0
#define ENV_RUNNABLE     1
#define ENV_NOT_RUNNABLE 2

struct Env
{
    struct Trapframe env_tf;        // Saved registers
    LIST_ENTRY(Env)  env_link;      // Free list
    u_int            env_id;        // Unique environment identifier
    u_int            env_parent_id; // env_id of this env's parent
    u_int            env_status;    // Status of the environment
    Pte              *env_ttbr0;    // Address of page dir
};

LIST_HEAD(Env_list, Env);
extern struct Env *envs;            // All environments
extern struct Env *curenv;          // the current env

void env_init(void);
int env_alloc(struct Env **e, u_int parent_id);
void env_free(struct Env *);
void env_create(u_char *binary, int size);
void env_destroy(struct Env *e);

int envid2env(u_int envid, struct Env **penv, int checkperm);
void env_run(struct Env *e);

#endif // !_ENV_H_
