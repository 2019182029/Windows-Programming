#pragma once
#include"Common.h"

HDC Boss_C_DC;
HBITMAP Pic_Boss_C;
// 이전 비트맵 정보 저장 (안전한 제거 용도)
HBITMAP old_Pic_Boss_C;

class Boss_C {
	int x, y; // 보스 위치
	int direct; // 1: 우측 상단, 2: 좌측 상단, 3: 좌측 하단, 4: 우측 하단
	int hp;
	int count; // 공격 횟수
	int print_type; // 출력 타입(평소엔 SRCCOPY였다가 주인공 캐릭터에게 피격 당하면 0.3초 간 NOTSRCCOPY로 출력됨)
	POINT Platform[4]; // 발판 좌표
	class attack_ball { // 공격 구체
		int a_x, a_y; // 원 중심점
		int degree;
		double rad; // 원 라디안
		int a_direct; // 원이 뻗어나온 방향  1: 상, 2: 하, 3: 좌, 4: 우
		int r; // 반지름
	public:
		attack_ball(int a_x, int a_y, int a_direct, int degree, int r) : a_x(a_x), a_y(a_y), a_direct(a_direct), degree(degree), r(r) {
			a_x += 500 + r * cos(rad);
			a_y += 500 + r * sin(rad);
		}
		// 공격 구체 회전
		void turn_ball() { degree += 10; rad = degree * M_PI / 180; }
	};
	std::vector<attack_ball> attack;
public:
	Boss_C(int x, int y, int direct, int hp) : x(x), y(y), direct(direct), hp(hp) {
		print_type = SRCCOPY;
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
		attack.emplace_back(500, 500, 1, 0, 0);
		for (int i = 0; i < 32; i++) {
			attack.emplace_back()
		}
	}
	// 보스 회전 공격
	void windmill() {

	}
	// 출력
	void print_stage6() {
		
		for (int i = 0; i < 4; i++) {
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		TransparentBlt(mainDC, x, y, 200, 200, Boss_C_DC, 0, 0, 200, 200, RGB(255, 255, 255));
	}
};
Boss_C C(500, 700, 1, 1000);