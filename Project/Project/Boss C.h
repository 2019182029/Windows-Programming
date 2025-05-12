#pragma once
#include"Common.h"

HDC Boss_C_DC;
HBITMAP Pic_Boss_C;
// ���� ��Ʈ�� ���� ���� (������ ���� �뵵)
HBITMAP old_Pic_Boss_C;

class Boss_C {
	int x, y; // ���� ��ġ
	int direct; // 1: ���� ���, 2: ���� ���, 3: ���� �ϴ�, 4: ���� �ϴ�
	int hp;
	int count; // ���� Ƚ��
	int print_type; // ��� Ÿ��(��ҿ� SRCCOPY���ٰ� ���ΰ� ĳ���Ϳ��� �ǰ� ���ϸ� 0.3�� �� NOTSRCCOPY�� ��µ�)
	POINT Platform[4]; // ���� ��ǥ
	class attack_ball { // ���� ��ü
		int a_x, a_y; // �� �߽���
		int degree;
		double rad; // �� ����
		int a_direct; // ���� ����� ����  1: ��, 2: ��, 3: ��, 4: ��
		int r; // ������
	public:
		attack_ball(int a_x, int a_y, int a_direct, int degree, int r) : a_x(a_x), a_y(a_y), a_direct(a_direct), degree(degree), r(r) {
			a_x += 500 + r * cos(rad);
			a_y += 500 + r * sin(rad);
		}
		// ���� ��ü ȸ��
		void turn_ball() { degree += 10; rad = degree * M_PI / 180; }
	};
	std::vector<attack_ball> attack;
public:
	Boss_C(int x, int y, int direct, int hp) : x(x), y(y), direct(direct), hp(hp) {
		print_type = SRCCOPY;
		// ��
		Platform[0].x = 500;
		Platform[0].y = 200;
		// ��
		Platform[1].x = 500;
		Platform[1].y = 700;
		// ��
		Platform[2].x = 200;
		Platform[2].y = 450;
		// ��
		Platform[3].x = 800;
		Platform[3].y = 450;
		attack.emplace_back(500, 500, 1, 0, 0);
		for (int i = 0; i < 32; i++) {
			attack.emplace_back()
		}
	}
	// ���� ȸ�� ����
	void windmill() {

	}
	// ���
	void print_stage6() {
		
		for (int i = 0; i < 4; i++) {
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		TransparentBlt(mainDC, x, y, 200, 200, Boss_C_DC, 0, 0, 200, 200, RGB(255, 255, 255));
	}
};
Boss_C C(500, 700, 1, 1000);