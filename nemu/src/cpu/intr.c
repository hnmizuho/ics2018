#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags.val);

  //rtl_li(&t0,1);
  //rtl_set_IF(&t0);

  rtl_push(&cpu.cs);
  //rtl_push(&cpu.eip);
  rtl_push(&ret_addr);
  

}

void dev_raise_intr() {
}
