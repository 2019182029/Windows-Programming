#pragma once

#include "Common.h"

class Item{
public:
	float m_x, m_y;
	float m_y_velocity;

	bool m_is_falling;

	ITEM_TYPE m_type;
	int m_bmp_index;

public:
	Item(ITEM_TYPE type);
	Item(float x, float y, ITEM_TYPE type);

	void move();
	void update();
	void print(HDC hDC, HDC iDC) const;
};

