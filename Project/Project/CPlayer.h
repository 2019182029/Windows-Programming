#pragma once

#include "Common.h"
#include "CWeapon.h"

class Player {
public:
	float m_x, m_y;
	float m_x_velocity, m_y_velocity;

	int m_anim_state;
	
	bool m_on_platform;
	bool m_double_jump;

	bool m_is_rolling;
	bool m_was_rolling;

	Weapon* m_weapon;

public:
	Player();

	void set_velocity(float x_velocity, float y_velocity);
	void set_on_platform(const POINT& platform);

	void move();
	void under_jump();
	void jump();
	void roll();
	void fire();
	void update();
	void print(HDC hDC, HDC pDC, HDC bDC) const;
};

