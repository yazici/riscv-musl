#include <unistd.h>
#include "syscall.h"
#include "libc.h"

int pause(void)
{
	int r;
	CANCELPT_BEGIN;
	r = syscall(SYS_pause);
	CANCELPT_END;
	return r;
}
