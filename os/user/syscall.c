#include "error.h"
#include "syscall.h"

int ipc_send(unsigned long whom, int val, int srcva, int perm)
{
	int r;

	while ((r = ipc_can_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV)
	{
		yield();
	}

	return r;
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
int ipc_recv(unsigned long *whom, unsigned long dstva, int *perm)
{
    int value;
	ipc_can_recv(whom, dstva, perm, &value);

    yield();

	return value;
}
