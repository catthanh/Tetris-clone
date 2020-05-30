
#pragma once
#include "data.h"


enum class Type { I = 1, T, O, J, L, S, Z };

class Tetromino
{

public:
	Tetromino();
	s32 offset_col() const { return offset_col_; };
	s32 offset_row() const { return offset_row_; };
	s32 orientation() const { return orientation_; };
	Type type() const { return type_; };
	s32 size() const;
	s32 tetromino_get(s32 col, s32 row);
	void move(s32 col, s32 row);
	void rotate(s32 i);
	void set_type(Type type);
private:
	Type type_;
	s32 offset_col_;
	s32 offset_row_;
	s32 orientation_;

};


