#pragma once

#include "Common.h"
#include "CWeapon.h"

class Player {
public:
	int m_hp;
	int m_max_hp;
	bool m_visible;
	bool m_invincible;
	std::chrono::system_clock::time_point m_invincible_time;

	float m_x, m_y;
	float m_x_velocity, m_y_velocity;

	int m_anim_state;
	
	bool m_on_platform;
	bool m_double_jump;

	bool m_is_rolling;
	bool m_was_rolling;
	std::chrono::system_clock::time_point m_rolled_time;

	Weapon* m_weapon;
	std::vector<Weapon*> m_old_weapon;

public:
	Player();

	void set_velocity(float x_velocity, float y_velocity);
	void set_on_ground(const float y_ground);
	void set_on_platform(const POINT& platform);

	void heal(int amount);
	void damaged();
	void move();
	void under_jump();
	void jump();
	void roll();
	void fire();
	void reset();
	void update();
	void print(HDC hDC, HDC pDC, HDC bDC) const;
};

