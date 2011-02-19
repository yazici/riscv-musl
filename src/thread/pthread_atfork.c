#include <pthread.h>
#include "libc.h"

static struct atfork_funcs {
	void (*prepare)(void);
	void (*parent)(void);
	void (*child)(void);
	struct atfork_funcs *prev, *next;
} *funcs;

static int lock;

static void fork_handler(int who)
{
	struct atfork_funcs *p;
	if (who < 0) {
		LOCK(&lock);
		for (p=funcs; p; p = p->next) {
			if (p->prepare) p->prepare();
			funcs = p;
		}
	} else {
		for (p=funcs; p; p = p->prev) {
			if (!who && p->parent) p->parent();
			else if (who && p->child) p->child();
			funcs = p;
		}
		UNLOCK(&lock);
	}
}

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void))
{
	struct atfork_funcs *new = malloc(sizeof *new);
	if (!new) return -1;

	LOCK(&lock);
	libc.fork_handler = fork_handler;
	new->next = funcs;
	new->prev = 0;
	new->prepare = prepare;
	new->parent = parent;
	new->child = child;
	if (funcs) funcs->prev = new;
	funcs = new;
	UNLOCK(&lock);
	return 0;
}
