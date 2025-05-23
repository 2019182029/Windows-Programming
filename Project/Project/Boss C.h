#pragma once
#include"Common.h"
#include "Element.h"
// Boss C는 60번대 타이머 사용
#define C_Turning_attack 60
#define C_Moving 61
#define C_animation 62

HDC Boss_C_DC;
HBITMAP Pic_Boss_C[5];
// 이전 비트맵 정보 저장 (안전한 제거 용도)
HBITMAP old_Pic_Boss_C;

int stage6_onoff = 0;
int Boss_C_ani = 0; // 애니메이션 변경 변수

class Boss_C {
	int x, y; // 보스 위치
	int direct; // 1: 우측 상단, 2: 좌측 상단, 3: 좌측 하단, 4: 우측 하단
	int hp;
	POINT Platform[4]; // 발판 좌표
	class attack_ball { // 공격 구체
		int a_x, a_y; // 원 중심점
		int degree;
		double rad; // 원 라디안
		int a_direct; // 원이 뻗어나온 방향  1: 상, 2: 하, 3: 좌, 4: 우
		int r; // 반지름
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
		// 공격 구체 회전
		void turn_ball() { 
			degree += 2; // 한 번의 회전량 
			rad = degree * M_PI / 180; 
			a_x = 600 + r * cos(rad);
			a_y = 450 + r * sin(rad);
		}
	};
	std::vector<attack_ball> attack;
public:
	Boss_C(int x, int y, int direct, int hp) : x(x), y(y), direct(direct), hp(hp) {
		// 상
		Platform[0].x = 500;
		Platform[0].y = 200;
		// 하
		Platform[1].x = 500;
		Platform[1].y = 700;
		// 좌
		Platform[2].x = 200;
		Platform[2].y = 450;
		// 우
		Platform[3].x = 800;
		Platform[3].y = 450;
		attack.emplace_back(0, 0); // 중앙 구체 생성 (회전의 중심)
		int count = 0;
		for (int i = 0; i < 48; i++) {
			if (i % 4 == 0) {
				count++;
			}
			attack.emplace_back(i * 90 - 3 * count, count * 40);
			
		}
	}
	// 보스 회전 공격
	void windmill() {
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			it->turn_ball();
		}
	}
	// 보스 이동
	void moving() {
		// 이동 거리
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
	void print_stage6() {
		int size = 20; // 구체 크기
		for (int i = 0; i < 4; i++) {
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			SelectObject(mainDC, blue_Brush);
			Ellipse(mainDC, it->ax() - size, it->ay() - size, it->ax() + size, it->ay() + size);
		}
		SelectObject(Boss_C_DC, Pic_Boss_C[Boss_C_ani]);
		TransparentBlt(mainDC, x, y, 200, 200, Boss_C_DC, 0, 0, 200, 200, RGB(255, 255, 255));
		// 보스 체력바 출력
		SelectObject(mainDC, red_Brush);
		Rectangle(mainDC, 150, 900, hp, 950);
		next_stage();
	}
};
Boss_C C(600, 450, 1, 1000);