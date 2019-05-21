#include "proc.h"
#include "memory.h"

static void *pf = NULL;

void* new_page(void) {
  assert(pf < (void *)_heap.end);
  void *p = pf;
  pf += PGSIZE;
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uint32_t new_brk) {
	 //Log("cur_brk:0x%-8x  max_brk:0x%-8x  new_brk:0x%-8x", current->cur_brk, current->max_brk, new_brk);
  if (current->cur_brk == 0) {
	current -> cur_brk = new_brk;
	current -> max_brk = (new_brk & 0xfffff000) + 0x1000;
  } else {
	if (new_brk > current->max_brk) {
	  int len = new_brk - current->max_brk;
	  while (len > 0) {
		void *pa = new_page();
		_map(&current->as, (void *)current->max_brk, pa);
		current->max_brk += PGSIZE;
		len -= PGSIZE;
	  }	  
	}
	current->cur_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _pte_init(new_page, free_page);
}
