#pragma once
#include "Common.h"
#include "Element.h"
// Boss B�� 40���� Ÿ�̸� ���
#define B_make_attack 40
#define B_remove_attack 41
#define B_shoot_attack 42
#define B_animation 43

HDC Boss_B_DC;
HBITMAP Pic_Boss_B_row[7], Pic_Boss_B_col[7];
// ���� ��Ʈ�� ���� ���� (������ ���� �뵵)
HBITMAP old_Pic_Boss_B;

int stage4_onoff = 0; // Ÿ�̸� �ߺ� ���� ����
int Boss_B_ani = 0; // �ִϸ��̼� ���� ����

// �浹 üũ�� �� (1: ������ ����(Attack Ŭ������ �ִ� a_x, a_y �߽����� �̿�), 2: ���ΰ� ĳ���Ͱ� ���� ���� �� �� �ֵ��� Platform[4] �迭�� ��ǥ ����
// �� ����Ŭ�� ������ ������ 2~3�� ���߷��� ������, �������� ���̵��� �ʹ� ���� ������ ���������� �׳� ���������� �ڸ��� �ٲ㳪���� ������ �����ϵ��� ���� (�����Բ� �̷� ���� ���� ���� �������� ���庸��)
class Boss_B {
public:
	int x, y; // ���� ��ġ
	int direct; // 1: ��, 2: ��, 3: ��, 4: ��
	int hp;
	int count; // ���� Ƚ��
	POINT Platform[4]; // ���� ��ǥ
	class Attack { // ���ݿ� ������ ��ġ ������ ������ Ŭ����
		int a_x, a_y; // �߽��� ��ǥ
		POINT P[3]; // �ﰢ�� ����� ���� ��ǥ
		int a_direct; // �ش� �ﰢ���� �߻�Ǵ� ����
		int size; // �ﰢ�� ũ��
	public:
		Attack(int a_x, int a_y, int a_direct): a_x(a_x), a_y(a_y), a_direct(a_direct) {
			size = 20; // �ﰢ�� ũ��
			if (a_direct == 1) { // �Ʒ��� ���ϴ� �ﰢ��
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y - size;
				P[2].x = a_x;
				P[2].y = a_y + size;
			}
			else if (a_direct == 2) { // ���� ���ϴ� �ﰢ��
				P[0].x = a_x - size;
				P[0].y = a_y + size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x;
				P[2].y = a_y - size;
			}
			else if (a_direct == 3) { // �������� ���ϴ� �ﰢ��
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x - size;
				P[1].y = a_y + size;
				P[2].x = a_x + size;
				P[2].y = a_y;
			}
			else if (a_direct == 4) { // ������ ���ϴ� �ﰢ��
				P[0].x = a_x + size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x - size;
				P[2].y = a_y;
			}
		}
		POINT *a_Point() { return P; }
		int adirect() { return a_direct; }
		void move(int num_x, int num_y) {
			a_x += num_x;
			a_y += num_y;
			// P��ǥ �ֽ�ȭ
			if (a_direct == 1) { // �Ʒ��� ���ϴ� �ﰢ��
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y - size;
				P[2].x = a_x;
				P[2].y = a_y + size;
			}
			else if (a_direct == 2) { // ���� ���ϴ� �ﰢ��
				P[0].x = a_x - size;
				P[0].y = a_y + size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x;
				P[2].y = a_y - size;
			}
			else if (a_direct == 3) { // �������� ���ϴ� �ﰢ��
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x - size;
				P[1].y = a_y + size;
				P[2].x = a_x + size;
				P[2].y = a_y;
			}
			else if (a_direct == 4) { // ������ ���ϴ� �ﰢ��
				P[0].x = a_x + size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x - size;
				P[2].y = a_y;
			}
		}
	};
	std::vector<Attack> attack; // ���� ���ݿ� ���ÿ� ���� ����
public:
	// ������
	Boss_B(int direct, int hp) : direct(direct), hp(hp) {
		count = 0;
		// ���� ��ġ �ʱ�ȭ
		Platform[0].x = 200;
		Platform[0].y = 400;
		Platform[1].x = 300;
		Platform[1].y = 600;
		Platform[2].x = 700;
		Platform[2].y = 600;
		Platform[3].x = 800;
		Platform[3].y = 400;
	}
	// ������ ��ȯ �Լ�
	int b_x() { return x; }
	int b_y() { return y; }
	int b_direct() { return direct; }
	int b_count() { return count; }
	// ���⿡ ���� ������ ��ġ ����(x, y) �ֽ�ȭ
	void b_pos(){
		if (direct == 1) {
			x = 100;
			y = 25;
		}
		else if (direct == 2) {
			x = 100;
			y = 830;
		}
		else if (direct == 3) {
			x = 20;
			y = 75;
		}
		else if (direct == 4) {
			x = 1065;
			y = 75;
		}
	}
	// �� ���� �� ���� Ƚ�� ���� (�̵� ��Ŀ������ ���� ��)
	void count_attack(int num) { count = num; }
	// ���� ���� �Լ�
	void move_direct(int num) { direct = num; }
	// ���� ���� ���� (������ ������)
	void attack_pattern() {
		b_pos();
		if (direct == 1 or direct == 2) { // ���� �Ʒ��� ���� ��� y�� ����, �¿� x���� ����
			std::uniform_int_distribution<int> ran_x( 200, 800 );
			if(direct == 1){ attack.emplace_back(ran_x(dre), y + 100, direct); }
			else if(direct == 2){ attack.emplace_back(ran_x(dre), y, direct); }
		}
		else if (direct == 3 or direct == 4) { // �����̳� �����ʿ� ���� ��� x�� ����, ���Ʒ� y���� ����
			std::uniform_int_distribution<int> ran_y( 300, 500 );
			if (direct == 3) { attack.emplace_back(x + 100, ran_y(dre), direct); }
			else if (direct == 4) { attack.emplace_back(x, ran_y(dre), direct); }
		}
	}
	// ���� �����
	void remove_attack() {
		if (!attack.empty()) {
			attack.erase(attack.begin());
		}
	}
	// ������ ������(�߻�)
	void shoot_attack() {
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			if (it->adirect() == 1) {
				it->move(0, 10);
			}
			else if (it->adirect() == 2) {
				it->move(0, -10);
			}
			else if (it->adirect() == 3) {
				it->move(10, 0);
			}
			else if (it->adirect() == 4) {
				it->move(-10, 0);
			}
		}
	}
	// ���� Ŭ����
	void next_stage() {
		if (hp <= 0) {
			stage++;
		}
	}
	// ����
	void reset() {
		direct = 4;
		b_pos();
		hp = 1000;
		count = 0;
		attack.clear();
		stage4_onoff = 0;
	}
	// ���
	void print_stage4() {
		// ���� ���
		b_pos();
		if (b_direct() == 1 or b_direct() == 2) {
			SelectObject(Boss_B_DC, Pic_Boss_B_row[Boss_B_ani]);
			TransparentBlt(mainDC, b_x(), b_y(), 1000, 100, Boss_B_DC, 0, 0, 600, 100, RGB(255, 255, 255));
		}
		else {
			SelectObject(Boss_B_DC, Pic_Boss_B_col[Boss_B_ani]);
			TransparentBlt(mainDC, b_x(), b_y(), 100, 800, Boss_B_DC, 0, 0, 100, 600, RGB(255, 255, 255));
			//StretchBlt(mainDC, b_x(), b_y(), 100, 800, Boss_B_DC, 0, 0, 100, 600, print_type); ���� �־ �Ǵ� ���
		}
		// ���� ���
		SelectObject(PlatformDC, Pic_Platform);
		for (int i = 0; i < 4; i++) { // 1~4�� ���� (���ʺ��� ���ʷ� ���)
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		// ���� ���
		HBRUSH oldBrush = (HBRUSH)SelectObject(mainDC, lightgray_Brush);
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			Polygon(mainDC, it->a_Point(), 3);
		}
		// ���� ü�¹� ���
		SelectObject(mainDC, red_Brush);
		Rectangle(mainDC, 150, 900, hp, 950);
		SelectObject(mainDC, oldBrush);
	}
};
Boss_B B(4, 1000); // ����� ������ ü���� �������� ���ڷ� ��