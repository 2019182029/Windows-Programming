#include "CItem.h"

Item::Item(ITEM_TYPE type) {
	m_x = 200.0f + (rand() % 800);
	m_y = 100.0f + (rand() % 100);
	m_y_velocity = 0.0f;

	m_is_falling = true;

	m_type = type;
	switch (m_type) {
	case WEAPON:
		m_bmp_index = 1 + (rand() % 3);
		break;

	case HEAL:
		m_bmp_index = 0;
		break;
	}
}

Item::Item(float x, float y, ITEM_TYPE type) : m_x(x), m_y(y) {
	m_y_velocity = 0.0f;

	m_is_falling = true;

	m_type = type;
	switch (m_type) {
	case WEAPON:
		m_bmp_index = 1 + (rand() % 3);
		break;

	case HEAL:
		m_bmp_index = 0;
		break;
	}
}

void Item::move() {
	m_y += m_y_velocity;
}

void Item::update() {
	// Gravity
	m_y_velocity = min(MAX_GRAVITY, m_y_velocity + 0.5f);

	move();
}

void Item::print(HDC hDC, HDC iDC) const {
	HPEN hPen = nullptr;
	HPEN oldPen = nullptr;

	HBRUSH hatchBrush = nullptr;
	HBRUSH oldBrush = nullptr;

	switch (m_type) {
	case WEAPON:
		hPen = CreatePen(PS_SOLID, 5, RGB(192, 0, 0));
		oldPen = (HPEN)SelectObject(hDC, hPen);

		hatchBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(128, 0, 0));
		oldBrush = (HBRUSH)SelectObject(hDC, hatchBrush);

		RoundRect(hDC, static_cast<int>(m_x), static_cast<int>(m_y),
			static_cast<int>(m_x + 50), static_cast<int>(m_y + 50), 10, 10);

		SelectObject(iDC, Pic_Weapon[m_bmp_index]);
		TransparentBlt(hDC, static_cast<int>(m_x), static_cast<int>(m_y), 50, 50,
			iDC, 0, 0, Bmp_Weapon[m_bmp_index].bmWidth, Bmp_Weapon[m_bmp_index].bmHeight, RGB(255, 255, 255));
		break;

	case HEAL:
		hPen = CreatePen(PS_SOLID, 5, RGB(0, 192, 0));
		oldPen = (HPEN)SelectObject(hDC, hPen);

		hatchBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 128, 0));
		oldBrush = (HBRUSH)SelectObject(hDC, hatchBrush);

		RoundRect(hDC, static_cast<int>(m_x), static_cast<int>(m_y),
			static_cast<int>(m_x + 50), static_cast<int>(m_y + 50), 10, 10);

		SelectObject(iDC, Pic_Heal);
		TransparentBlt(hDC, static_cast<int>(m_x), static_cast<int>(m_y), 50, 50,
			iDC, 0, 0, Bmp_Heal.bmWidth, Bmp_Heal.bmHeight, RGB(255, 255, 255));
		break;
	}

	SelectObject(hDC, oldPen);
	SelectObject(hDC, oldBrush);
	DeleteObject(hPen);
	DeleteObject(hatchBrush);
}
