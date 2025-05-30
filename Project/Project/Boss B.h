#pragma once
#include "Common.h"
#include "Element.h"
// Boss B는 40번대 타이머 사용
#define B_make_attack 40
#define B_remove_attack 41
#define B_shoot_attack 42
#define B_animation 43

HDC Boss_B_DC;
HBITMAP Pic_Boss_B_row[7], Pic_Boss_B_col[7];
// 이전 비트맵 정보 저장 (안전한 제거 용도)
HBITMAP old_Pic_Boss_B;

int stage4_onoff = 0; // 타이머 중복 방지 변수
int Boss_B_ani = 0; // 애니메이션 변경 변수

// 충돌 체크할 것 (1: 보스의 공격(Attack 클래스에 있는 a_x, a_y 중심점을 이용), 2: 주인공 캐릭터가 발판 위에 설 수 있도록 Platform[4] 배열의 좌표 참고
// 한 사이클의 공격이 끝나고 2~3초 멈추려고 했으나, 생각보다 난이도가 너무 쉽고 게임이 루즈해져서 그냥 연속적으로 자리를 바꿔나가며 공격을 지속하도록 만듦 (교수님께 이런 세부 사항 변경 가능한지 여쭤보기)
class Boss_B {
public:
	int x, y; // 보스 위치
	int direct; // 1: 상, 2: 하, 3: 좌, 4: 우
	int hp;
	int count; // 공격 횟수
	POINT Platform[4]; // 발판 좌표
	class Attack { // 공격용 가시의 위치 정보를 저장한 클래스
		int a_x, a_y; // 중심점 좌표
		POINT P[3]; // 삼각형 출력을 위한 좌표
		int a_direct; // 해당 삼각형이 발사되는 방향
		int size; // 삼각형 크기
	public:
		Attack(int a_x, int a_y, int a_direct): a_x(a_x), a_y(a_y), a_direct(a_direct) {
			size = 20; // 삼각형 크기
			if (a_direct == 1) { // 아래를 향하는 삼각형
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y - size;
				P[2].x = a_x;
				P[2].y = a_y + size;
			}
			else if (a_direct == 2) { // 위를 향하는 삼각형
				P[0].x = a_x - size;
				P[0].y = a_y + size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x;
				P[2].y = a_y - size;
			}
			else if (a_direct == 3) { // 오른쪽을 향하는 삼각형
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x - size;
				P[1].y = a_y + size;
				P[2].x = a_x + size;
				P[2].y = a_y;
			}
			else if (a_direct == 4) { // 왼쪽을 향하는 삼각형
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
			// P좌표 최신화
			if (a_direct == 1) { // 아래를 향하는 삼각형
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y - size;
				P[2].x = a_x;
				P[2].y = a_y + size;
			}
			else if (a_direct == 2) { // 위를 향하는 삼각형
				P[0].x = a_x - size;
				P[0].y = a_y + size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x;
				P[2].y = a_y - size;
			}
			else if (a_direct == 3) { // 오른쪽을 향하는 삼각형
				P[0].x = a_x - size;
				P[0].y = a_y - size;
				P[1].x = a_x - size;
				P[1].y = a_y + size;
				P[2].x = a_x + size;
				P[2].y = a_y;
			}
			else if (a_direct == 4) { // 왼쪽을 향하는 삼각형
				P[0].x = a_x + size;
				P[0].y = a_y - size;
				P[1].x = a_x + size;
				P[1].y = a_y + size;
				P[2].x = a_x - size;
				P[2].y = a_y;
			}
		}
	};
	std::vector<Attack> attack; // 보스 공격용 가시에 대한 벡터
public:
	// 생성자
	Boss_B(int direct, int hp) : direct(direct), hp(hp) {
		count = 0;
		// 발판 위치 초기화
		Platform[0].x = 200;
		Platform[0].y = 400;
		Platform[1].x = 300;
		Platform[1].y = 600;
		Platform[2].x = 700;
		Platform[2].y = 600;
		Platform[3].x = 800;
		Platform[3].y = 400;
	}
	// 변수값 반환 함수
	int b_x() { return x; }
	int b_y() { return y; }
	int b_direct() { return direct; }
	int b_count() { return count; }
	// 방향에 따른 보스의 위치 정보(x, y) 최신화
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
	// 각 방향 당 공격 횟수 측정 (이동 메커니즘을 위한 것)
	void count_attack(int num) { count = num; }
	// 방향 변경 함수
	void move_direct(int num) { direct = num; }
	// 보스 공격 패턴 (공격을 생성함)
	void attack_pattern() {
		b_pos();
		if (direct == 1 or direct == 2) { // 위나 아래에 있을 경우 y값 고정, 좌우 x값만 변동
			std::uniform_int_distribution<int> ran_x( 200, 800 );
			if(direct == 1){ attack.emplace_back(ran_x(dre), y + 100, direct); }
			else if(direct == 2){ attack.emplace_back(ran_x(dre), y, direct); }
		}
		else if (direct == 3 or direct == 4) { // 왼쪽이나 오른쪽에 있을 경우 x값 고정, 위아래 y값만 변동
			std::uniform_int_distribution<int> ran_y( 300, 500 );
			if (direct == 3) { attack.emplace_back(x + 100, ran_y(dre), direct); }
			else if (direct == 4) { attack.emplace_back(x, ran_y(dre), direct); }
		}
	}
	// 공격 지우기
	void remove_attack() {
		if (!attack.empty()) {
			attack.erase(attack.begin());
		}
	}
	// 공격이 움직임(발사)
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
	// 보스 클리어
	void next_stage() {
		if (hp <= 0) {
			stage++;
		}
	}
	// 리셋
	void reset() {
		direct = 4;
		b_pos();
		hp = 1000;
		count = 0;
		attack.clear();
		stage4_onoff = 0;
	}
	// 출력
	void print_stage4() {
		// 보스 출력
		b_pos();
		if (b_direct() == 1 or b_direct() == 2) {
			SelectObject(Boss_B_DC, Pic_Boss_B_row[Boss_B_ani]);
			TransparentBlt(mainDC, b_x(), b_y(), 1000, 100, Boss_B_DC, 0, 0, 600, 100, RGB(255, 255, 255));
		}
		else {
			SelectObject(Boss_B_DC, Pic_Boss_B_col[Boss_B_ani]);
			TransparentBlt(mainDC, b_x(), b_y(), 100, 800, Boss_B_DC, 0, 0, 100, 600, RGB(255, 255, 255));
			//StretchBlt(mainDC, b_x(), b_y(), 100, 800, Boss_B_DC, 0, 0, 100, 600, print_type); 배경색 있어도 되는 경우
		}
		// 발판 출력
		SelectObject(PlatformDC, Pic_Platform);
		for (int i = 0; i < 4; i++) { // 1~4번 발판 (왼쪽부터 차례로 출력)
			StretchBlt(mainDC, Platform[i].x, Platform[i].y, 200, 15, PlatformDC, 0, 0, 200, 15, SRCCOPY);
		}
		// 공격 출력
		HBRUSH oldBrush = (HBRUSH)SelectObject(mainDC, lightgray_Brush);
		for (auto it = attack.begin(); it != attack.end(); ++it) {
			Polygon(mainDC, it->a_Point(), 3);
		}
		// 보스 체력바 출력
		SelectObject(mainDC, red_Brush);
		Rectangle(mainDC, 150, 900, hp, 950);
		SelectObject(mainDC, oldBrush);
	}
};
Boss_B B(4, 1000); // 방향과 보스의 체력을 생성자의 인자로 줌