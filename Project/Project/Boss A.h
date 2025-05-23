#pragma once
#include "Common.h"
#include "Element.h"
// 보스 A는 20번대 타이머 사용
#define A_animation 20
#define A_make_attack 21
#define A_remove_attack 22
#define A_shoot_attack 23

HDC Boss_A_DC;
HBITMAP Pic_Boss_A[7];
// 이전 비트맵 정보 저장 (안전한 제거 용도)
HBITMAP old_Pic_Boss_A;

int stage2_onoff = 0; // 타이머 중복 방지 변수
int Boss_A_ani = 0; // 애니메이션 변경 변수

class Boss_A {
	int x, y; // 보스 위치
	int hp;
	POINT Platform[4]; // 발판 좌표
	class Attack { // 공격
		int ax, ay; // 공격 가시 중심점
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
		// 1번
		Platform[0].x = 200;
		Platform[0].y = 650;
		// 2번
		Platform[1].x = 400;
		Platform[1].y = 650;
		// 3번
		Platform[2].x = 600;
		Platform[2].y = 650;
		// 4번
		Platform[3].x = 800;
		Platform[3].y = 650;
	}
	// 공격 생성
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
	// 공격 제거
	void remove_attack() {
		if (!attack.empty()) {
			attack.erase(attack.begin());
		}
	}
	// 공격 이동
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
	// 회복 아이템 생성 (플레이어 공격과 상호작용 하는 과정에서 호출)
	void make_heal() {
		std::uniform_int_distribution<int> ran{ 1, 4 };
		if (hp < 1000 and hp % 250 == 0) {
			heal.emplace_back(Platform[ran(dre)].x + 100, Platform[ran(dre)].y - 50);
		}
	}
	// 보스 클리어
	void next_stage() {
		if (hp <= 0) {
			stage++;
		}
	}
	// 출력
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
		// 보스 체력바 출력
		SelectObject(mainDC, red_Brush);
		Rectangle(mainDC, 150, 900, hp, 950);
		// 체력 물약
		for (auto it = heal.begin(); it != heal.end(); ++it) {
			TransparentBlt(mainDC, it->h_x(), it->h_y(), 50, 50, HealDC, 0, 0, 50, 50, RGB(255, 255, 255));
		}
		next_stage();
	}
};
Boss_A A(1025, 200, 1000);