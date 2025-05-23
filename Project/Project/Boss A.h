#pragma once
#include "Common.h"
#include "Element.h"
// ���� A�� 20���� Ÿ�̸� ���
#define A_animation 20
#define A_make_attack 21
#define A_remove_attack 22
#define A_shoot_attack 23

HDC Boss_A_DC;
HBITMAP Pic_Boss_A[7];
// ���� ��Ʈ�� ���� ���� (������ ���� �뵵)
HBITMAP old_Pic_Boss_A;

int stage2_onoff = 0; // Ÿ�̸� �ߺ� ���� ����
int Boss_A_ani = 0; // �ִϸ��̼� ���� ����

class Boss_A {
	int x, y; // ���� ��ġ
	int hp;
	POINT Platform[4]; // ���� ��ǥ
	class Attack { // ����
		int ax, ay; // ���� ���� �߽���
		int direct;
		int size;
		POINT P[3];
	public:
		Attack(int ax, int ay, int direct): ax(ax), ay(ay), direct(direct){
			size = 20;
			if (direct == 1) {
				P[0].x = ax + size;
				P[0].y = ay - size;
				P[1].x = ax + size;
				P[1].y = ay + size;
				P[2].x = ax - size;
				P[2].y = ay;
			}
			else if (direct == 2) {
				P[0].x = ax - size;
				P[0].y = ay - size;
				P[1].x = ax + size;
				P[1].y = ay - size;
				P[2].x = ax;
				P[2].y = ay + size;
			}
		}
		int a_direct() { return direct; }
		int a_size() { return size; }
		POINT *a_point() { return P; }
		void left_move() { 
			ax -= 20; 
			if (direct == 1) {
				P[0].x = ax + size;
				P[0].y = ay - size;
				P[1].x = ax + size;
				P[1].y = ay + size;
				P[2].x = ax - size;
				P[2].y = ay;
			}
			else if (direct == 2) {
				P[0].x = ax - size;
				P[0].y = ay - size;
				P[1].x = ax + size;
				P[1].y = ay - size;
				P[2].x = ax;
				P[2].y = ay + size;
			}
		}
		void down_move() { 
			ay += 20; 
			if (direct == 1) {
				P[0].x = ax + size;
				P[0].y = ay - size;
				P[1].x = ax + size;
				P[1].y = ay + size;
				P[2].x = ax - size;
				P[2].y = ay;
			}
			else if (direct == 2) {
				P[0].x = ax - size;
				P[0].y = ay - size;
				P[1].x = ax + size;
				P[1].y = ay - size;
				P[2].x = ax;
				P[2].y = ay + size;
			}
		}
	};
	std::vector<Attack> attack;
public:
	Boss_A(int x, int y, int hp) : x(x), y(y), hp(hp) {
		// 1��
		Platform[0].x = 200;
		Platform[0].y = 650;
		// 2��
		Platform[1].x = 400;
		Platform[1].y = 650;
		// 3��
		Platform[2].x = 600;
		Platform[2].y = 650;
		// 4��
		Platform[3].x = 800;
		Platform[3].y = 650;
	}
	// ���� ����
	void make_attack(int num) {
		if (num == 1) {
			for (int i = 0; i < 20; i++) {
				attack.emplace_back(1000, 100 + i * 40, 1);
			}
		}
		else if(num == 2) {
			for (int i = 0; i < 20; i++) {
				attack.emplace_back(200 + i * 40, 0, 2);
			}
		}
	}
	// ���� ����
	void remove_attack() {
		if (!attack.empty()) {
			attack.erase(attack.begin());
		}
	}
	// ���� �̵�
	void shoot_attack() {
		if (!attack.empty()) {
			for (auto it = attack.begin(); it != attack.end(); ++it) {
				if (it->a_direct() == 1) {
					it->left_move();
				}
				else {
					it->down_move();
				}
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
	void print_stage2() {
		for (int i = 0; i < 4; i++) {
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		SelectObject(mainDC, lightgray_Brush);
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			Polygon(mainDC, it->a_point(), 3);
		}
		SelectObject(Boss_A_DC, Pic_Boss_A[Boss_A_ani]);
		TransparentBlt(mainDC, x, y, 100, 600, Boss_A_DC, 0, 0, 100, 600, RGB(255, 255, 255));
		// ���� ü�¹� ���
		SelectObject(mainDC, red_Brush);
		Rectangle(mainDC, 150, 900, hp, 950);
		// ü�� ����
		for (auto it = heal.begin(); it != heal.end(); ++it) {
			TransparentBlt(mainDC, it->h_x(), it->h_y(), 50, 50, HealDC, 0, 0, 50, 50, RGB(255, 255, 255));
		}
		next_stage();
	}
};
Boss_A A(1025, 200, 1000);