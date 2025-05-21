#include "CPlayer.h"

Player::Player() {
	m_x = 250.0f; m_y = 0.0f;
	m_x_velocity = 0.0f; m_y_velocity = 0.0f;

	m_anim_state = IDLE;

	m_on_platform = false;
	m_double_jump = true;

	m_is_rolling = false;
	m_was_rolling = false;

	m_weapon = new Weapon(SHOTGUN);
}

void Player::set_velocity(float x_velocity, float y_velocity) {
	m_x_velocity += x_velocity;
	m_y_velocity += y_velocity;
}

void Player::set_on_platform(const POINT& platform) {
	m_y = platform.y - Bmp_Player[m_anim_state].bmHeight;
	m_y_velocity = 0.0f;

	m_on_platform = true;
	m_double_jump = true;
}

void Player::move() {
	m_x += m_x_velocity;
	m_y += m_y_velocity;
}

void Player::under_jump() {
	m_y += 2.0f;
}

void Player::jump() {
	if (m_on_platform) {
		m_y_velocity = -12.5f;
	} else if (m_double_jump) {
		m_y_velocity = -12.5f;
		m_double_jump = false;
	}
}

void Player::roll() {
	if (!m_is_rolling) {
		if (0.0f < m_x_velocity) {
			m_x_velocity = 12.5f;

			m_anim_state = DODGE;

			m_is_rolling = true;
			m_was_rolling = true;
		} else if (0.0f > m_x_velocity) {
			m_x_velocity = -12.5f;

			m_anim_state = DODGE;

			m_is_rolling = true;
			m_was_rolling = true;
		}
	}
}

void Player::fire() {
	if ((IDLE == m_anim_state) || (DODGE == m_anim_state)) {
		return;
	}

	if (m_weapon) {
		m_weapon->fire(
			m_x + (Bmp_Player[m_anim_state].bmWidth / 2) - 5,
			m_y + (Bmp_Player[m_anim_state].bmHeight / 2) - 7,
			m_anim_state);
	}
}

void Player::update() {
	// Roll
	if (m_is_rolling) {
		m_x_velocity *= 0.95f;

		if (5.0f > abs(m_x_velocity)) {
			if (0.0f < m_x_velocity) {
				m_anim_state = RIGHT;
			} else if (0.0f > m_x_velocity) {
				m_anim_state = LEFT;
			}

			m_x_velocity = 0.0f;
			m_is_rolling = false;
		}
	}

	// Gravity
	m_y_velocity = min(MAX_GRAVITY, m_y_velocity + 0.5f);

	move();

	// Weapon
	if (m_weapon) {
		if (m_weapon->m_burst_count) {
			m_weapon->m_bullets.emplace_back(
				m_x + (Bmp_Player[m_anim_state].bmWidth / 2) - 5,
				m_y + (Bmp_Player[m_anim_state].bmHeight / 2) - 7,
				m_anim_state);
			--m_weapon->m_burst_count;
		}

		m_weapon->update();
	}
}

void Player::print(HDC hDC, HDC pDC, HDC bDC) const {
	SelectObject(pDC, Pic_Player[m_anim_state]);
	TransparentBlt(hDC, static_cast<int>(m_x), static_cast<int>(m_y), Bmp_Player[m_anim_state].bmWidth, Bmp_Player[m_anim_state].bmHeight,
		pDC, 0, 0, Bmp_Player[m_anim_state].bmWidth, Bmp_Player[m_anim_state].bmHeight, RGB(255, 255, 255));

	// Weapon
	if (m_weapon) {
		m_weapon->print(hDC, bDC);
	}
}
