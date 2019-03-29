#include <sys/types.h>

/* brk is handled entirely within the C library.  This limits METAL programs that
 * use the C library to be disallowed from dynamically allocating memory
 * without talking to the C library, but that sounds like a sane way to go
 * about it.  Note that there is no error checking anywhere in this file, users
 * will simply get the relevant error when actually trying to use the memory
 * that's been allocated. */
extern char __los_heap_addr_start__;
extern char __los_heap_addr_end__;
static char *brk = &__los_heap_addr_start__;

int
_brk(void *addr)
{
  brk = addr;
  return 0;
}

char *
_sbrk(ptrdiff_t incr)
{
  char *old = brk;

  /* If __heap_size == 0, we can't allocate memory on the heap */
  if(&__los_heap_addr_start__ == &__los_heap_addr_end__) {
    return NULL;
  }

  /* Don't move the break past the end of the heap */
  if ((brk + incr) < &__los_heap_addr_end__) {
    brk += incr;
  } else {
    brk = &__los_heap_addr_end__;
  }

  return old;
}
