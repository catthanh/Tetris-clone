#include "Tetromino.h"
#include "data.h"
#include <cstdlib>
#include <iostream>
#include <ctime>



Tetromino::Tetromino() :
	type_{ static_cast<Type>(1) },
	offset_col_{ 3 },
	offset_row_{ -1 },
	orientation_{ 0 }
{

}

s32 Tetromino::size() const
{
	switch (type_)
	{
	case Type::I:
	{
		return 5;
	}
	case Type::T:
	{
		return 3;
	}
	case Type::O:
	{
		return 2;
	}
	case Type::J:
	{
		return 3;
	}
	case Type::L:
	{
		return 3;
	}
	case Type::S:
	{
		return 3;
	}
	case Type::Z:
	{
		return 3;
	}
	default:
		break;
	}
	return 0;
}

void Tetromino::move(s32 col, s32 row)
{
	offset_col_ += col;
	offset_row_ += row;


}

void Tetromino::rotate(s32 i)
{
	if (i > 0)
		++orientation_ %= 4;
	else
		if (i < 0)
			(orientation_ += 3) %= 4;
		else return;

}

void Tetromino::set_type(Type type)
{
	type_ = type;
}


s32 Tetromino::tetromino_get(s32 col, s32 row)
{
	//get current data
	const s32* data = tetrorino[static_cast<s32>(type())];
	s32 ssize{ size() };

	switch (orientation())
	{
	case 0: //0deg
	{
		return data[col + row * ssize];
		break;
	}
	case 1: //90deg
	{
		return data[(ssize - col - 1) * ssize + row];
		break;
	}
	case 2: //180deg
	{
		return data[(ssize - 1) - col + ((ssize - 1) - row) * ssize];
		break;
	}
	case 3: //270deg
	{
		return data[col * ssize + (ssize - row - 1)];
		break;
	}
	default:
		return 0;
		break;
	}
}

