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
	PDE *pgdir = p->ptr;
	PDE *pde = &pgdir[PDX(va)];
	PTE *pgtab;
	if (*pde & PTE_P) { //present
		pgtab = (PTE *)PTE_ADDR(*pde);
	} else {
		//映射过程中发现需要申请新的页表
		pgtab = (PTE *)palloc_f();
		*pde = PTE_ADDR(pgtab) | PTE_P;
	}
	pgtab[PTX(va)] = PTE_ADDR(pa) | PTE_P;
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  //_umake将ustack底部初始化一个entry为返回地址的陷阱帧
  uint32_t *ptr = ustack.end;
  //navyapps程序入口函数_start的 栈帧，即8个通用寄存器
  for (int i = 0; i < 8; i++) {
	*ptr = 0x0; 
  	 ptr--;
  }
  //陷阱帧，包括栈帧的8个通用寄存器
  *ptr = 0x202; 	  ptr--; //eflags,即IF置1即可
  *ptr = 0x8; 	          ptr--; //cs 为了diff test
  *ptr = (uint32_t)entry; ptr--; //eip
  *ptr = 0x0;             ptr--; //error code
  *ptr = 0x81;            ptr--; //irq id
  for (int i = 0; i < 8; i++) {
	*ptr = 0x0;
  	 ptr--;
  }
  ptr++;
  return (_RegSet *)ptr; //将会记录到tf
}
