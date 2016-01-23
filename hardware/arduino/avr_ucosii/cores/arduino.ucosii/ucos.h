
#ifndef _UCOS_H_
#define _UCOS_H_


//#include <Arduino.h>
#include <uCOS_2_91\ucos_ii.h>
#include <uCOS_2_91\os_cpu_i.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void  SYSTEM_TickInit (void);
#endif
