#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <stdio.h>

static char stack[32 * 1024];
static int var;

static int clone_fn(void *arg)
{
	int id = (int)arg;
	int i = 0;
	for (;;i++)
	{
		printf("clone_fn(%d) -> pid: %d (%d) var: %d\n", id, getpid(), i, var);
		if (id) var++;
		sleep(i);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int r = clone(clone_fn, stack + sizeof(stack) - sizeof(void*), 
				CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_PTRACE | CLONE_VM, (void*)1);
	printf("%s -> pid: %d, clone: %d\n\n", argv[0], getpid(), r);
	clone_fn(0);
}
