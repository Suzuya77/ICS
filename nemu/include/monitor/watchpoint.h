#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char EXPR[32];
  int oldValue, newValue;

} WP;

bool watchpointMonitor();
#endif
