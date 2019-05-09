#include "common.h"
#include "syscall.h"

static inline _RegSet* sys_none(_RegSet *r){
  SYSCALL_ARG1(r) = 1; //约定系统调用返回值存于此，即eax
  return NULL;
}
static inline _RegSet* sys_exit(_RegSet *r){
  _halt(SYSCALL_ARG2(r)); 
  return NULL;
}
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);

  switch (a[0]) {
    case SYS_none:return sys_none(r);
    case SYS_exit:return sys_exit(r);
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
