#pragma once

#ifdef DEBUG
  #include <stdio.h>
#endif

#ifdef DEBUG
  #define DPRINT(...) printf(__VA_ARGS__)
#else
  #define DPRINT(...) do {} while (0)
#endif

#define CLOCK_PERIOD_NSEC 200000000

#define SET_BIT(x, y) x |= (0x01 << y)
#define GET_BIT(x, y) (x >> y) & 0x01

typedef enum bool {
    false = 0,
    true = 1
} bool;
