#include "common.h"
#include "syscall.h"

static inline _RegSet* sys_none(_RegSet *r){
  SYSCALL_ARG1(r) = 1; //约定系统调用返回值存于此，即eax
  return NULL;
}
static inline _RegSet* sys_exit(_RegSet *r){
  //接受一个退出状态的参数，顺序ebx ecx edx
  _halt(SYSCALL_ARG2(r)); 
  return NULL;
}
static inline _RegSet* sys_write(_RegSet *r){
  /*int fd = (int)SYSCALL_ARG2(r);
  char *buf = (char *)SYSCALL_ARG3(r);
  int len = (int)SYSCALL_ARG4(r);

  if(fd == 1 || fd == 2){
      for(int i = 0; i < len; i++) {
          _putc(buf[i]);
      }
      SYSCALL_ARG1(r) = SYSCALL_ARG4(r);
  }
  return NULL;*/
	if(SYSCALL_ARG2(r) == 1 || SYSCALL_ARG2(r) == 2){
		for(int i = 0; i < SYSCALL_ARG4(r); i++) {
			_putc(((char*)SYSCALL_ARG3(r))[i]);
		}
		SYSCALL_ARG1(r) = SYSCALL_ARG4(r);
	}
	return NULL;
}
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
printf("%d",a[0]);
  switch (a[0]) {
    case SYS_none:return sys_none(r);
    case SYS_exit:return sys_exit(r);
    case SYS_write:return sys_write(r);
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
