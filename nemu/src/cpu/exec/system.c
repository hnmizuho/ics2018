#include "cpu/exec.h"
extern void raise_intr(uint8_t NO, vaddr_t ret_addr);
void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  rtl_li(&t0,id_dest->addr);//不是id_dest->val！！！
  rtl_li(&cpu.idtr.i_limit,vaddr_read(t0,2));
  rtl_li(&cpu.idtr.i_base,vaddr_read(t0+2,4));
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //看print_sam
  //只添加了0 3
  switch (id_dest->reg) {
	case 0:
		cpu.cr0.val = id_src->val;
		break;
	case 3:
		cpu.cr3.val = id_src->val;
		break;
	default:
		Assert(0, "Shoule reach here for NO cr%d", id_dest->reg);
		break;
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  switch (id_src->reg) {
	case 0:
		operand_write(id_dest, &cpu.cr0.val);
		break;
	case 3:
		operand_write(id_dest, &cpu.cr3.val);
		break;
	default:
		Assert(0, "Shoule reach here for NO cr%d", id_dest->reg);
		break;
  }

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //应该push的是调用int之前的，而不是此时的eip
  raise_intr(id_dest->val,decoding.seq_eip); //不是cpu.eip
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  rtl_pop(&decoding.jmp_eip);
  decoding.is_jmp = 1;
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.val);

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  t2 = pio_read(id_src->val,id_dest->width);
  operand_write(id_dest,&t2);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  pio_write(id_dest->val,id_dest->width,id_src->val);

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
