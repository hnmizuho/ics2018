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

// data-mov.c
make_EHelper(push);
make_EHelper(pop);

// arith.c
make_EHelper(sub); 

// logic.c
make_EHelper(xor); 

