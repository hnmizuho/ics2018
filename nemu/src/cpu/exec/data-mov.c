#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) { //压入dest
  rtl_push(&id_dest->val); 
  print_asm_template1(push);
}

make_EHelper(pop) { //弹出到dest
  rtl_pop(&t0); //这里只能弹出到rtlreg_t*里,不能直接弹到Operand*里
  operand_write(id_dest,&t0); //Operand* rtlreg_t* (decode.c)
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //CWD
    rtl_msb(&t0,&cpu.eax,2);
    if(t0 == 1)cpu.edx = cpu.edx | 0xffff;
    else cpu.edx = 0;
  }
  else {
    //CDQ
    rtl_msb(&t0,&cpu.eax,4);
    if(t0 == 1)cpu.edx = cpu.edx | 0xffffffff;
    else cpu.edx = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //CBW
    rtl_sext(&t0,&cpu.eax,1);
    cpu.eax = (cpu.eax & 0xffff0000) | (t0 & 0xffff);
  }
  else {
    //CWDE
    rtl_sext(&t0,&cpu.eax,2);
    cpu.eax = t0;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
