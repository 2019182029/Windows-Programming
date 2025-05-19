#include "CPlayer.h"

Player::Player() {
	m_x = 0.0f; m_y = 0.0f;
	m_x_velocity = 0.0f; m_y_velocity = 0.0f;

	m_anim_index = IDLE;

	m_on_platform = false;
	m_double_jump = true;

	m_is_rolling = false;
	m_was_rolling = false;
}

void Player::set_velocity(float x_velocity, float y_velocity) {
	m_x_velocity += x_velocity;
	m_y_velocity += y_velocity;
}

void Player::set_on_platform(const POINT& platform) {
	m_y = platform.y - Bmp_Player[m_anim_index].bmHeight;
	m_y_velocity = 0.0f;

	m_on_platform = true;
	m_double_jump = true;
}

void Player::move() {
	m_x += m_x_velocity;
	m_y += m_y_velocity;
}

void Player::jump() {
	if (m_on_platform) {
		m_y_velocity = -12.5f;
		m_on_platform = false;
	} else if (m_double_jump) {
		m_y_velocity = -12.5f;
		m_double_jump = false;
	}
}

void Player::roll() {
	if (!m_is_rolling) {
		if (0.0f < m_x_velocity) {
			m_x_velocity = 7.5f;

			m_anim_index = DODGE;

			m_is_rolling = true;
			m_was_rolling = true;
		} else if (0.0f > m_x_velocity) {
			m_x_velocity = -7.5f;

			m_anim_index = DODGE;

			m_is_rolling = true;
			m_was_rolling = true;
		}
	}
}

void Player::update() {
	move();

	// Roll
	if (m_is_rolling) {
		m_x_velocity *= 0.99f;

		if (5.0f > abs(m_x_velocity)) {
			if (0.0f < m_x_velocity) {
				m_anim_index = RIGHT;
			} else if (0.0f > m_x_velocity) {
				m_anim_index = LEFT;
			}

			m_x_velocity = 0.0f;
			m_is_rolling = false;
		}
	}

	// Gravity
	m_y_velocity = min(MAX_GRAVITY, m_y_velocity + 0.5f);
}
