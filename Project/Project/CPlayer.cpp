#include "CPlayer.h"

Player::Player() {
	m_x = 0.0f; m_y = 0.0f;
	m_x_velocity = 0.0f; m_y_velocity = 0.0f;
}

void Player::set_velocity(float x_velocity, float y_velocity) {
	m_x_velocity += x_velocity;
	m_y_velocity += y_velocity;
}

void Player::set_on_platform(const POINT& platform) {
	m_y = platform.y - static_cast<float>(Bmp_Player.bmHeight);
	m_y_velocity = 0.0f;
}

void Player::move() {
	m_x += m_x_velocity;
	m_y += m_y_velocity;
}

void Player::update() {
	move();

	// Clamp Gravity
	m_y_velocity = min(MAX_GRAVITY, m_y_velocity + 0.1f);
}
