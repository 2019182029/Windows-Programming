#include "Common.h"
#include "Boss A.h"
#include "Boss B.h"
#include "Boss C.h"
#include "CPlayer.h"
#include "CItem.h"
#include "Element.h"

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
bool player_item_collision(const Player& player, const Item& item);
bool item_platform_collision(const Item& item, const POINT& platform);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpszCmdParam, int nCmdShow) { // WinMain�κп� �ּ��� ��ġ���� �ʴ´ٴ� ������ ���� �� ��. �����ص� ��.
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	LPCTSTR lpszClass = L"Window Class Name";
	LPCTSTR lpszWindowName = L"windows program";
	g_hinst = hInstance; // hInstance�� ���� ���� ���� ���� ���ø����̼��� �ν��Ͻ� �ڵ��� ����

	// WndClass�� ��� �������� ���
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	WndClass.lpfnWndProc = (WNDPROC)WndProc; // lpfnWndProc�� �����ϱ� ���� WndProc�� WNDPROC �ڷ����� �Լ��� ��ȯ �� ����. �׳ɵ� ���� ����������, Ȥ�� �� ������ ����� ��.
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ������ �� �������ִ� �ڵ� GetStockObject�� ���ڷ� brush�� �ָ� ��
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	// ������ �Ӽ��� �� �� '|' �� or�� ��Ʈ �����ڸ� Ȱ���Ͽ� �Ӽ��� �ָ� �ȴ�. WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_HSCROLL | WS_VSCROLL | WS_THICKFRAME �̷� ������
	// ���� ����: ������ Ŭ���� �̸�, ������ Ÿ��Ʋ �̸�, ������ ��Ÿ��, ������ x��ǥ, ������ y��ǥ, ������ ���� ũ��, ������ ���� ũ��, �θ� ������ �ڵ�(�θ� ������ NULL), ������ ��ܿ� �ٴ� �޴��� �ڵ�(�޴� ������ NULL), WinMain���� ���� �ν��Ͻ� �ڵ�
	// x�� ���� Ŀ������ â�� ��������, y�� ���� Ŀ������ â�� �Ʒ��� �������� �ȴ�.
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

		// �귯�� ����
		red_Brush = CreateSolidBrush(RGB(255, 0, 0));
		blue_Brush = CreateSolidBrush(RGB(0, 0, 255));
		lightgray_Brush = CreateSolidBrush(RGB(240, 240, 240));

		// mainDC�� ���� ��Ʈ�� ���� �� ����
		mainDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, rt.right, rt.bottom);
		old_hBitmap = (HBITMAP)SelectObject(mainDC, (HBITMAP)hBitmap);
		SelectObject(mainDC, (HBITMAP)hBitmap);

		// MapDc�� ���� ��Ʈ�� ����
		MapDC = CreateCompatibleDC(hDC);
		Pic_BossMap = (HBITMAP)LoadImage(g_hinst, _T("boss stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_RelaxMap = (HBITMAP)LoadImage(g_hinst, _T("relax stage.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_BossMap = (HBITMAP)SelectObject(MapDC, Pic_BossMap);

		// PlayerDC�� ���� ��Ʈ�� ���� �� ����
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

		// PlatformDC�� ���� ��Ʈ�� ���� �� ����
		PlatformDC = CreateCompatibleDC(hDC);
		Pic_Platform = (HBITMAP)LoadImage(g_hinst, _T("platform.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Platform = (HBITMAP)SelectObject(PlatformDC, Pic_Platform);
		GetObject(Pic_Platform, sizeof(BITMAP), &Bmp_Platform);

		// WeaponDC�� ���� ��Ʈ�� ���� �� ����
		WeaponDC = CreateCompatibleDC(hDC);
		Pic_Weapon[PISTOL] = (HBITMAP)LoadImage(g_hinst, _T("pistol.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SMG] = (HBITMAP)LoadImage(g_hinst, _T("smg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SHOTGUN] = (HBITMAP)LoadImage(g_hinst, _T("shotgun.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Weapon[SNIPER] = (HBITMAP)LoadImage(g_hinst, _T("sniper.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		for (int i = 0; i < 4; ++i) {
			GetObject(Pic_Weapon[i], sizeof(BITMAP), &Bmp_Weapon[i]);
		}
		old_Pic_Weapon = (HBITMAP)SelectObject(WeaponDC, Pic_Weapon[0]);

		// BulletDC�� ���� ��Ʈ�� ���� �� ����
		BulletDC = CreateCompatibleDC(hDC);
		Pic_Bullet = (HBITMAP)LoadImage(g_hinst, _T("bullet.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Bullet = (HBITMAP)SelectObject(BulletDC, Pic_Bullet);
		GetObject(Pic_Bullet, sizeof(BITMAP), &Bmp_Bullet);

		// PortalDC�� ���� ��Ʈ�� ���� �� ����
		PortalDC = CreateCompatibleDC(hDC);
		Pic_Portal = (HBITMAP)LoadImage(g_hinst, _T("portal.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Portal = (HBITMAP)SelectObject(PortalDC, Pic_Portal);

		// HealDC�� ���� ��Ʈ�� ���� �� ����
		HealDC = CreateCompatibleDC(hDC);
		Pic_Heal = (HBITMAP)LoadImage(g_hinst, _T("healing_item.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Heal = (HBITMAP)SelectObject(HealDC, Pic_Heal);

		// Boss_A_DC�� ���� ��Ʈ�� ���� �� ����
		Boss_A_DC = CreateCompatibleDC(hDC);
		Pic_Boss_A[0] = (HBITMAP)LoadImage(g_hinst, _T("boss A1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[1] = (HBITMAP)LoadImage(g_hinst, _T("boss A2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[2] = (HBITMAP)LoadImage(g_hinst, _T("boss A3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[3] = (HBITMAP)LoadImage(g_hinst, _T("boss A4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[4] = (HBITMAP)LoadImage(g_hinst, _T("boss A5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[5] = (HBITMAP)LoadImage(g_hinst, _T("boss A6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[6] = (HBITMAP)LoadImage(g_hinst, _T("boss A7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_A = (HBITMAP)SelectObject(Boss_A_DC, Pic_Boss_A);

		// Boss_B_DC�� ���� ��Ʈ�� ���� �� ����
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

		// Boss_C_DC�� ���� ��Ʈ�� ���� �� ����
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
		old_Brush = (HBRUSH)SelectObject(hDC, red_Brush); // �⺻ �귯�� ����
		FillRect(mainDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH)); // ���� ȭ�� ������(�Ͼ� �簢������ �������) �ֽ�ȭ
		if (stage % 2 == 0) { // ¦���� ��������, Ȧ���� �޽� ������ ��µ�
			// ������
			SelectObject(MapDC, Pic_BossMap);
			StretchBlt(mainDC, 0, 0, rt.right, rt.bottom, MapDC, 0, 0, 1200, 1000, SRCCOPY); // ���� �� ä���� ����ϱ⿡ rt ����ü Ȱ��
		}
		else {
			// �޽� ����
			SelectObject(MapDC, Pic_RelaxMap);
			StretchBlt(mainDC, 0, 700, rt.right, 300, MapDC, 0, 0, 1200, 300, SRCCOPY); // �� �ϴܺο� ���� �� ���� �ִ�ġ�� ���� 300 ������ ���
			TransparentBlt(mainDC, portal.px(), portal.py(), 100, 100, PortalDC, 0, 0, 100, 100, RGB(255, 255, 255)); // ��Ż
			heal.emplace_back(500, 600);
			TransparentBlt(mainDC, heal.begin()->h_x(), heal.begin()->h_y(), 50, 50, HealDC, 0, 0, 50, 50, RGB(255, 255, 255)); // ȸ�� ����
		}

		// Player
		player.print(mainDC, PlayerDC, BulletDC);

		// Item
		for (const auto& item : items) {
			item.print(mainDC, WeaponDC);
		}

		// ���
		StretchBlt(mainDC, 500, 500, 30, 30, PlayerDC, 0, 0, 30, 30, SRCCOPY);

		// �������� 2�� ���� ó��
		if (stage == 2) {
			A.print_stage2();
			// Ÿ�̸� �ߺ� ���� ����
			if (stage2_onoff == 0) {
				SetTimer(hWnd, A_animation, 200, NULL);
				SetTimer(hWnd, A_make_attack, 3000, NULL);
				SetTimer(hWnd, A_remove_attack, 5000, NULL);
				SetTimer(hWnd, A_shoot_attack, 100, NULL);
				A.make_attack(1);
				stage2_onoff = 1;
			}
		}

		// �������� 4�� ���� ó��
		else if (stage == 4) {
			B.print_stage4();
			// Ÿ�̸� �ߺ� ���� ���� (stage4 Ŭ���� �� ���� ���������� �Ѿ �� stage4_onoff �ٽ� 0���� �ʱ�ȭ���ֱ�)
			if (stage4_onoff == 0) {
				SetTimer(hWnd, B_make_attack, 1000, NULL);
				SetTimer(hWnd, B_remove_attack, 3000, NULL);
				SetTimer(hWnd, B_shoot_attack, 10, NULL);
				SetTimer(hWnd, B_animation, 200, NULL);
				stage4_onoff = 1;
			}
		}

		// �������� 6�� ���� ó��
		else if (stage == 6) {
			C.print_stage6();
			// Ÿ�̸� �ߺ� ���� ����
			if (stage6_onoff == 0) {
				SetTimer(hWnd, C_Turning_attack, 100, NULL);
				SetTimer(hWnd, C_Moving, 150, NULL);
				SetTimer(hWnd, C_animation, 200, NULL);
				stage6_onoff = 1;
			}
		}

		// ������ ������ Ŭ���� ���� �� �¸������� ��Ÿ���� �޽��� �ڽ� ���
		if (stage == 7) {
			MessageBox(hWnd, _T("�¸�!"), _T("����"), MB_OK);
		}

		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mainDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_TIMER: {
		// ���� B ���� ���� Ÿ�̸�
		if (wParam == B_make_attack) {
			std::uniform_int_distribution<int> ran_attack( 5, 7 );
			B.attack_pattern();
			B.count_attack(B.b_count() + 1);
			// ���� Ƚ�� ���� �� ���� ���� �� ���� (���� �ߺ����� �ʵ��� ����) (���� Ƚ���� 7�� �ѱ��� �ʵ��� ����)
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
		// ���� B ���� ���� Ÿ�̸�
		else if (wParam == B_remove_attack) {
			B.remove_attack();
		}
		// ���� B ���� �̵� Ÿ�̸�
		else if (wParam == B_shoot_attack) {
			B.shoot_attack();
		}
		// ���� B �ִϸ��̼�
		else if (wParam == B_animation) {
			Boss_B_ani++;
			Boss_B_ani = Boss_B_ani % 7;
		}
		// ���� C ���� ��ü ȸ�� Ÿ�̸�
		else if (wParam == C_Turning_attack) {
			C.windmill();
		}
		// ���� �̵�
		else if (wParam == C_Moving) {
			C.moving();
		}
		// ���� C �ִϸ��̼�
		else if (wParam == C_animation) {
			Boss_C_ani++;
			Boss_C_ani = Boss_C_ani % 5;
		}
		// ���� A �ִϸ��̼�
		else if (wParam == A_animation) {
			Boss_A_ani++;
			Boss_A_ani = Boss_A_ani % 7;
		}
		// ���� A ���� ����
		else if (wParam == A_make_attack) {
			std::uniform_int_distribution<int> ran{ 1,2 };
			A.make_attack(ran(dre));
		}
		// ���� ���� ����
		else if (wParam == A_remove_attack) {
			for (int i = 0; i < 20; i++) {
				A.remove_attack();
			}
		}
		// ���� ���� �̵�
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
		// �� DC ���� �ʱ�ȭ
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

		// �귯�� �ʱ�ȭ
		SelectObject(mainDC, old_Brush);
		DeleteObject(red_Brush);
		DeleteObject(blue_Brush);
		DeleteObject(lightgray_Brush);
		// ������ DC �� ��Ʈ�� ����
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

			for (auto iter = items.begin(); iter != items.end();) {
				if (player_item_collision(player, *iter)) {
					Weapon* old_weapon = nullptr;
					if (player.m_weapon) { old_weapon = player.m_weapon; }

					switch (iter->m_type) {
					case WEAPON:
						player.m_weapon = new Weapon(iter->m_bmp_index);
						break;
					}

					if (old_weapon) { delete old_weapon; }

					iter = items.erase(iter);
					continue;
				}
				++iter;
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
