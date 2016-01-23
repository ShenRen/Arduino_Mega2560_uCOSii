
#ifndef _UCOS_H_
#define _UCOS_H_


//#include <arduino.h>
#include "ucos_ii.h"
#include "os_cpu_i.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void  SYSTEM_TickInit (void);
#endif
