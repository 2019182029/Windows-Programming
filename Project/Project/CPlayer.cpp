#include "CPlayer.h"

Player::Player() {
	m_hp = 5;
	m_max_hp = 5;
	m_visible = true;
	m_invincible = false;
	m_invincible_time = std::chrono::system_clock::now();

	m_x = 250.0f; m_y = 0.0f;
	m_x_velocity = 0.0f; m_y_velocity = 0.0f;

	m_anim_state = IDLE;

	m_on_platform = false;
	m_double_jump = true;

	m_is_rolling = false;
	m_was_rolling = false;
	m_rolled_time = std::chrono::system_clock::now();

	m_weapon = new Weapon(PISTOL);
}

void Player::set_velocity(float x_velocity, float y_velocity) {
	m_x_velocity += x_velocity;
	m_y_velocity += y_velocity;
}

void Player::set_on_ground(const float y_ground) {
	m_y = y_ground;
	m_y_velocity = 0.0f;

	m_on_platform = true;
	m_double_jump = true;
}

void Player::set_on_platform(const POINT& platform) {
	m_y = platform.y - Bmp_Player[m_anim_state].bmHeight;
	m_y_velocity = 0.0f;

	m_on_platform = true;
	m_double_jump = true;
}

void Player::heal(int amount) {
	m_hp = min(m_hp + amount, m_max_hp);
}

void Player::damaged() {
	m_hp = max(m_hp - 1, 0);
	m_invincible = true;
	m_invincible_time = std::chrono::system_clock::now();
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
	if (1 > std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_rolled_time).count()) {
		return;
	}

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

		m_rolled_time = std::chrono::system_clock::now();
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

		if (0 == m_weapon->m_rounds) {
			m_old_weapon.emplace_back(m_weapon);
			m_weapon = new Weapon(PISTOL);
		}
	}
}

void Player::reset() {
	m_hp = m_max_hp;
	m_visible = true;
	m_invincible = false;

	m_x = 250.0f;
	m_y = 200.0f;
	m_x_velocity = 0.0f;
	m_y_velocity = 0.0f;

	m_anim_state = IDLE;

	m_on_platform = true;
	m_double_jump = true;

	m_is_rolling = false;
	m_was_rolling = true; // 이거 true로 해야 방향키 입력한 채 죽어도 정상 조작이 가능합니다. (false면 죽은 후 한쪽 방향으로 쭉 자동 이동)

	Weapon* old_weapon = m_weapon;
	m_weapon->m_bullets.clear();
	m_weapon = new Weapon(PISTOL);
	delete old_weapon;

	for (auto& weapon : m_old_weapon) {
		weapon->m_bullets.clear();
		delete weapon;
	}
	m_old_weapon.clear();
}

void Player::update() {
	// Invincible
	if (m_invincible) {
		auto now = std::chrono::system_clock::now();

		if (1000 < std::chrono::duration_cast<std::chrono::milliseconds>(now - m_invincible_time).count()) {
			m_visible = true;
			m_invincible = false;
		} else {
			m_visible = !m_visible;
		}
	}

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
			--m_weapon->m_rounds;
			--m_weapon->m_burst_count;

			if (0 == m_weapon->m_rounds) {
				m_old_weapon.emplace_back(m_weapon);
				m_weapon = new Weapon(PISTOL);
			}
		}

		m_weapon->update();
	}

	if (!m_old_weapon.empty()) {
		for (auto iter = m_old_weapon.begin(); iter != m_old_weapon.end();) {
			(*iter)->update();

			if (0 == (*iter)->m_bullets.size()) {
				delete (*iter);
				iter = m_old_weapon.erase(iter);
				continue;
			}
			++iter;
		}
	}
}

void Player::print(HDC hDC, HDC pDC, HDC bDC) const {
	if (m_visible) {
		SelectObject(pDC, Pic_Player[m_anim_state]);
		TransparentBlt(hDC, static_cast<int>(m_x), static_cast<int>(m_y), Bmp_Player[m_anim_state].bmWidth, Bmp_Player[m_anim_state].bmHeight,
			pDC, 0, 0, Bmp_Player[m_anim_state].bmWidth, Bmp_Player[m_anim_state].bmHeight, RGB(255, 255, 255));
	}

	// Weapon
	if (m_weapon) {
		m_weapon->print(hDC, bDC);
	}

	if (!m_old_weapon.empty()) {
		for (const auto& old_weapon : m_old_weapon) {
			old_weapon->print(hDC, bDC);
		}
	}

	// UI - Weapon
	HPEN hPen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	if (m_weapon) {
		RoundRect(hDC, 5, 5, 55, 55, 10, 10);

		SelectObject(WeaponDC, Pic_Weapon[m_weapon->m_type]);
		TransparentBlt(hDC, 5, 5, 50, 50,
			WeaponDC, 0, 0, Bmp_Weapon[m_weapon->m_type].bmWidth, Bmp_Weapon[m_weapon->m_type].bmHeight, RGB(255, 255, 255));

		RECT Rect = { 5, 30, 50, 50 };
		std::wstring Text;

		if (m_weapon->m_type == PISTOL) {
			Text = L"∞";
		}
		else {
			Text = std::to_wstring(m_weapon->m_rounds);
		}

		SetBkMode(hDC, TRANSPARENT);
		SetTextColor(hDC, RGB(0, 0, 0));
		DrawText(hDC, Text.c_str(), -1, &Rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
	}

	DeleteObject(hPen);
	DeleteObject(hBrush);

	// UI - Player
	hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	SelectObject(hDC, hPen);
	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	SelectObject(hDC, hBrush);

	for (int i = 0; i < m_hp; ++i) {
		Rectangle(hDC, 60 + 25 * i, 5, 
			60 + 25 * (i + 1), 25);
	}

	SelectObject(hDC, oldPen);
	SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}
