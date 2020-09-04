#ifndef GESTO_H
#define GESTO_H

#include <Wire.h>
#include "paj7620.h"

#define GES_REACTION_TIME    500
#define GES_ENTRY_TIME      500       // 800
#define GES_QUIT_TIME     1000

extern unsigned long gesto_timeout;
extern char * gestos[10];

uint8_t gesto_begin();

uint8_t gesto_get_status();

#endif // GESTO_H
