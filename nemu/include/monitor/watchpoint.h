#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char eexpr[100];
  uint32_t init;

} WP;

WP* new_wp();
void free_wp(int N);
void show_wp();
bool check_wp();
#endif
