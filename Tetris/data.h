#pragma once

#pragma once
#include <cstdint>
#include <cstdlib>
#include <ctime>
using u32 = uint32_t;
using s32 = int32_t;
using u8 = uint8_t;
using s8 = int8_t;
inline extern const s32 CELL_PIXCEL_SIZE{ 30 };
inline extern const s32 MATRIX_WIDTH{ 10 };
inline extern const s32 MATRIX_HEIGHT{ 40 };
inline extern const s32 VISIBLE_MATRIX_WIDTH{ 10 };
inline extern const s32 VISIBLE_MATRIX_HEIGHT{ 20 };
inline extern const s32 WINDOW_PIXEL_HEIGHT{ CELL_PIXCEL_SIZE * VISIBLE_MATRIX_HEIGHT };
inline extern const s32 WINDOW_PIXEL_WIDTH{ CELL_PIXCEL_SIZE * VISIBLE_MATRIX_WIDTH*2 };
inline extern const s32 tetroI[]
{
	  0, 0, 0, 0,
	  0, 0, 0, 0,
	  1, 1, 1, 1,
	  0, 0, 0, 0,
};

inline extern const s32 tetroT[]
{
	0, 0, 0,
	2, 2, 2,
	0, 2, 0
};
inline extern const s32 tetroO[]
{
	 3, 3,
	 3, 3
};
inline extern const s32 tetroJ[]
{
	0, 0, 0,
	4, 0, 0,
	4, 4, 4
};
inline extern const s32 tetroL[]
{
	0, 0, 0,
	0, 0, 5,
	5, 5, 5
};
inline extern const s32 tetroS[]
{
	0, 0, 0,
	0, 6, 6,
	6, 6, 0
};
inline extern const s32 tetroZ[]
{
	0, 0, 0,
	7, 7, 0,
	0, 7, 7
};


inline extern const s32* tetrorino[]
{
	nullptr, tetroI, tetroT, tetroO, tetroJ, tetroL, tetroS, tetroZ
};

class Input
{
public:
	u8 left;
	u8 right;
	u8 up;
	u8 down;

	u8 space;
	u32 pressTime;

	s8 dleft;
	s8 dright;
	s8 dup;
	s8 ddown;
	s8 dspace;
};