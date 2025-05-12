#pragma once

// 기타 여러 선언 및 정의를 수행하는 헤더

#define _USE_MATH_DEFINES
#include<Windows.h>
#include<tchar.h>
#include<random>
#include<iostream>
#include<math.h>
#include<cmath>
#include<vector>
#pragma comment(lib, "msimg32.lib")

HINSTANCE g_hinst; // 일단 보류 (현재 실행 중인 애플리케이션의 인스턴스 핸들을 담기 위한 g_hinst 변수
std::default_random_engine dre; // 랜덤 엔진

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);
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

HDC mainDC; // 모든 비트맵을 mainDC에 복사한 뒤, 이를 한꺼번에 hDC로 옮겨서 더블 버퍼링 구현
HDC MapDC; // 맵 관련 DC로, 보스맵과 휴식 공간을 다룸
HDC PlayerDC; // 플레이어 캐릭터 관련 DC로, 기본 상태와 구르기 상태를 다룸
HDC PlatformDC; // 발판 DC
HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player, Pic_Platform;
// 이전 비트맵 정보 저장 (안전한 제거 용도)
HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform;

HBRUSH red_Brush, lightgray_Brush, old_Brush;

RECT rt; // 윈도우 크기 저장용 RECT 구조체

// 스테이지 구성 양상: 휴식 공간 -> 보스 A -> 휴식 공간 -> 보스 B -> 휴식 공간 -> 보스 C -> 보스 C 클리어하면 게임 승리 메시지 창과 함께 게임 종료
int stage = 6; // 스테이지 구분 (홀수면 휴식 공간, 짝수면 보스맵) (1부터 시작하며, 다음 스테이지로 넘어갈 때마다 1씩 증가시킴) (구현하는 스테이지에 따라 수치 다르게 주고 작업)