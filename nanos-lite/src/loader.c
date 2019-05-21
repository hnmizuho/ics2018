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
  int fd = fs_open(filename, 0, 0);
  int filesize = fs_filesz(fd);
  void *pa;
  void *va = DEFAULT_ENTRY;
  while(filesize > 0) {
	  pa = new_page();
	  _map(as, va, pa);
	  va += PGSIZE;
	  fs_read(fd, pa, PGSIZE);  
	  filesize -= PGSIZE;
  }
  return (uintptr_t)DEFAULT_ENTRY;
}
