#pragma once

#include "stdafx.h"

class Piece
{
private:
	int type;
	int color;
	int xCoord;
	int yCoord;
public:
	Piece(int type, int color, int xCoord, int yCoord) : type(type), color(color), xCoord(xCoord), yCoord(yCoord) {}
	int GetType();
	int GetColor();
	int GetXcoord();
	int GetYcoord();
};