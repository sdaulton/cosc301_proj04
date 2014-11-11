#include <ucontext.h>

#ifndef __FIFO_H__
#define __FIFO_H__

ucontext_t* fifo_pop(ucontext_t **list);
void fifo_push(ucontext_t *list, ucontext_t thread);


#endif // __FIFO_H__