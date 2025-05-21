#pragma once

#include "Common.h"

class Bullet {
public:
	float m_x, m_y;
	float m_x_velocity, m_y_velocity;

	float m_distance;

public:
	Bullet();
	Bullet(float x, float y, int dir);

	void move();
	void update();
	void print(HDC hDC, HDC bDC) const;
};

class Weapon{
public:
	int m_type;

	int m_rounds;
	float m_attack;

	float m_range;
	float m_cooldown;

	std::vector<Bullet> m_bullets;

public:
	Weapon(int type);

	void fire(int x, int y, int dir);
	void update();
	void print(HDC hDC, HDC bDC) const;
};

