#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

void* new_page(void);

int fs_open(const char *pathname, int flags, int mode);
size_t fs_filesz(int fd);
ssize_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  /*size_t size = get_ramdisk_size();
  void * buff = NULL;
  ramdisk_read(buff,0,size); 
  memcpy(DEFAULT_ENTRY,buff,size); //之前误用memset
  //后来才知道，ramdisk_read已经memcpy了，上一句无用功
  return (uintptr_t)DEFAULT_ENTRY;*/
  int fd = fs_open(filename, 0, 0);
  size_t nbyte = fs_filesz(fd);
  void *pa;
  void *va;

  Log("loaded: [%d]%s size:%d", fd, filename, nbyte);

  void *end = DEFAULT_ENTRY + nbyte;
  for (va = DEFAULT_ENTRY; va < end; va += PGSIZE) {
	  pa = new_page();
	  Log("Map va to pa: 0x%08x to 0x%08x", va, pa);
	  _map(as, va, pa);
  	  fs_read(fd, pa, (end - va) < PGSIZE ? (end - va) : PGSIZE);
  }

  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
