#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags.val);

  //rtl_li(&t0,1);
  //rtl_set_IF(&t0);
  cpu.eflags.IF = 0;

  rtl_push(&cpu.cs);
  //rtl_push(&cpu.eip);
  rtl_push(&ret_addr);
  
  //cpu.cs = ??
  rtl_li(&t0,vaddr_read(cpu.idtr.i_base+4*NO,4));
  //if((t0 & 0x00008000) == 0)
  //    assert(0);
  //cpu.eip = ret_addr + (t0 & 0x00001111);
  decoding.jmp_eip = t0&0xffff;
  //rtl_addi(&decoding.jmp_eip,&ret_addr,t0);
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
}
