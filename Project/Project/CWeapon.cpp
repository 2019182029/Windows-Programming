#include "CWeapon.h"

//////////////////////////////////////////////////
// Bullet
Bullet::Bullet() {
	m_x = 0.0f; m_y = 0.0f;
	m_x_velocity = 0.0f; m_y_velocity = 0.0f;

	m_distance = 0.0f;

	m_hit = false;
	m_hit_time = std::chrono::system_clock::now();
}

Bullet::Bullet(float x, float y, int dir, float speed) : m_x(x), m_y(y) {
	switch (dir) {
	case UP:
		m_x_velocity = 0.0f;
		m_y_velocity = -speed;
		break;

	case DOWN:
		m_x_velocity = 0.0f;
		m_y_velocity = speed;
		break;

	case LEFT:
		m_x_velocity = -speed;
		m_y_velocity = 0.0f;
		break;

	case RIGHT:
		m_x_velocity = speed;
		m_y_velocity = 0.0f;
		break;
	}

	m_distance = 0.0f;

	m_hit = false;
	m_hit_time = std::chrono::system_clock::now();
}

Bullet::Bullet(float x, float y, float angle) : m_x(x), m_y(y) {
	float speed = 7.5f + (rand() % 5);

	m_x_velocity = cosf(angle) * speed;
	m_y_velocity = sinf(angle) * speed;

	m_distance = 0.0f;

	m_hit = false;
	m_hit_time = std::chrono::system_clock::now();
}

void Bullet::hit() {
	m_hit = true;
	m_hit_time = std::chrono::system_clock::now();
}

void Bullet::move() {
	m_x += m_x_velocity;
	m_y += m_y_velocity;

	m_distance += abs(m_x_velocity);
	m_distance += abs(m_y_velocity);
}

void Bullet::update() {
	if (!m_hit) {
		move();
	}
}

void Bullet::print(HDC hDC, HDC bDC) const {
	if (!m_hit) {
		SelectObject(bDC, Pic_Bullet);
		TransparentBlt(hDC, static_cast<int>(m_x), static_cast<int>(m_y), 10, 10,
			bDC, 0, 0, Bmp_Bullet.bmWidth, Bmp_Bullet.bmHeight, RGB(255, 255, 255));
	} else {
		SelectObject(DamageDC, Pic_Damage);
		TransparentBlt(hDC, static_cast<int>(m_x) - 10, static_cast<int>(m_y) - 10, 50, 50,
			DamageDC, 0, 0, Bmp_Damage.bmWidth, Bmp_Damage.bmHeight, RGB(255, 255, 255));
	}
}

//////////////////////////////////////////////////
// Weapon
Weapon::Weapon(int type) {
	m_type = type;

	switch (m_type) {
	case PISTOL:
		m_rounds = 0x7FFF'FFFF;
		m_attack = 5;

		m_range = 600.0f;
		m_cooldown = 150.0f;
		break;

	case SMG:
		m_rounds = 30;
		m_attack = 5;

		m_range = 600.0f;
		m_cooldown = 300.0f;
		break;

	case SHOTGUN:
		m_rounds = 8;
		m_attack = 20;
		// 나중에 공격력 10으로 변경 (지금은 테스트 용으로 공격력 높여두기)

		m_range = 300.0f;
		m_cooldown = 1000.0f;
		break;

	case SNIPER:
		m_rounds = 5;
		m_attack = 50;

		m_range = 1200.0f;
		m_cooldown = 1000.0f;
		break;
	}

	m_shot_time = std::chrono::system_clock::now();
	m_burst_count = 0;
}

void Weapon::fire(int x, int y, int dir) {
	if (m_cooldown > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_shot_time).count()) {
		return;
	}

	switch (m_type) {
	case PISTOL:
		--m_rounds;
		m_bullets.emplace_back(static_cast<float>(x), static_cast<float>(y), dir);
		break;

	case SMG:
		m_burst_count = 3;
		break;

	case SHOTGUN: {
		--m_rounds;

		float base_angle;

		switch (dir) {
		case UP:    base_angle = -static_cast<float>(M_PI) / 2;	break;
		case DOWN:  base_angle = static_cast<float>(M_PI) / 2; break;
		case LEFT:  base_angle = static_cast<float>(M_PI); break;
		case RIGHT: base_angle = 0.0f; break;
		}

		for (int i = -3; i <= 3; ++i) {
			float spread = 0.05f; 
			float angle = base_angle + (i * spread);

			m_bullets.emplace_back(static_cast<float>(x), static_cast<float>(y), angle);
		}
		break;
	}

	case SNIPER:
		--m_rounds;
		m_bullets.emplace_back(static_cast<float>(x), static_cast<float>(y), dir, 50.0f);
		break;
	}

	m_shot_time = std::chrono::system_clock::now();
}

void Weapon::update() {
	auto current_time = std::chrono::system_clock::now();

	for (auto iter = m_bullets.begin(); iter != m_bullets.end();) {
		iter->update();

		if ((iter->m_distance >= m_range) ||
			(iter->m_hit && 
				100 < std::chrono::duration_cast<std::chrono::milliseconds>(current_time - iter->m_hit_time).count())) {
			iter = m_bullets.erase(iter);
			continue;
		}
		++iter;
	}
}

void Weapon::print(HDC hDC, HDC bDC) const {
	for (const auto& bullet : m_bullets) {
		bullet.print(hDC, bDC);
	}
}
