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

	u_int            env_ipc_value;   // data value sent to us
    u_int *          env_ipc_value_adr;//
	u_int            env_ipc_from;    // envid of the sender
    u_long *         env_ipc_from_adr;// 
	u_int            env_ipc_recving; // env is blocked receiving
	u_long           env_ipc_dstva;	  // va at which to map received page
	u_int            env_ipc_perm;	  // perm of page mapping received
    u_int *          env_ipc_perm_adr;//
};

LIST_HEAD(Env_list, Env);
extern struct Env *envs;            // All environments
extern struct Env *curenv;          // the current env

void env_init(void);
int env_alloc(struct Env **e, u_int parent_id);
void env_free(struct Env *);
void env_create(u_char *binary, int size);

void env_run(struct Env *e);


// void _ipc_recv(u_long *, u_long,);
// int _ipc_can_send(u_long, u_int, u_long, u_int);

#endif // !_ENV_H_
