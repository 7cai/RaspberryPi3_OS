#include "syscall.h"

int umain()
{
	printf("test pingpong...\n");
	
	unsigned long who;
	int i, perm;

	who = fork();
	
	if (who != 0) {
		// get the ball rolling
		printf("\nsend 0 from %x to %x\n", getenvid(), who);
		ipc_send(who, 0, 0, 0);
	}

	for (;;) {
		printf("%x am waiting.....\n", getenvid());
		i = ipc_recv(&who, 0, &perm);

		printf("%x got %d from %x\n", getenvid(), i, who);

		if (i == 10)
		{
			while(1);
		}

		i++;
		printf("\nsend %d from %x to %x\n", i, getenvid(), who);
		ipc_send(who, i, 0, 0);

		if (i == 10)
		{
			while(1);
		}
	}

	while(1);
}