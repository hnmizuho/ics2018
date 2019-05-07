#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0,&id_dest->val,&id_src->val);//手册为dest:=src and src2
  //operand_write(id_dest,&t0); 
  rtl_update_ZFSF(&t0,id_dest->width); //手册的“操作”一栏没让做，但是需要做
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);

  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);

  rtl_update_ZFSF(&t2,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t3,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t3);

  rtl_update_ZFSF(&t3,id_dest->width);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t2,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t2);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);

  operand_write(id_dest, &t2);
  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtl_mv(&t1,&id_dest->val);
  rtl_not(&t1);
  operand_write(id_dest,&t1);

  print_asm_template1(not);
}
//源代码框架没有这个
make_EHelper(rol) {
  t0 = id_src->val;
  t1 = 1;
  //id_dest->val = id_src->val;
  for(int i=0;i<t0;i++)
  {
      rtl_msb(&t2,&id_dest->val,id_dest->width);
      rtl_shl(&id_dest->val,&id_dest->val,&t1); //之前写成shr
      id_dest->val += t2;
      rtl_set_CF(&t2);
  }
  //if(t0 == 1)
  //    rtl_set_OF(&t1);
  rtl_xor(&t0, &id_dest->val, &id_src->val); //代码复用 OF
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);
  print_asm_template1(not);
}
