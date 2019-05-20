#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "memory/mmu.h"
enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
  /* Do NOT change the order of the GPRs' definitions. */
  /* In NEMU, rtlreg_t is exactly uint32_t. This makes RTL instructions
   * in PA2 able to directly access these registers.
   */
  union{
    union{
      uint32_t _32;
      uint16_t _16;
      uint8_t _8[2];
    } gpr[8];
    struct{
      rtlreg_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    };
  };
  vaddr_t eip;

  union{
      uint32_t val;//和下面的32bits占用同一片空间，整个eflags的值
      struct{
          uint32_t CF:1;
          unsigned:5;
          uint32_t ZF:1;
          uint32_t SF:1;
          unsigned:1;
          uint32_t IF:1;
          unsigned:1;
          uint32_t OF:1;
          unsigned:20;
      }; //这个结构占用32bits
  }eflags;//32bits寄存器

  struct{
      uint32_t i_limit;  //IDT数组长度 实际为16位 为了方便
      uint32_t i_base; //IDT数组基址
  }idtr;
  uint32_t cs;//实际16位 参照GDB remote protocol client

  /*union{
      uint32_t cr0;
      struct{
          uint32_t PE:1;
          uint32_t MP:1;
          uint32_t EM:1;
          uint32_t TS:1;
          uint32_t ET:1;
          unsigned   :26;
          uint32_t PG:1;
      }; 
  };
  uint32_t cr3;*/
  CR0 cr0;
  CR3 cr3;

  bool INTR;

} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 8);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

static inline const char* reg_name(int index, int width) {
  assert(index >= 0 && index < 8);
  switch (width) {
    case 4: return regsl[index];
    case 1: return regsb[index];
    case 2: return regsw[index];
    default: assert(0);
  }
}

#endif
