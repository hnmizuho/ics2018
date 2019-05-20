#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

//这些函数体分散在exec文件夹下不同.c里
//没实现的先不声明 因为里面有坑人的 TODO()

// control.c
make_EHelper(call);
make_EHelper(ret); 
make_EHelper(jcc); 
make_EHelper(jmp); 
make_EHelper(call_rm);
make_EHelper(jmp_rm);
// data-mov.c
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(movsx);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(leave);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(mov_cr2r);
make_EHelper(mov_r2cr);
// arith.c
make_EHelper(sub); 
make_EHelper(add); 
make_EHelper(adc); 
make_EHelper(sbb); 
make_EHelper(cmp); 
make_EHelper(dec);
make_EHelper(inc);
make_EHelper(imul2);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(idiv);
make_EHelper(div);
make_EHelper(neg);
// logic.c
make_EHelper(xor); 
make_EHelper(and); 
make_EHelper(or); 
make_EHelper(setcc); 
make_EHelper(test); 
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(not);
make_EHelper(shr);
make_EHelper(rol);
// special.c
make_EHelper(nop); 

// system.c
make_EHelper(in); 
make_EHelper(out); 
make_EHelper(lidt);
make_EHelper(int); 
make_EHelper(iret); 
