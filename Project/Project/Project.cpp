#include "Common.h"
#include "Boss B.h"
#include "Boss C.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

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

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
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
		Pic_Player = (HBITMAP)LoadImage(g_hinst, _T("player.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Player = (HBITMAP)SelectObject(PlayerDC, Pic_Player);
		SelectObject(PlayerDC, Pic_Player);
		// PlatformDC�� ���� ��Ʈ�� ���� �� ����
		PlatformDC = CreateCompatibleDC(hDC);
		Pic_Platform = (HBITMAP)LoadImage(g_hinst, _T("platform.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Platform = (HBITMAP)SelectObject(PlatformDC, Pic_Platform);
		// Boss_B_DC�� ���� ��Ʈ�� ���� �� ����
		Boss_B_DC = CreateCompatibleDC(hDC);
		Pic_Boss_B_row = (HBITMAP)LoadImage(g_hinst, _T("boss B_row.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_B_col = (HBITMAP)LoadImage(g_hinst, _T("boss B_col.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_B = (HBITMAP)SelectObject(Boss_B_DC, Pic_Boss_B_row);
		// Boss_C_DC�� ���� ��Ʈ�� ���� �� ����
		Boss_C_DC = CreateCompatibleDC(hDC);
		Pic_Boss_C = (HBITMAP)LoadImage(g_hinst, _T("boss C.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_C = (HBITMAP)SelectObject(Boss_C_DC, Pic_Boss_C);
		ReleaseDC(hWnd, hDC);
		break;
	}
	case WM_PAINT: {
		hDC = BeginPaint(hWnd, &ps);
		old_Brush = (HBRUSH)SelectObject(hDC, red_Brush); // �⺻ �귯�� ����
		FillRect(mainDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH)); // ���� ȭ�� ������(�Ͼ� �簢������ �������) �ֽ�ȭ
		if (stage % 2 == 0) { // ¦���� ��������, Ȧ���� �޽� ������ ��µ�
			SelectObject(MapDC, Pic_BossMap);
			StretchBlt(mainDC, 0, 0, rt.right, rt.bottom, MapDC, 0, 0, 1200, 1000, SRCCOPY); // ���� �� ä���� ����ϱ⿡ rt ����ü Ȱ��
		}
		else {
			SelectObject(MapDC, Pic_RelaxMap);
			StretchBlt(mainDC, 0, 700, rt.right, 300, MapDC, 0, 0, 1200, 300, SRCCOPY); // �� �ϴܺο� ���� �� ���� �ִ�ġ�� ���� 300 ������ ���
		}
		// ���
		StretchBlt(mainDC, 500, 500, 30, 30, PlayerDC, 0, 0, 30, 30, SRCCOPY);
		// �������� 4�� ���� ó��
		if (stage == 4) {
			B.print_stage4();
			// Ÿ�̸� ���� �ߺ� ���� (stage4 Ŭ���� �� ���� ���������� �Ѿ �� stage4_onoff �ٽ� 0���� �ʱ�ȭ���ֱ�)
			if (stage4_onoff == 0) {
				SetTimer(hWnd, B_make_attack, 1000, NULL);
				SetTimer(hWnd, B_remove_attack, 3000, NULL);
				SetTimer(hWnd, B_shoot_attack, 10, NULL);
				stage4_onoff = 1;
			}
		}
		else if (stage == 6) {
			//C.print_stage6();
		}
		BitBlt(hDC, 0, 0, rt.right, rt.bottom, mainDC, 0, 0, SRCCOPY);

		SelectObject(hDC, old_Brush); // �귯�� �ʱ�ȭ
		// �� DC ���� �ʱ�ȭ
		SelectObject(hDC, old_hBitmap);
		SelectObject(hDC, old_Pic_BossMap);
		SelectObject(hDC, old_Pic_Boss_B);
		SelectObject(hDC, old_Pic_Boss_C);
		SelectObject(hDC, old_Pic_Platform);
		SelectObject(hDC, old_Pic_Player);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER: {
		// ���� ���� Ÿ�̸�
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
		// ���� ���� Ÿ�̸�
		else if (wParam == B_remove_attack) {
			B.remove_attack();
		}
		// ���� �̵� Ÿ�̸�
		else if (wParam == B_shoot_attack) {
			B.shoot_attack();
		}
		// �ǰ� �� Ÿ�̸� (0.3�� �Ŀ� �ش� Ÿ�̸Ӱ� ����Ǹ鼭 ������ ��� ����� ������� �ǵ����� Ÿ�̸� ����)
		// ��, ������ �ǰ� ���ڸ��� ���� �����Ǹ�, B_attacked Ÿ�̸Ӹ� �����Ѵ�. ���� 0.3�� �Ŀ� B_attacked Ÿ�̸Ӹ� ���� ���� ������� ���ƿ��� ��)
		// �ǰ� üũ�� SetTImer(hWnd, B_attacked, 300, NULL); �ۼ����ֱ�
		// �ǰ� �� �� ������ �ϴ� �����ϱ�
		else if (wParam == B_attacked) {
			B.print_type_change();
			KillTimer(hWnd, B_attacked);
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_DESTROY: {
		DeleteObject(red_Brush);
		DeleteObject(lightgray_Brush);
		// ������ DC �� ��Ʈ�� ����
		DeleteDC(mainDC);
		DeleteDC(MapDC);
		DeleteDC(PlayerDC);
		DeleteDC(PlatformDC);
		DeleteDC(Boss_B_DC);
		DeleteObject(hBitmap);
		DeleteObject(Pic_BossMap);
		DeleteObject(Pic_RelaxMap);
		DeleteObject(Pic_Player);
		DeleteObject(Pic_Platform);
		DeleteObject(Pic_Platform);
		DeleteObject(Pic_Boss_B_row);
		DeleteObject(Pic_Boss_B_col);
		DeleteObject(Pic_Boss_C);
		PostQuitMessage(0);
		return 0;
	}
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}