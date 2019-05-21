#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
//printf("va: 0x%-8x pa: 0x%-8x\n", (uintptr_t)va, (uintptr_t)pa);
	int pde_index = PDX(va);
	//printf("pde_index: %d\n", pde_index);
	int pte_index = PTX(va);
	//printf("pte_idex: %d\n", pte_index);
	PDE * updir = (PDE *)(p->ptr);
	PTE * uptab;
	if ((updir[pde_index] & 0x1) == 0){
		uptab = (PTE *)(void *)(palloc_f());
		//printf("uptab: 0x%-8x\n", (uintptr_t)uptab);
		updir[pde_index] = (uintptr_t)uptab | PTE_P;
		//printf("updir[%d] = 0x%-8x\n", pde_index, (uintptr_t)(updir[pde_index]));
	} else {
		uptab = (PTE *)(void *)((uintptr_t)updir[pde_index] & 0xfffff000);
		//printf("uptab: 0x%-8x\n", (uintptr_t)uptab);
		//printf("updir[%d] = 0x%-8x\n", pde_index, (uintptr_t)(updir[pde_index]));
	}

	PTE pte = ((uintptr_t)pa & 0xfffff000) | PTE_P;
	//printf("pte: 0x%-8x\n", pte);
	uptab[pte_index] = pte;
	//printf("uptab[%d] = 0x%-8x\n", pte_index-1, (uintptr_t)(uptab[pte_index-1]));
	//printf("uptab[%d] = 0x%-8x\n", pte_index, (uintptr_t)(uptab[pte_index]));
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  return NULL;
}
