#include "seccomp-bpf.h"

static int install_syscall_filter(void)
{
	struct sock_filter filter[] = 
	{
		/* Validate architecture. */
		VALIDATE_ARCHITECTURE,
		/* Grab the system call number. */
		EXAMINE_SYSCALL,
		/* List allowed syscalls. */
		ALLOW_SYSCALL(exit_group),
		ALLOW_SYSCALL(exit),
		ALLOW_SYSCALL(read),
		ALLOW_SYSCALL(lseek),
		ALLOW_SYSCALL(open),
		ALLOW_SYSCALL(close),
        ALLOW_SYSCALL(write),
		KILL_PROCESS,
	};

	struct sock_fprog prog = 
	{
		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
		.filter = filter,
	};

	if ( prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) ) 
	{
		perror("prctl(NO_NEW_PRIVS)");
		goto failed;
	}
	if ( prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog) ) 
	{
		perror("prctl(SECCOMP)");
		goto failed;
	}
	return 0;

failed:
	if ( errno == EINVAL )
	{
		fprintf(stderr, "SECCOMP_FILTER is not available. :(\n");
	}

	return 1;
}

void bof() {
    char stack[0x20];
    read(0, stack, 0x100);
}

int main(int argc, char *argv[]) {
    install_syscall_filter();
    bof();
}