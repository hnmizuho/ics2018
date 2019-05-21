#include "common.h"

#define DEFAULT_ENTRY ((void *)0x08048000)
//extern void ramdisk_read(void *buf, off_t offset, size_t len);
//extern size_t get_ramdisk_size();
extern void _map(_Protect *p, void *va, void *pa);
extern void* new_page(void);

int fs_open(const char *pathname, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  /*size_t size = get_ramdisk_size();
  void * buff = NULL;
  ramdisk_read(buff,0,size); 
  memcpy(DEFAULT_ENTRY,buff,size); //之前误用memset
  //后来才想起来，ramdisk_read已经memcpy了，上一句无用功
  return (uintptr_t)DEFAULT_ENTRY;*/

  int fd = fs_open(filename, 0, 0);
  int bytes = fs_filesz(fd);

  Log("Load [%d] %s with size: %d", fd, filename, bytes);

  void *pa,*va = DEFAULT_ENTRY;
  while(bytes>0){
  	pa = new_page(); //申请空闲物理页
  	_map(as, va, pa);//该物理页映射到用户程序虚拟地址空间
  	fs_read(fd, pa, PGSIZE);  //读一页文件到该物理页

  	va += PGSIZE;
  	bytes -= PGSIZE;
  }
  //fs_read(fd,DEFAULT_ENTRY,bytes);
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
