#pragma once
#include"Common.h"

class Portal {
	int x, y;
public:
	Portal(int x, int y): x(x), y(y){}
	int px() { return x; }
	int py() { return y; }
};
Portal portal(800, 600);

class Healing_item {
	int hx, hy;
public:
	Healing_item(int hx, int hy) : hx(hx), hy(hy) {}
	int h_x() { return hx; }
	int h_y() { return hy; }
};
std::vector<Healing_item> heal;