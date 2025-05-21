#include "CWeapon.h"

//////////////////////////////////////////////////
// Bullet
Bullet::Bullet() {
	m_x = 0.0f; m_y = 0.0f;
	m_x_velocity = 0.0f; m_y_velocity = 0.0f;

	m_distance = 0.0f;
}

Bullet::Bullet(float x, float y, int dir) : m_x(x), m_y(y) {
	switch (dir) {
	case UP:
		m_x_velocity = 0.0f;
		m_y_velocity = -10.0f;
		break;

	case DOWN:
		m_x_velocity = 0.0f;
		m_y_velocity = 10.0f;
		break;

	case LEFT:
		m_x_velocity = -10.0f;
		m_y_velocity = 0.0f;
		break;

	case RIGHT:
		m_x_velocity = 10.0f;
		m_y_velocity = 0.0f;
		break;

	default:
		m_distance = 2000.0f;
	}

	m_distance = 0.0f;
}

void Bullet::move() {
	m_x += m_x_velocity;
	m_y += m_y_velocity;

	m_distance += m_x_velocity;
	m_distance += m_y_velocity;
}

void Bullet::update() {
	move();
}

void Bullet::print(HDC hDC, HDC bDC) const {
	SelectObject(bDC, Pic_Bullet);
	TransparentBlt(hDC, static_cast<int>(m_x), static_cast<int>(m_y), 10, 10,
		bDC, 0, 0, Bmp_Bullet.bmWidth, Bmp_Bullet.bmHeight, RGB(255, 255, 255));
}

//////////////////////////////////////////////////
// Weapon
Weapon::Weapon(int type) {
	m_type = type;

	switch (m_type) {
	case PISTOL:
		m_rounds = 0x7FFF'FFFF;
		m_attack = 1;

		m_range = 1000.0f;
		m_cooldown = 0.1f;
		break;
	}
}

void Weapon::fire(int x, int y, int dir) {
	m_bullets.emplace_back(x, y, dir);
}

void Weapon::update() {
	for (auto iter = m_bullets.begin(); iter != m_bullets.end();) {
		iter->update();

		if (iter->m_distance >= m_range) {
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
