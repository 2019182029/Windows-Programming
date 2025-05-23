#pragma once
#include"Common.h"
#include "Element.h"
// Boss C�� 60���� Ÿ�̸� ���
#define C_Turning_attack 60
#define C_Moving 61
#define C_animation 62

HDC Boss_C_DC;
HBITMAP Pic_Boss_C[5];
// ���� ��Ʈ�� ���� ���� (������ ���� �뵵)
HBITMAP old_Pic_Boss_C;

int stage6_onoff = 0;
int Boss_C_ani = 0; // �ִϸ��̼� ���� ����

class Boss_C {
	int x, y; // ���� ��ġ
	int direct; // 1: ���� ���, 2: ���� ���, 3: ���� �ϴ�, 4: ���� �ϴ�
	int hp;
	POINT Platform[4]; // ���� ��ǥ
	class attack_ball { // ���� ��ü
		int a_x, a_y; // �� �߽���
		int degree;
		double rad; // �� ����
		int a_direct; // ���� ����� ����  1: ��, 2: ��, 3: ��, 4: ��
		int r; // ������
	public:
		attack_ball(int degree, int r) : degree(degree), r(r) {
			a_x = 600 + r * cos(rad);
			a_y = 450 + r * sin(rad);
			rad = degree * M_PI / 180;
			a_x = 600 + r * cos(rad);
			a_y = 450 + r * sin(rad);
		}
		int ax() { return a_x; }
		int ay() { return a_y; }
		// ���� ��ü ȸ��
		void turn_ball() { 
			degree += 2; // �� ���� ȸ���� 
			rad = degree * M_PI / 180; 
			a_x = 600 + r * cos(rad);
			a_y = 450 + r * sin(rad);
		}
	};
	std::vector<attack_ball> attack;
public:
	Boss_C(int x, int y, int direct, int hp) : x(x), y(y), direct(direct), hp(hp) {
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
		attack.emplace_back(0, 0); // �߾� ��ü ���� (ȸ���� �߽�)
		int count = 0;
		for (int i = 0; i < 48; i++) {
			if (i % 4 == 0) {
				count++;
			}
			attack.emplace_back(i * 90 - 3 * count, count * 40);
			
		}
	}
	// ���� ȸ�� ����
	void windmill() {
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			it->turn_ball();
		}
	}
	// ���� �̵�
	void moving() {
		// �̵� �Ÿ�
		int move_x = 15;
		int move_y = 15;
		RECT area = {320, 200, 840, 720};
		if (direct == 1) {
			x += move_x;
			y -= move_y;
			if (x + 100 >= area.right or y + 100 <= area.top) {
				direct = 2;
			}
		}
		else if (direct == 2) {
			x -= move_x;
			y -= move_y;
			if (x + 100 <= area.left or y + 100 <= area.top) {
				direct = 3;
			}
		}
		else if (direct == 3) {
			x -= move_x;
			y += move_y;
			if (x + 100 <= area.left or y + 100 >= area.bottom) {
				direct = 4;
			}
		}
		else if (direct == 4) {
			x += move_x;
			y += move_y;
			if (x + 100 >= area.right or y + 100 >= area.bottom) {
				direct = 1;
			}
		}
	}
	// ȸ�� ������ ���� (�÷��̾� ���ݰ� ��ȣ�ۿ� �ϴ� �������� ȣ��)
	void make_heal() {
		std::uniform_int_distribution<int> ran{ 1, 4 };
		if (hp < 1000 and hp % 250 == 0) {
			heal.emplace_back(Platform[ran(dre)].x + 100, Platform[ran(dre)].y - 50);
		}
	}
	// ���� Ŭ����
	void next_stage() {
		if (hp <= 0) {
			stage++;
		}
	}
	// ���
	void print_stage6() {
		int size = 20; // ��ü ũ��
		for (int i = 0; i < 4; i++) {
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			SelectObject(mainDC, blue_Brush);
			Ellipse(mainDC, it->ax() - size, it->ay() - size, it->ax() + size, it->ay() + size);
		}
		SelectObject(Boss_C_DC, Pic_Boss_C[Boss_C_ani]);
		TransparentBlt(mainDC, x, y, 200, 200, Boss_C_DC, 0, 0, 200, 200, RGB(255, 255, 255));
		// ���� ü�¹� ���
		SelectObject(mainDC, red_Brush);
		Rectangle(mainDC, 150, 900, hp, 950);
		next_stage();
	}
};
Boss_C C(600, 450, 1, 1000);