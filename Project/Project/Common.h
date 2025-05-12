#pragma once

// ��Ÿ ���� ���� �� ���Ǹ� �����ϴ� ���

#define _USE_MATH_DEFINES
#include<Windows.h>
#include<tchar.h>
#include<random>
#include<iostream>
#include<math.h>
#include<cmath>
#include<vector>
#pragma comment(lib, "msimg32.lib")

HINSTANCE g_hinst; // �ϴ� ���� (���� ���� ���� ���ø����̼��� �ν��Ͻ� �ڵ��� ��� ���� g_hinst ����
std::default_random_engine dre; // ���� ����

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam);
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

HDC mainDC; // ��� ��Ʈ���� mainDC�� ������ ��, �̸� �Ѳ����� hDC�� �Űܼ� ���� ���۸� ����
HDC MapDC; // �� ���� DC��, �����ʰ� �޽� ������ �ٷ�
HDC PlayerDC; // �÷��̾� ĳ���� ���� DC��, �⺻ ���¿� ������ ���¸� �ٷ�
HDC PlatformDC; // ���� DC
HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player, Pic_Platform;
// ���� ��Ʈ�� ���� ���� (������ ���� �뵵)
HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform;

HBRUSH red_Brush, lightgray_Brush, old_Brush;

RECT rt; // ������ ũ�� ����� RECT ����ü

// �������� ���� ���: �޽� ���� -> ���� A -> �޽� ���� -> ���� B -> �޽� ���� -> ���� C -> ���� C Ŭ�����ϸ� ���� �¸� �޽��� â�� �Բ� ���� ����
int stage = 6; // �������� ���� (Ȧ���� �޽� ����, ¦���� ������) (1���� �����ϸ�, ���� ���������� �Ѿ ������ 1�� ������Ŵ) (�����ϴ� ���������� ���� ��ġ �ٸ��� �ְ� �۾�)