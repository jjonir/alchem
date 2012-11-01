#ifndef _SPACE_H
#define _SPACE_H

#include <stdint.h>

typedef struct position {
	uint8_t x, y, z;
} position_t;

typedef enum orientation {
	PX,PY,PZ,NX,NY,NZ,ORIENTATION_NUM
} orientation_t;

#define for_each_position(P, X, Y, Z) \
	for (P.z = 0; P.z < Z; P.z++) \
		for (P.y = 0; P.y < Y; P.y++) \
			for (P.x = 0; P.x < X; P.x++)

#define SET_POS(P, X, Y, Z) do { \
	P.x = X; \
	P.y = Y; \
	P.z = Z; \
} while (0)

#define SET_POS_V(TO, FROM) do { \
	TO.x = FROM.x; \
	TO.y = FROM.y; \
	TO.z = FROM.z; \
} while (0)

#define POS_EQ(P1, P2) \
	((P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z))

#define POS_SUB(LV, P1, P2) do { \
	LV.x = P1.x - P2.x; \
	LV.y = P1.y - P2.y; \
	LV.z = P1.z - P2.z; \
} while (0)

#define POS_ADD(LV, P1, P2) do { \
	LV.x = P1.x + P2.x; \
	LV.y = P1.y + P2.y; \
	LV.z = P1.z + P2.z; \
} while (0)

static inline position_t pos(uint8_t x, uint8_t y, uint8_t z)
{
	position_t rv;
	rv.x = x; rv.y = y; rv.z = z;
	return rv;
}

static inline int adjacent(struct position p1, struct position p2)
{
	return (((p1.x == p2.x) && (p1.y == p2.y) && (p1.z != p2.z)) ||
		((p1.x == p2.x) && (p1.y != p2.y) && (p1.z == p2.z)) ||
		((p1.x != p2.x) && (p1.y == p2.y) && (p1.z == p2.z)));
}

static inline struct position pos_shift(enum orientation o, uint8_t dist)
{
	struct position dp = {0, 0, 0};
	switch (o) {
	case PX:
		dp.x = dist;
		break;
	case NX:
		dp.x = -dist;
		break;
	case PY:
		dp.y = dist;
		break;
	case NY:
		dp.y = -dist;
		break;
	case PZ:
		dp.z = dist;
		break;
	case NZ:
		dp.z = -dist;
		break;
	default: break;
	}

	return dp;
}

static inline enum orientation opposite(enum orientation o)
{
	switch (o) {
	case PX: return NX;
	case NX: return PX;
	case PY: return NY;
	case NY: return PY;
	case PZ: return NZ;
	case NZ: return PZ;
	default: return ORIENTATION_NUM;
	}
}

/*
 * +Z: x->+y, y->-x
 * +Y: x->+z, z->-x
 * +X: y->+z, z->-y
 */
static inline enum orientation rotated(enum orientation o, enum orientation dir)
{
	const enum orientation rot_tab[ORIENTATION_NUM][ORIENTATION_NUM] = {
		{PX, PX, PZ, NZ, PY, NY}, // PX
		{NX, NX, NZ, PZ, NY, PY}, // NX
		{PZ, NZ, PY, PY, NX, PX}, // PY
		{NZ, PZ, NY, NY, PX, NX}, // NY
		{NY, PY, NX, PX, PZ, PZ}, // PZ
		{PY, NY, PX, NX, NZ, NZ}  // NZ
	};

	return rot_tab[o][dir];
}

#endif
