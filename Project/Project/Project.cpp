#include "Common.h"
#include "Boss A.h"
#include "Boss B.h"
#include "Boss C.h"
#include "CPlayer.h"
#include "CItem.h"
#include "Element.h"

//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

HWND g_hWnd;
HWND hWnd2;

Player player;
std::vector<Item> items;
bool key_pressed[256] = { false };

int cumulative_damage = 0;

bool running = true;
auto update_time = std::chrono::system_clock::now();
auto item_spawn_time = std::chrono::system_clock::now();

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void worker();
void change_stage();
void reset();

bool player_platform_collision(const Player& player, const POINT& platform, float old_player_bottom);

bool player_item_collision(const Player& player, const Item& item);
bool player_portal_collision(const Player& player, const Portal& portal);

void player_map_collision();
void player_thorn_collision();
void player_boss_a_collision();
void player_boss_b_collision();
void player_boss_c_collision();

void bullet_map_collision();
void bullet_boss_a_collision();
void bullet_boss_b_collision();
void bullet_boss_c_collision();

void item_map_collision();
bool item_platform_collision(const Item& item, const POINT& platform);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdParam, int nCmdShow) { // WinMain부분에 주석이 일치하지 않는다는 오류는 원래 잘 뜸. 무시해도 됨.
	MSG Message;
	WNDCLASSEX WndClass;
	LPCTSTR lpszClass = L"Window Class Name";
	LPCTSTR lpszWindowName = L"windows program";
	g_hinst = hInstance; // hInstance를 통해 현재 실행 중인 애플리케이션의 인스턴스 핸들을 받음

	// WndClass의 멤버 변수들을 사용
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)WndProc; // lpfnWndProc에 대입하기 위해 WndProc를 WNDPROC 자료형의 함수로 변환 후 대입. 그냥도 대입 가능하지만, 혹시 모를 오류에 대비한 것.
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 윈도우 색 설정해주는 코드 GetStockObject의 인자로 brush를 주면 됨
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	// 윈도우 속성을 줄 때 '|' 인 or의 비트 연산자를 활용하여 속성을 주면 된다. WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_HSCROLL | WS_VSCROLL | WS_THICKFRAME 이런 식으로
	// 인자 순서: 윈도우 클래스 이름, 윈도우 타이틀 이름, 윈도우 스타일, 윈도우 x좌표, 윈도우 y좌표, 윈도우 가로 크기, 윈도우 세로 크기, 부모 윈도우 핸들(부모 없으면 NULL), 윈도우 상단에 붙는 메뉴의 핸들(메뉴 없으면 NULL), WinMain에서 받은 인스턴스 핸들
	// x는 값이 커질수록 창이 우측으로, y는 값이 커질수록 창이 아래로 내려가게 된다.
	g_hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1200, 1000, NULL, (HMENU)NULL, hInstance, NULL);

	std::thread worker_thread(worker);

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	
	running = false;
	worker_thread.join();

	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC;

	switch (iMessage) {
	case WM_CREATE: {
		hWnd2 = hWnd;
		hDC = GetDC(hWnd);
		GetClientRect(hWnd, &rt);

		// 브러시 생성
		red_Brush = CreateSolidBrush(RGB(255, 0, 0));
		blue_Brush = CreateSolidBrush(RGB(0, 0, 255));
		lightgray_Brush = CreateSolidBrush(RGB(240, 240, 240));

		// mainDC에 대한 비트맵 생성 및 설정
		mainDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		old_hBitmap = (HBITMAP)SelectObject(mainDC, (HBITMAP)hBitmap);
		SelectObject(mainDC, (HBITMAP)hBitmap);

		// MapDc에 대한 비트맵 생성
		MapDC = CreateCompatibleDC(hDC);
		Pic_BossMap = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_RelaxMap = (HBITMAP)LoadImage(g_hinst, _T("Resource/relax stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_TutorialMap = (HBITMAP)LoadImage(g_hinst, _T("Resource/tutorial stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_BossMap = (HBITMAP)SelectObject(MapDC, Pic_BossMap);

		// PlayerDC에 대한 비트맵 생성 및 설정
		PlayerDC = CreateCompatibleDC(hDC);
		Pic_Player[IDLE] = (HBITMAP)LoadImage(g_hinst, _T("Resource/player.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[UP] = (HBITMAP)LoadImage(g_hinst, _T("Resource/player_up.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[DOWN] = (HBITMAP)LoadImage(g_hinst, _T("Resource/player_down.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[LEFT] = (HBITMAP)LoadImage(g_hinst, _T("Resource/player_left.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[RIGHT] = (HBITMAP)LoadImage(g_hinst, _T("Resource/player_right.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[DODGE] = (HBITMAP)LoadImage(g_hinst, _T("Resource/player_dodge.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Player = (HBITMAP)SelectObject(PlayerDC, Pic_Player[player.m_anim_state]);
		for (int i = 0; i < 6; ++i) {
			GetObject(Pic_Player[i], sizeof(BITMAP), &Bmp_Player[i]);
		}

		// PlatformDC에 대한 비트맵 생성 및 설정
		PlatformDC = CreateCompatibleDC(hDC);
		Pic_Platform = (HBITMAP)LoadImage(g_hinst, _T("Resource/platform.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Platform = (HBITMAP)SelectObject(PlatformDC, Pic_Platform);
		GetObject(Pic_Platform, sizeof(BITMAP), &Bmp_Platform);

		// WeaponDC에 대한 비트맵 생성 및 설정
		WeaponDC = CreateCompatibleDC(hDC);
		Pic_Weapon[PISTOL] = (HBITMAP)LoadImage(g_hinst, _T("Resource/pistol.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SMG] = (HBITMAP)LoadImage(g_hinst, _T("Resource/smg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SHOTGUN] = (HBITMAP)LoadImage(g_hinst, _T("Resource/shotgun.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SNIPER] = (HBITMAP)LoadImage(g_hinst, _T("Resource/sniper.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		for (int i = 0; i < 4; ++i) {
			GetObject(Pic_Weapon[i], sizeof(BITMAP), &Bmp_Weapon[i]);
		}
		old_Pic_Weapon = (HBITMAP)SelectObject(WeaponDC, Pic_Weapon[0]);

		// BulletDC에 대한 비트맵 생성 및 설정
		BulletDC = CreateCompatibleDC(hDC);
		Pic_Bullet = (HBITMAP)LoadImage(g_hinst, _T("Resource/bullet.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Bullet = (HBITMAP)SelectObject(BulletDC, Pic_Bullet);
		GetObject(Pic_Bullet, sizeof(BITMAP), &Bmp_Bullet);

		DamageDC = CreateCompatibleDC(hDC);
		Pic_Damage = (HBITMAP)LoadImage(g_hinst, _T("Resource/Damage Effect.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Damage = (HBITMAP)SelectObject(DamageDC, Pic_Damage);
		GetObject(Pic_Damage, sizeof(BITMAP), &Bmp_Damage);

		// PortalDC에 대한 비트맵 생성 및 설정
		PortalDC = CreateCompatibleDC(hDC);
		Pic_Portal = (HBITMAP)LoadImage(g_hinst, _T("Resource/portal.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Portal = (HBITMAP)SelectObject(PortalDC, Pic_Portal);
		GetObject(Pic_Portal, sizeof(BITMAP), &Bmp_Portal);

		// HealDC에 대한 비트맵 생성 및 설정
		HealDC = CreateCompatibleDC(hDC);
		Pic_Heal = (HBITMAP)LoadImage(g_hinst, _T("Resource/healing_item.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Heal = (HBITMAP)SelectObject(HealDC, Pic_Heal);
		GetObject(Pic_Heal, sizeof(BITMAP), &Bmp_Heal);

		// Boss_A_DC에 대한 비트맵 생성 및 설정
		Boss_A_DC = CreateCompatibleDC(hDC);
		Pic_Boss_A[0] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[1] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[2] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[3] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[4] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[5] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[6] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss A7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_A = (HBITMAP)SelectObject(Boss_A_DC, Pic_Boss_A);

		// Boss_B_DC에 대한 비트맵 생성 및 설정
		Boss_B_DC = CreateCompatibleDC(hDC);
		Pic_Boss_B_row[0] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[1] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[2] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[3] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[4] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[5] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[6] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_row7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[0] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[1] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[2] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[3] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[4] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[5] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[6] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss B_col7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_B = (HBITMAP)SelectObject(Boss_B_DC, Pic_Boss_B_row[0]);

		// Boss_C_DC에 대한 비트맵 생성 및 설정
		Boss_C_DC = CreateCompatibleDC(hDC);
		Pic_Boss_C[0] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss C1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[1] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss C2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[2] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss C3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[3] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss C4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[4] = (HBITMAP)LoadImage(g_hinst, _T("Resource/boss C5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_C = (HBITMAP)SelectObject(Boss_C_DC, Pic_Boss_C);
		ReleaseDC(hWnd, hDC);
		break;
	}

	case WM_PAINT: {
		hDC = BeginPaint(hWnd, &ps);

		FillRect(mainDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH)); // 최종 화면 지워서(하얀 사각형으로 덮어씌워서) 최신화

		if (stage % 2 == 0) { // 짝수는 보스맵이, 홀수는 휴식 공간이 출력됨
			// 보스맵
			SelectObject(MapDC, Pic_BossMap);
			StretchBlt(mainDC, 0, 0, rt.right, rt.bottom, MapDC, 0, 0, 1200, 1000, SRCCOPY); // 맵을 꽉 채워서 출력하기에 rt 구조체 활용
		}
		else {
			// 휴식 공간
			if (stage == 1) { // 맨 처음 스테이지일 경우
				SelectObject(MapDC, Pic_TutorialMap);
				StretchBlt(mainDC, 0, 0, rt.right, 700, MapDC, 0, 0, 1200, 700, SRCCOPY);
			}
			SelectObject(MapDC, Pic_RelaxMap);
			StretchBlt(mainDC, 0, 700, rt.right, 300, MapDC, 0, 0, 1200, 300, SRCCOPY); // 맵 하단부에 폭은 맵 가로 최대치에 높이 300 범위에 출력
			TransparentBlt(mainDC, portal.px(), portal.py(), 100, 100, PortalDC, 0, 0, 100, 100, RGB(255, 255, 255)); // 포탈
		}

		// Player
		player.print(mainDC, PlayerDC, BulletDC);

		// Item
		for (const auto& item : items) {
			switch (item.m_type) {
			case ITEM_TYPE::WEAPON:
				item.print(mainDC, WeaponDC);
				break;

			case ITEM_TYPE::HEAL:
				item.print(mainDC, HealDC);
				break;
			}
		}

		// 스테이지 2에 대한 처리
		if (stage == 2) {
			A.print_stage2();
			// 타이머 중복 생성 방지
			if (stage2_onoff == 0) {
				SetTimer(hWnd, A_animation, 200, NULL);
				SetTimer(hWnd, A_make_attack, 3000, NULL);
				SetTimer(hWnd, A_remove_attack, 5000, NULL);
				SetTimer(hWnd, A_shoot_attack, 100, NULL);
				A.make_attack(1);
				stage2_onoff = 1;
			}
		}

		// 스테이지 4에 대한 처리
		else if (stage == 4) {
			B.print_stage4();
			// 타이머 중복 생성 방지 (stage4 클리어 후 다음 스테이지로 넘어갈 때 stage4_onoff 다시 0으로 초기화해주기)
			if (stage4_onoff == 0) {
				SetTimer(hWnd, B_make_attack, 1000, NULL);
				SetTimer(hWnd, B_remove_attack, 3000, NULL);
				SetTimer(hWnd, B_shoot_attack, 10, NULL);
				SetTimer(hWnd, B_animation, 200, NULL);
				stage4_onoff = 1;
			}
		}

		// 스테이지 6에 대한 처리
		else if (stage == 6) {
			C.print_stage6();
			// 타이머 중복 생성 방지
			if (stage6_onoff == 0) {
				SetTimer(hWnd, C_Turning_attack, 100, NULL);
				SetTimer(hWnd, C_Moving, 150, NULL);
				SetTimer(hWnd, C_animation, 200, NULL);
				stage6_onoff = 1;
			}
		}

		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mainDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_TIMER: {
		// 보스 B 공격 생성 타이머
		if (wParam == B_make_attack) {
			std::uniform_int_distribution<int> ran_attack( 5, 7 );
			B.attack_pattern();
			B.count_attack(B.b_count() + 1);
			// 공격 횟수 충족 시 방향 변경 후 공격 (방향 중복되지 않도록 설정) (공격 횟수가 7을 넘기지 않도록 조정)
			if (B.b_count() == ran_attack(dre) or B.b_count() >= 7) {
				int num = B.b_direct();
				std::vector<int> numbers = { 1, 2, 3, 4 };
				for (auto it = numbers.begin(); it != numbers.end();) {
					if (*it == num) {
						it = numbers.erase(it);
					}
					else { ++it; }
				}
				std::uniform_int_distribution<int> ran_move(0, static_cast<int>(numbers.size() - 1));
				B.move_direct(numbers[ran_move(dre)]);
				B.count_attack(0);
			}
		}
		// 보스 B 공격 제거 타이머
		else if (wParam == B_remove_attack) {
			B.remove_attack();
		}
		// 보스 B 공격 이동 타이머
		else if (wParam == B_shoot_attack) {
			B.shoot_attack();
		}
		// 보스 B 애니메이션
		else if (wParam == B_animation) {
			Boss_B_ani++;
			Boss_B_ani = Boss_B_ani % 7;
		}
		// 보스 C 공격 구체 회전 타이머
		else if (wParam == C_Turning_attack) {
			C.windmill();
		}
		// 보스 이동
		else if (wParam == C_Moving) {
			C.moving();
		}
		// 보스 C 애니메이션
		else if (wParam == C_animation) {
			Boss_C_ani++;
			Boss_C_ani = Boss_C_ani % 5;
		}
		// 보스 A 애니메이션
		else if (wParam == A_animation) {
			Boss_A_ani++;
			Boss_A_ani = Boss_A_ani % 7;
		}
		// 보스 A 공격 생성
		else if (wParam == A_make_attack) {
			std::uniform_int_distribution<int> ran{ 1,2 };
			A.make_attack(ran(dre));
		}
		// 보스 공격 제거
		else if (wParam == A_remove_attack) {
			for (int i = 0; i < 20; i++) {
				A.remove_attack();
			}
		}
		// 보스 공격 이동
		else if (wParam == A_shoot_attack) {
			A.shoot_attack();
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_CHAR: {
		switch (wParam) {
		case 'w':
			player.m_anim_state = UP;
			break;

		case 's':
			player.m_anim_state = DOWN;
			break;

		case 'j':
			player.fire();
			break;

		case 'k': 
			if (player.m_on_platform && key_pressed['S']) {
				player.under_jump();
			} else {
				player.jump();
			}
			break;

		case 'l':
			player.roll();
			break;
		}
		break;
	}

	case WM_KEYDOWN: {
		if (!key_pressed[wParam]) {
			key_pressed[wParam] = true;

			switch (wParam) {
			case 'A':
				if (player.m_is_rolling) { break; }
				player.set_velocity(-5.0f, 0);
				player.m_anim_state = LEFT;
				break;

			case 'D':
				if (player.m_is_rolling) { break; }
				player.set_velocity(5.0f, 0);
				player.m_anim_state = RIGHT;
				break;

			default:
				break;
			}
		}
		break;
	}

	case WM_KEYUP: {
		if (key_pressed[wParam]) {
			key_pressed[wParam] = false;

			switch (wParam) {
			case 'A':
				if (player.m_is_rolling) { break; }
				player.set_velocity(5.0f, 0);
				break;

			case 'D':
				if (player.m_is_rolling) { break; }
				player.set_velocity(-5.0f, 0);
				break;

			default:
				break;
			}
		}
		break;
	}

	case WM_DESTROY: {
		// 각 DC 정보 초기화
		SelectObject(mainDC, old_hBitmap);
		SelectObject(MapDC, old_Pic_BossMap);
		SelectObject(Boss_A_DC, old_Pic_Boss_A);
		SelectObject(Boss_B_DC, old_Pic_Boss_B);
		SelectObject(Boss_C_DC, old_Pic_Boss_C);
		SelectObject(PlatformDC, old_Pic_Platform);
		SelectObject(PlayerDC, old_Pic_Player);
		SelectObject(WeaponDC, old_Pic_Weapon);
		SelectObject(BulletDC, old_Pic_Bullet);
		SelectObject(PortalDC, old_Pic_Portal);
		SelectObject(HealDC, old_Pic_Heal);
		SelectObject(DamageDC, old_Pic_Damage);

		// 브러시 초기화
		SelectObject(mainDC, old_Brush);
		DeleteObject(red_Brush);
		DeleteObject(blue_Brush);
		DeleteObject(lightgray_Brush);

		// 생성한 DC 및 비트맵 삭제
		DeleteDC(mainDC);
		DeleteDC(MapDC);
		DeleteDC(PlayerDC);
		DeleteDC(PlatformDC);
		DeleteDC(PortalDC);
		DeleteDC(HealDC);
		DeleteDC(Boss_A_DC);
		DeleteDC(Boss_B_DC);
		DeleteDC(Boss_C_DC);
		DeleteObject(hBitmap);
		DeleteObject(Pic_BossMap);
		DeleteObject(Pic_RelaxMap);
		for (int i = 0; i < 6; ++i) {
			DeleteObject(Pic_Player[i]);
		}
		DeleteObject(Pic_Platform);
		for (int i = 0; i < 4; ++i) {
			DeleteObject(Pic_Weapon[i]);
		}
		DeleteObject(Pic_Bullet);
		DeleteObject(Pic_Damage);
		DeleteObject(Pic_Player);
		DeleteObject(Pic_Platform);
		DeleteObject(Pic_Portal);
		DeleteObject(Pic_Heal);
		for (int i = 0; i < 7; i++) {
			DeleteObject(Pic_Boss_B_row[i]);
			DeleteObject(Pic_Boss_B_col[i]);
			DeleteObject(Pic_Boss_A[i]);
		}
		for (int i = 0; i < 5; i++) {
			DeleteObject(Pic_Boss_C[i]);
		}
		PostQuitMessage(0);
		return 0;
	}
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void worker() {
	while (running) {
		auto current_time = std::chrono::system_clock::now();

		if (16 < std::chrono::duration_cast<std::chrono::milliseconds>(current_time - update_time).count()) {
			// Player Move
			float old_player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;

			player.update();

			if (!player.m_is_rolling && player.m_was_rolling) {
				if (key_pressed['A']) {
					player.set_velocity(-5.0f, 0.0f);
				}

				if (key_pressed['D']) {
					player.set_velocity(5.0f, 0.0f);
				}

				player.m_was_rolling = false;
			}

			// Player Collision
			player.m_on_platform = false;

			switch (stage) {
			case 1:
			case 3:
			case 5:
				if (player_portal_collision(player, portal)) {
					player.m_x = 250.0f; player.m_y = 200.0f;
					items.clear();
					++stage;
				}

				player_map_collision();
				break;

			case 2:
				for (const auto& platform : A.Platform) {
					if (player_platform_collision(player, platform, old_player_bottom)) {
						player.set_on_platform(platform);
						break;
					}
				}

				player_map_collision();
				player_thorn_collision();
				player_boss_a_collision();

				bullet_map_collision();
				bullet_boss_a_collision();
				break;

			case 4:
				for (const auto& platform : B.Platform) {
					if (player_platform_collision(player, platform, old_player_bottom)) {
						player.set_on_platform(platform);
						break;
					}
				}

				player_map_collision();
				player_thorn_collision();
				player_boss_b_collision();

				bullet_map_collision();
				bullet_boss_b_collision();
				break;

			case 6:
				for (const auto& platform : C.Platform) {
					if (player_platform_collision(player, platform, old_player_bottom)) {
						player.set_on_platform(platform);
						break;
					}
				}

				player_map_collision();
				player_thorn_collision();
				player_boss_c_collision();

				bullet_map_collision();
				bullet_boss_c_collision();
				break;

			case 7:
				player_map_collision();
				break;
			}

			// Item Collision
			for (auto& item : items) {
				if (item.m_is_falling) {
					item.update();

					switch (stage) {
					case 1:
					case 3:
					case 5:
					case 7:
						if (675 < (item.m_y + Bmp_Heal.bmHeight)) {
							item.m_y = 675 - 50.0f;
						}
						break;

					case 2:
						for (const auto& platform : A.Platform) {
							if (item_platform_collision(item, platform)) {
								item.m_y = platform.y - 50.0f;
								item.m_is_falling = false;
								break;
							}
						}

						item_map_collision();
						break;

					case 4:
						for (const auto& platform : B.Platform) {
							if (item_platform_collision(item, platform)) {
								item.m_y = platform.y - 50.0f;
								item.m_is_falling = false;
								break;
							}
						}

						item_map_collision();
						break;

					case 6:
						for (const auto& platform : C.Platform) {
							if (item_platform_collision(item, platform)) {
								item.m_y = platform.y - 50.0f;
								item.m_is_falling = false;
								break;
							}
						}
						
						item_map_collision();
						break;
					}
				}
			}

			// Player - Item Collision
			for (auto iter = items.begin(); iter != items.end();) {
				if (player_item_collision(player, *iter)) {
					switch (iter->m_type) {
					case WEAPON:
						if (player.m_weapon) { 
							player.m_old_weapon.emplace_back(player.m_weapon);
							player.m_weapon = new Weapon(iter->m_bmp_index);
						}
						break;

					case HEAL:
						player.heal(player.m_max_hp / 2);
						break;
					}

					iter = items.erase(iter);
					continue;
				}
				++iter;
			}

			update_time = current_time;

			InvalidateRect(g_hWnd, NULL, FALSE);
		}

		// Spawn Item
		if (1 != (stage % 2)) {
			if (10 < std::chrono::duration_cast<std::chrono::seconds>(current_time - item_spawn_time).count()) {
				items.emplace_back(WEAPON);

				item_spawn_time = current_time;
			}
		}
	}
}

void change_stage() {
	player.m_x = 250.0f; player.m_y = 200.0f;

	player.m_weapon->m_bullets.clear();
	for (auto& weapon : player.m_old_weapon) {
		weapon->m_bullets.clear();
		delete weapon;
	}
	player.m_old_weapon.clear();

	items.clear();
	items.emplace_back(500.0f, 500.0f, ITEM_TYPE::HEAL);

	cumulative_damage = 0;

	++stage;

	if (7 == stage) {
		MessageBox(hWnd2, _T("승리!"), _T("정보"), MB_OK);
		reset();
	}
}

void reset() {
	player.reset();

	items.clear();

	cumulative_damage = 0;

	stage = 1;

	A.reset();
	B.reset();
	C.reset();

	KillTimer(hWnd2, A_animation);
	KillTimer(hWnd2, A_make_attack);
	KillTimer(hWnd2, A_remove_attack);
	KillTimer(hWnd2, A_shoot_attack);

	KillTimer(hWnd2, B_animation);
	KillTimer(hWnd2, B_make_attack);
	KillTimer(hWnd2, B_remove_attack);
	KillTimer(hWnd2, B_shoot_attack);

	KillTimer(hWnd2, C_animation);
	KillTimer(hWnd2, C_Turning_attack);
	KillTimer(hWnd2, C_Moving);
}

bool player_platform_collision(const Player& player, const POINT& platform, float old_player_bottom) {
	if (0.0f > player.m_y_velocity) {
		return false;
	}

	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;

	int platform_top = platform.y;
	int platform_bottom = platform.y + Bmp_Platform.bmHeight;
	int platform_left = platform.x;
	int platform_right = platform.x + Bmp_Platform.bmWidth;

	if ((platform_top + 1.0f) < old_player_bottom) {
		return false;
	}

	if ((player_top < platform_bottom) &&
		(player_bottom > platform_top) &&
		(player_left < platform_right) &&
		(player_right > platform_left)) {
		return true;
	}
	return false;
}

bool player_item_collision(const Player& player, const Item& item) {
	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;

	float item_top = item.m_y;
	float item_bottom = item.m_y + 50.0f;
	float item_left = item.m_x;
	float item_right = item.m_x + 50.0f;

	if ((player_top < item_bottom) &&
		(player_bottom > item_top) &&
		(player_left < item_right) &&
		(player_right > item_left)) {
		return true;
	}
	return false;
}

bool player_portal_collision(const Player& player, const Portal& portal) {
	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;

	float portal_top = static_cast<float>(portal.py());
	float portal_bottom = portal.py() + 100.0f;
	float portal_left = static_cast<float>(portal.px());
	float portal_right = portal.px() + 100.0f;

	if ((player_top < portal_bottom) &&
		(player_bottom > portal_top) &&
		(player_left < portal_right) &&
		(player_right > portal_left)) {
		return true;
	}
	return false;
}

void player_map_collision() {
	if (1 == (stage % 2)) {
		if (700.0f - Bmp_Player[player.m_anim_state].bmHeight < player.m_y) { player.set_on_ground(700.0f - Bmp_Player[player.m_anim_state].bmHeight); }
		if (0.0f > player.m_x) { player.m_x = 0.0f; }
		if (rt.right - Bmp_Player[player.m_anim_state].bmWidth < player.m_x) { player.m_x = static_cast<float>(rt.right) - Bmp_Player[player.m_anim_state].bmWidth; }
	} else {
		if (30.0f > player.m_y) { player.m_y = 30.0f; }
		if (rt.bottom - 30.0f - Bmp_Player[player.m_anim_state].bmHeight < player.m_y) { player.set_on_ground(rt.bottom - 30.0f - Bmp_Player[player.m_anim_state].bmHeight); }
		if (20.0f > player.m_x) { player.m_x = 20.0f; }
		if (rt.right - 20.0f - Bmp_Player[player.m_anim_state].bmWidth < player.m_x) { player.m_x = rt.right - 20.0f - Bmp_Player[player.m_anim_state].bmWidth; }
	}
}

void player_thorn_collision() {
	if (player.m_invincible || player.m_is_rolling) {
		return;
	}

	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;


	if (rt.bottom - 65.0f < player_bottom) {
		player.damaged();
		if (0 == player.m_hp) {
			reset();
			return;
		}
		player.m_y_velocity = -19.8f;
		player.m_double_jump = true;
		return;
	}

	if ((65.0f > player_top) ||
		(50.0f > player_left) || 
		(rt.right - 50.0f < player_right)) {
		player.damaged();
		if (0 == player.m_hp) {
			reset();
			return;
		}
		player.m_double_jump = true;
		return;
	}
}

void player_boss_a_collision() {
	if (player.m_invincible || player.m_is_rolling) {
		return;
	}

	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;

	float boss_top = static_cast<float>(A.y);
	float boss_bottom = A.y + 600.0f;
	float boss_left = static_cast<float>(A.x);
	float boss_right = A.x + 100.0f;

	// Player - Boss Collision
	if ((player_top < boss_bottom) &&
		(player_bottom > boss_top) &&
		(player_left < boss_right) &&
		(player_right > boss_left)) {
		player.damaged();
		if (0 == player.m_hp) {
			reset();
		}
		return;
	}

	// Player - Attack Collision
	for (int i = 0; i < A.attack.size(); i = i + 20) {
		POINT* P1 = A.attack[i].a_point();
		POINT* P2 = A.attack[i + 19].a_point();

		float attack_top = static_cast<float>(min(min(P1[0].y, P1[1].y), min(P1[2].y, min(P2[0].y, min(P2[1].y, P2[2].y)))));
		float attack_bottom = static_cast<float>(max(max(P1[0].y, P1[1].y), max(P1[2].y, max(P2[0].y, max(P2[1].y, P2[2].y)))));
		float attack_left = static_cast<float>(min(min(P1[0].x, P1[1].x), min(P1[2].x, min(P2[0].x, min(P2[1].x, P2[2].x)))));
		float attack_right = static_cast<float>(max(max(P1[0].x, P1[1].x), max(P1[2].x, max(P2[0].x, max(P2[1].x, P2[2].x)))));

		if ((player_top < attack_bottom) &&
			(player_bottom > attack_top) &&
			(player_left < attack_right) &&
			(player_right > attack_left)) {
			player.damaged();
			if (0 == player.m_hp) {
				reset();
			}
			return;
		}
	}
}

void player_boss_b_collision() {
	if (player.m_invincible || player.m_is_rolling) {
		return;
	}

	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;

	float boss_top, boss_bottom, boss_left, boss_right;

	if (B.b_direct() == 1 || B.b_direct() == 2) {
		boss_top = static_cast<float>(B.b_y());
		boss_bottom = B.b_y() + 100.0f;
		boss_left = static_cast<float>(B.b_x());
		boss_right = B.b_x() + 1000.0f;
	} else {
		boss_top = static_cast<float>(B.b_y());
		boss_bottom = B.b_y() + 800.0f;
		boss_left = static_cast<float>(B.b_x());
		boss_right = B.b_x() + 100.0f;
	}

	// Player - Boss Collision
	if ((player_top < boss_bottom) &&
		(player_bottom > boss_top) &&
		(player_left < boss_right) &&
		(player_right > boss_left)) {
		player.damaged();
		if (0 == player.m_hp) {
			reset();
		}
		return;
	}

	// Player - Attack Collision
	for (auto& attack : B.attack) {
		POINT* P = attack.a_Point();

		float attack_top = static_cast<float>(min(min(P[0].y, P[1].y), P[2].y));
		float attack_bottom = static_cast<float>(max(max(P[0].y, P[1].y), P[2].y));
		float attack_left = static_cast<float>(min(min(P[0].x, P[1].x), P[2].x));
		float attack_right = static_cast<float>(max(max(P[0].x, P[1].x), P[2].x));

		if ((player_top < attack_bottom) &&
			(player_bottom > attack_top) &&
			(player_left < attack_right) &&
			(player_right > attack_left)) {
			player.damaged();
			if (0 == player.m_hp) {
				reset();
			}
			return;
		}
	}
}

void player_boss_c_collision() {
	if (player.m_invincible || player.m_is_rolling) {
		return;
	}

	float player_top = player.m_y;
	float player_bottom = player.m_y + Bmp_Player[player.m_anim_state].bmHeight;
	float player_left = player.m_x;
	float player_right = player.m_x + Bmp_Player[player.m_anim_state].bmWidth;

	float boss_top = static_cast<float>(C.y);
	float boss_bottom = C.y + 200.0f;
	float boss_left = static_cast<float>(C.x);
	float boss_right = C.x + 200.0f;

	// Player - Boss Collision
	if ((player_top < boss_bottom) &&
		(player_bottom > boss_top) &&
		(player_left < boss_right) &&
		(player_right > boss_left)) {
		player.damaged();
		if (0 == player.m_hp) {
			reset();
		}
		return;
	}

	// Player - Attack Collision
	int size = 20;
	for (auto& attack : C.attack) {
		float attack_top = static_cast<float>(attack.ay() - size);
		float attack_bottom = static_cast<float>(attack.ay() + size);
		float attack_left = static_cast<float>(attack.ax() - size);
		float attack_right = static_cast<float>(attack.ax() + size);

		if ((player_top < attack_bottom) &&
			(player_bottom > attack_top) &&
			(player_left < attack_right) &&
			(player_right > attack_left)) {
			player.damaged();
			if (0 == player.m_hp) {
				reset();
			}
			return;
		}
	}
}

void bullet_map_collision() {
	for (auto iter = player.m_weapon->m_bullets.begin(); iter != player.m_weapon->m_bullets.end();) {
		if ((30.0f > iter->m_y) || 
			(rt.bottom - 30.0f < iter->m_y) ||
			(20.0f > iter->m_x) ||
			(rt.right - 20.0f < iter->m_x)) {
			iter = player.m_weapon->m_bullets.erase(iter);
			continue;
		}
		++iter;
	}

	for (auto weapon : player.m_old_weapon) {
		for (auto iter = weapon->m_bullets.begin(); iter != weapon->m_bullets.end();) {
			if ((30.0f > iter->m_y) ||
				(rt.bottom - 30.0f < iter->m_y) ||
				(20.0f > iter->m_x) ||
				(rt.right - 20.0f < iter->m_x)) {
				iter = weapon->m_bullets.erase(iter);
				continue;
			}
			++iter;
		}
	}
}

void bullet_boss_a_collision() {
	float boss_top = static_cast<float>(A.y);
	float boss_bottom = A.y + 600.0f;
	float boss_left = static_cast<float>(A.x);
	float boss_right = A.x + 100.0f;

	for (auto iter = player.m_weapon->m_bullets.begin(); iter != player.m_weapon->m_bullets.end();) {
		if (iter->m_hit) {
			++iter;
			continue;
		}

		if ((boss_left < iter->m_x) &&
			(iter->m_x < boss_right) &&
			(boss_top < iter->m_y) &&
			(iter->m_y < boss_bottom)) {
			A.hp -= player.m_weapon->m_attack;
			cumulative_damage += player.m_weapon->m_attack;
			iter->hit();

			if (150 >= A.hp) {
				change_stage();
				return;
			}

			if (250 <= cumulative_damage) {
				cumulative_damage = (cumulative_damage % 250);
				items.emplace_back(ITEM_TYPE::HEAL);
			}
			continue;
		}
		++iter;
	}

	for (auto weapon : player.m_old_weapon) {
		for (auto iter = weapon->m_bullets.begin(); iter != weapon->m_bullets.end();) {
			if (iter->m_hit) {
				++iter;
				continue;
			}

			if ((boss_left < iter->m_x) &&
				(iter->m_x < boss_right) &&
				(boss_top < iter->m_y) &&
				(iter->m_y < boss_bottom)) {
				A.hp -= weapon->m_attack;
				cumulative_damage += player.m_weapon->m_attack;
				iter->hit();

				if (150 >= A.hp) {
					change_stage();
					return;
				}

				if (250 <= cumulative_damage) {
					cumulative_damage = (cumulative_damage % 250);
					items.emplace_back(ITEM_TYPE::HEAL);
				}
				continue;
			}
			++iter;
		}
	}
}

void bullet_boss_b_collision() {
	float boss_top, boss_bottom, boss_left, boss_right;
	if (B.b_direct() == 1 || B.b_direct() == 2) {
		boss_top = static_cast<float>(B.b_y());
		boss_bottom = B.b_y() + 100.0f;
		boss_left = static_cast<float>(B.b_x());
		boss_right = B.b_x() + 1000.0f;
	}
	else {
		boss_top = static_cast<float>(B.b_y());
		boss_bottom = B.b_y() + 800.0f;
		boss_left = static_cast<float>(B.b_x());
		boss_right = B.b_x() + 100.0f;
	}

	for (auto iter = player.m_weapon->m_bullets.begin(); iter != player.m_weapon->m_bullets.end();) {
		if (iter->m_hit) {
			++iter;
			continue;
		}

		if ((boss_left < iter->m_x) &&
			(iter->m_x < boss_right) &&
			(boss_top < iter->m_y) &&
			(iter->m_y < boss_bottom)) {
			B.hp -= player.m_weapon->m_attack;
			cumulative_damage += player.m_weapon->m_attack;
			iter->hit();

			if (150 >= B.hp) {
				change_stage();
				return;
			}

			if (250 <= cumulative_damage) {
				cumulative_damage = (cumulative_damage % 250);
				items.emplace_back(ITEM_TYPE::HEAL);
			}
			continue;
		}
		++iter;
	}

	for (auto weapon : player.m_old_weapon) {
		for (auto iter = weapon->m_bullets.begin(); iter != weapon->m_bullets.end();) {
			if (iter->m_hit) {
				++iter;
				continue;
			}

			if ((boss_left < iter->m_x) &&
				(iter->m_x < boss_right) &&
				(boss_top < iter->m_y) &&
				(iter->m_y < boss_bottom)) {
				B.hp -= weapon->m_attack;
				cumulative_damage += player.m_weapon->m_attack;
				iter->hit();

				if (150 >= B.hp) {
					change_stage();
					return;
				}

				if (250 <= cumulative_damage) {
					cumulative_damage = (cumulative_damage % 250);
					items.emplace_back(ITEM_TYPE::HEAL);
				}
				continue;
			}
			++iter;
		}
	}
}

void bullet_boss_c_collision() {
	float boss_top = static_cast<float>(C.y);
	float boss_bottom = C.y + 200.0f;
	float boss_left = static_cast<float>(C.x);
	float boss_right = C.x + 200.0f;

	for (auto iter = player.m_weapon->m_bullets.begin(); iter != player.m_weapon->m_bullets.end();) {
		if (iter->m_hit) {
			++iter;
			continue;
		}

		if ((boss_left < iter->m_x) &&
			(iter->m_x < boss_right) &&
			(boss_top < iter->m_y) &&
			(iter->m_y < boss_bottom)) {
			C.hp -= player.m_weapon->m_attack;
			cumulative_damage += player.m_weapon->m_attack;
			iter->hit();

			if (150 >= C.hp) {
				change_stage();
				return;
			}

			if (250 <= cumulative_damage) {
				cumulative_damage = (cumulative_damage % 250);
				items.emplace_back(ITEM_TYPE::HEAL);
			}
			continue;
		}
		++iter;
	}

	for (auto weapon : player.m_old_weapon) {
		for (auto iter = weapon->m_bullets.begin(); iter != weapon->m_bullets.end();) {
			if (iter->m_hit) {
				++iter;
				continue;
			}

			if ((boss_left < iter->m_x) &&
				(iter->m_x < boss_right) &&
				(boss_top < iter->m_y) &&
				(iter->m_y < boss_bottom)) {
				C.hp -= weapon->m_attack;
				cumulative_damage += player.m_weapon->m_attack;
				iter->hit();

				if (150 >= C.hp) {
					change_stage();
					return;
				}

				if (250 <= cumulative_damage) {
					cumulative_damage = (cumulative_damage % 250);
					items.emplace_back(ITEM_TYPE::HEAL);
				}
				continue;
			}
			++iter;
		}
	}
}

void item_map_collision() {
	for (auto& item : items) {
		if (rt.bottom - 30.0f - 50.0f < item.m_y) { 
			item.m_y = rt.bottom - 30.0f - 50.0f;
		}
	}
}

bool item_platform_collision(const Item& item, const POINT& platform) {
	float item_top = item.m_y;
	float item_bottom = item.m_y + 50.0f;
	float item_left = item.m_x;
	float item_right = item.m_x + 50.0f;

	int platform_top = platform.y;
	int platform_bottom = platform.y + Bmp_Platform.bmHeight;
	int platform_left = platform.x;
	int platform_right = platform.x + Bmp_Platform.bmWidth;

	if ((item_top < platform_bottom) &&
		(item_bottom > platform_top) &&
		(item_left < platform_right) &&
		(item_right > platform_left)) {
		return true;
	}
	return false;
}
