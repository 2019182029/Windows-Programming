#pragma once

#include "Common.h"

class Bullet {
public:
	float m_x, m_y;
	float m_x_velocity, m_y_velocity;

	float m_distance;

	bool m_hit;
	std::chrono::system_clock::time_point m_hit_time;

public:
	Bullet();
	Bullet(float x, float y, int dir, float speed = 10.0f);
	Bullet(float x, float y, float angle);

	void hit();
	void move();
	void update();
	void print(HDC hDC, HDC bDC) const;
};

class Weapon{
public:
	int m_type;

	int m_rounds;
	int m_attack;

	float m_range;
	float m_cooldown;
	std::chrono::system_clock::time_point m_shot_time;

	int m_burst_count;

	std::vector<Bullet> m_bullets;

public:
	Weapon(int type);

	void fire(int x, int y, int dir);
	void update();
	void print(HDC hDC, HDC bDC) const;
};

