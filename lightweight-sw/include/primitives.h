#ifndef _PRIMITIVES_H_INCLUDED_
#define _PRIMITIVES_H_INCLUDED_

#define ROTATE_LEFT(x, amount, dsize)                                          \
  ((x << amount) | (x >> ((dsize * 8) - amount)))
#define ROTATE_RIGHT(x, amount, dsize)                                         \
  ((x >> amount) | ((x & ((1 << amount) - 1) << ((dsize * 8) - amount))))

#endif
