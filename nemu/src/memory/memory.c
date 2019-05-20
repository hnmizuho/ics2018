#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];//8b*128*1024*1024 = 128MB

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int port;
  if((port = is_mmio(addr))!=-1)
      return mmio_read(addr,len,port);
  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int port;
  if((port = is_mmio(addr))!=-1)
      mmio_write(addr,len,data,port);
  else
      memcpy(guest_to_host(addr), &data, len);
}

/*uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  paddr_write(addr, len, data);
}*/
paddr_t page_translate(vaddr_t addr, bool w1r0) {
  //aka page_walk
if(cpu.cr0.protect_enable);
return addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if ((((addr ^ (addr + len)) & 0x003ff000) != 0) && (((addr + len) & 0x00000fff) != 0)) {
	//data cross the page boundary
	assert(0);
  } else {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if ((((addr ^ (addr + len)) & 0x003ff000) != 0) && (((addr + len) & 0x00000fff) != 0)) {
	//data cross the page boundary
	assert(0);
  } else {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
}
