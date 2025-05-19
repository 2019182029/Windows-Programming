#pragma once

#include "Common.h"

#define PLAYER_MOVE 0

constexpr float MAX_GRAVITY = 9.8f;

class Player {
public:
	float m_x, m_y;
	float m_x_velocity, m_y_velocity;

public:
	Player();

	void set_velocity(float x_velocity, float y_velocity);
	void set_on_platform(const POINT& platform);

	void move();
	void update();
};

