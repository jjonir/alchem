#ifndef _SPACE_H
#define _SPACE_H

#include <stdint.h>

typedef struct position {
	uint8_t x, y, z;
} position_t;

#define SET_POS(P,X,Y,Z) do { \
	P.x = X; P.y = Y; P.z = Z; \
} while(0)

#define POS_EQ(P1,P2) \
	((P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z))

#define POS_SUB(LV,P1,P2) do { \
	LV.x = P1.x - P2.x; \
	LV.y = P1.y - P2.y; \
	LV.z = P1.z - P2.z; \
} while(0)

#define POS_ADD(LV,P1,P2) do { \
	LV.x = P1.x + P2.x; \
	LV.y = P1.y + P2.y; \
	LV.z = P1.z + P2.z; \
} while(0)

static inline position_t pos(uint8_t x, uint8_t y, uint8_t z) {
	position_t rv;
	rv.x = x; rv.y = y; rv.z = z;
	return rv;
}

typedef enum orientation {
	PX,PY,PZ,NX,NY,NZ
} orientation_t;

#endif
