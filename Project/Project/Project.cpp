#include "Common.h"
#include "Boss B.h"
#include "Boss C.h"
#include "CPlayer.h"
#include "CItem.h"

//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

Player player;
std::vector<Item> items;
bool key_pressed[256] = { false };

bool running = true;
auto update_time = std::chrono::system_clock::now();
auto item_spawn_time = std::chrono::system_clock::now();

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

void worker();
bool player_platform_collision(const Player& player, const POINT& platform, float old_player_bottom);
bool item_platform_collision(const Item& item, const POINT& platform);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdParam, int nCmdShow) { // WinMain부분에 주석이 일치하지 않는다는 오류는 원래 잘 뜸. 무시해도 됨.
	HWND hWnd;
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
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1200, 1000, NULL, (HMENU)NULL, hInstance, NULL);

	std::thread worker_thread(worker);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
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
		Pic_BossMap = (HBITMAP)LoadImage(g_hinst, _T("boss stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_RelaxMap = (HBITMAP)LoadImage(g_hinst, _T("relax stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_BossMap = (HBITMAP)SelectObject(MapDC, Pic_BossMap);

		// PlayerDC에 대한 비트맵 생성 및 설정
		PlayerDC = CreateCompatibleDC(hDC);
		Pic_Player[IDLE] = (HBITMAP)LoadImage(g_hinst, _T("player.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[UP] = (HBITMAP)LoadImage(g_hinst, _T("player_up.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[DOWN] = (HBITMAP)LoadImage(g_hinst, _T("player_down.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[LEFT] = (HBITMAP)LoadImage(g_hinst, _T("player_left.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[RIGHT] = (HBITMAP)LoadImage(g_hinst, _T("player_right.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Player[DODGE] = (HBITMAP)LoadImage(g_hinst, _T("player_dodge.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Player = (HBITMAP)SelectObject(PlayerDC, Pic_Player[player.m_anim_state]);
		for (int i = 0; i < 6; ++i) {
			GetObject(Pic_Player[i], sizeof(BITMAP), &Bmp_Player[i]);
		}

		// PlatformDC에 대한 비트맵 생성 및 설정
		PlatformDC = CreateCompatibleDC(hDC);
		Pic_Platform = (HBITMAP)LoadImage(g_hinst, _T("platform.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Platform = (HBITMAP)SelectObject(PlatformDC, Pic_Platform);
		GetObject(Pic_Platform, sizeof(BITMAP), &Bmp_Platform);

		// WeaponDC에 대한 비트맵 생성 및 설정
		WeaponDC = CreateCompatibleDC(hDC);
		Pic_Weapon[PISTOL] = (HBITMAP)LoadImage(g_hinst, _T("pistol.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SMG] = (HBITMAP)LoadImage(g_hinst, _T("smg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SHOTGUN] = (HBITMAP)LoadImage(g_hinst, _T("shotgun.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SNIPER] = (HBITMAP)LoadImage(g_hinst, _T("sniper.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		for (int i = 0; i < 4; ++i) {
			GetObject(Pic_Weapon[i], sizeof(BITMAP), &Bmp_Weapon[i]);
		}
		old_Pic_Weapon = (HBITMAP)SelectObject(WeaponDC, Pic_Weapon[0]);

		// Boss_B_DC에 대한 비트맵 생성 및 설정
		Boss_B_DC = CreateCompatibleDC(hDC);
		Pic_Boss_B_row[0] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[1] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[2] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[3] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[4] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[5] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_row[6] = (HBITMAP)LoadImage(g_hinst, _T("boss B_row7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[0] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[1] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[2] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[3] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[4] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[5] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col[6] = (HBITMAP)LoadImage(g_hinst, _T("boss B_col7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_B = (HBITMAP)SelectObject(Boss_B_DC, Pic_Boss_B_row[0]);

		// Boss_C_DC에 대한 비트맵 생성 및 설정
		Boss_C_DC = CreateCompatibleDC(hDC);
		Pic_Boss_C[0] = (HBITMAP)LoadImage(g_hinst, _T("boss C1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[1] = (HBITMAP)LoadImage(g_hinst, _T("boss C2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[2] = (HBITMAP)LoadImage(g_hinst, _T("boss C3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[3] = (HBITMAP)LoadImage(g_hinst, _T("boss C4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_C[4] = (HBITMAP)LoadImage(g_hinst, _T("boss C5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_C = (HBITMAP)SelectObject(Boss_C_DC, Pic_Boss_C);
		ReleaseDC(hWnd, hDC);
		break;
	}

	case WM_PAINT: {
		hDC = BeginPaint(hWnd, &ps);
		old_Brush = (HBRUSH)SelectObject(hDC, red_Brush); // 기본 브러시 저장
		FillRect(mainDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH)); // 최종 화면 지워서(하얀 사각형으로 덮어씌워서) 최신화
		if (stage % 2 == 0) { // 짝수는 보스맵이, 홀수는 휴식 공간이 출력됨
			SelectObject(MapDC, Pic_BossMap);
			StretchBlt(mainDC, 0, 0, rt.right, rt.bottom, MapDC, 0, 0, 1200, 1000, SRCCOPY); // 맵을 꽉 채워서 출력하기에 rt 구조체 활용
		}
		else {
			SelectObject(MapDC, Pic_RelaxMap);
			StretchBlt(mainDC, 0, 700, rt.right, 300, MapDC, 0, 0, 1200, 300, SRCCOPY); // 맵 하단부에 폭은 맵 가로 최대치에 높이 300 범위에 출력
		}

		// Player
		int anim_index = player.m_anim_state;
		SelectObject(PlayerDC, Pic_Player[anim_index]);
		TransparentBlt(mainDC, static_cast<int>(player.m_x), static_cast<int>(player.m_y), Bmp_Player[anim_index].bmWidth, Bmp_Player[anim_index].bmHeight,
			PlayerDC, 0, 0, Bmp_Player[anim_index].bmWidth, Bmp_Player[anim_index].bmHeight, RGB(255, 255, 255));

		// Item
		for (const auto& item : items) {
			item.print(mainDC, WeaponDC);
		}

		// 스테이지 4에 대한 처리
		if (stage == 4) {
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
				std::uniform_int_distribution<int> ran_move( 0, numbers.size() - 1);
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
		// 피격 시 타이머 (0.3초 후에 해당 타이머가 실행되면서 보스의 출력 방식을 원래대로 되돌리고 타이머 삭제)
		// 즉, 보스는 피격 되자마자 색이 반전되며, B_attacked 타이머를 설정한다. 이후 0.3초 후에 B_attacked 타이머를 통해 색이 원래대로 돌아오는 것)
		// 피격 체크에 SetTImer(hWnd, B_attacked, 300, NULL); 작성해주기
		// 피격 시 색 반전은 일단 보류하기
		else if (wParam == B_attacked) {
			B.print_type_change();
			KillTimer(hWnd, B_attacked);
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
		SelectObject(Boss_B_DC, old_Pic_Boss_B);
		SelectObject(Boss_C_DC, old_Pic_Boss_C);
		SelectObject(PlatformDC, old_Pic_Platform);
		SelectObject(PlayerDC, old_Pic_Player);
		SelectObject(WeaponDC, old_Pic_Weapon);
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
		DeleteDC(Boss_B_DC);
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
		for (int i = 0; i < 7; i++) {
			DeleteObject(Pic_Boss_B_row[i]);
			DeleteObject(Pic_Boss_B_col[i]);
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
			// Player
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

			player.m_on_platform = false;

			switch (stage) {
			case 4:
				for (const auto& platform : B.Platform) {
					if (player_platform_collision(player, platform, old_player_bottom)) {
						player.set_on_platform(platform);
						break;
					}
				}
				break;

			case 6:
				for (const auto& platform : C.Platform) {
					if (player_platform_collision(player, platform, old_player_bottom)) {
						player.set_on_platform(platform);
						break;
					}
				}
				break;
			}

			// Item
			for (auto& item : items) {
				if (item.m_is_falling) {
					item.update();

					switch (stage) {
					case 4:
						for (const auto& platform : B.Platform) {
							if (item_platform_collision(item, platform)) {
								item.m_y = platform.y - 50.0f;
								item.m_is_falling = false;
								break;
							}
						}
						break;

					case 6:
						for (const auto& platform : C.Platform) {
							if (item_platform_collision(item, platform)) {
								item.m_y = platform.y - 50.0f;
								item.m_is_falling = false;
								break;
							}
						}
						break;
					}
				}
			}

			update_time = current_time;
		}

		if (10 < std::chrono::duration_cast<std::chrono::seconds>(current_time - item_spawn_time).count()) {
			items.emplace_back(WEAPON);

			item_spawn_time = current_time;
		}
	}
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
