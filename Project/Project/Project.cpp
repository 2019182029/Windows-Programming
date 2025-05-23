#include "Common.h"
#include "Boss A.h"
#include "Boss B.h"
#include "Boss C.h"
#include "Element.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

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
		Pic_Player = (HBITMAP)LoadImage(g_hinst, _T("player.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Player = (HBITMAP)SelectObject(PlayerDC, Pic_Player);
		SelectObject(PlayerDC, Pic_Player);
		// PlatformDC에 대한 비트맵 생성 및 설정
		PlatformDC = CreateCompatibleDC(hDC);
		Pic_Platform = (HBITMAP)LoadImage(g_hinst, _T("platform.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Platform = (HBITMAP)SelectObject(PlatformDC, Pic_Platform);
		// PortalDC에 대한 비트맵 생성 및 설정
		PortalDC = CreateCompatibleDC(hDC);
		Pic_Portal = (HBITMAP)LoadImage(g_hinst, _T("portal.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Portal = (HBITMAP)SelectObject(PortalDC, Pic_Portal);
		// HealDC에 대한 비트맵 생성 및 설정
		HealDC = CreateCompatibleDC(hDC);
		Pic_Heal = (HBITMAP)LoadImage(g_hinst, _T("healing_item.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Heal = (HBITMAP)SelectObject(HealDC, Pic_Heal);
		// Boss_A_DC에 대한 비트맵 생성 및 설정
		Boss_A_DC = CreateCompatibleDC(hDC);
		Pic_Boss_A[0] = (HBITMAP)LoadImage(g_hinst, _T("boss A1.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[1] = (HBITMAP)LoadImage(g_hinst, _T("boss A2.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[2] = (HBITMAP)LoadImage(g_hinst, _T("boss A3.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[3] = (HBITMAP)LoadImage(g_hinst, _T("boss A4.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[4] = (HBITMAP)LoadImage(g_hinst, _T("boss A5.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[5] = (HBITMAP)LoadImage(g_hinst, _T("boss A6.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		Pic_Boss_A[6] = (HBITMAP)LoadImage(g_hinst, _T("boss A7.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		old_Pic_Boss_A = (HBITMAP)SelectObject(Boss_A_DC, Pic_Boss_A);
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
			// 보스맵
			SelectObject(MapDC, Pic_BossMap);
			StretchBlt(mainDC, 0, 0, rt.right, rt.bottom, MapDC, 0, 0, 1200, 1000, SRCCOPY); // 맵을 꽉 채워서 출력하기에 rt 구조체 활용
		}
		else {
			// 휴식 공간
			SelectObject(MapDC, Pic_RelaxMap);
			StretchBlt(mainDC, 0, 700, rt.right, 300, MapDC, 0, 0, 1200, 300, SRCCOPY); // 맵 하단부에 폭은 맵 가로 최대치에 높이 300 범위에 출력
			TransparentBlt(mainDC, portal.px(), portal.py(), 100, 100, PortalDC, 0, 0, 100, 100, RGB(255, 255, 255)); // 포탈
			heal.emplace_back(500, 600);
			TransparentBlt(mainDC, heal.begin()->h_x(), heal.begin()->h_y(), 50, 50, HealDC, 0, 0, 50, 50, RGB(255, 255, 255)); // 회복 물약
		}
		// 출력
		StretchBlt(mainDC, 500, 500, 30, 30, PlayerDC, 0, 0, 30, 30, SRCCOPY);
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
		// 마지막 보스를 클리어 했을 시 승리했음을 나타내는 메시지 박스 띄움
		if (stage == 7) {
			MessageBox(hWnd, _T("승리!"), _T("정보"), MB_OK);
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
	case WM_DESTROY: {
		// 각 DC 정보 초기화
		SelectObject(mainDC, old_hBitmap);
		SelectObject(MapDC, old_Pic_BossMap);
		SelectObject(Boss_A_DC, old_Pic_Boss_A);
		SelectObject(Boss_B_DC, old_Pic_Boss_B);
		SelectObject(Boss_C_DC, old_Pic_Boss_C);
		SelectObject(PlatformDC, old_Pic_Platform);
		SelectObject(PlayerDC, old_Pic_Player);
		SelectObject(PortalDC, old_Pic_Portal);
		SelectObject(HealDC, old_Pic_Heal);
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