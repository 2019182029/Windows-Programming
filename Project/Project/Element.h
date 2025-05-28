#pragma once
#include"Common.h"

class Portal {
	int x, y;
public:
	Portal(int x, int y): x(x), y(y){}
	int px() const { return x; }
	int py() const { return y; }
};
Portal portal(800, 600);