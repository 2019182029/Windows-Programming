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

extern HINSTANCE g_hinst;  // �ϴ� ���� (���� ���� ���� ���ø����̼��� �ν��Ͻ� �ڵ��� ��� ���� g_hinst ����)

extern std::default_random_engine dre;  // ���� ����



extern HDC mainDC;  // ��� ��Ʈ���� mainDC�� ������ ��, �̸� �Ѳ����� hDC�� �Űܼ� ���� ���۸� ����
extern HDC MapDC;  // �� ���� DC��, �����ʰ� �޽� ������ �ٷ�
extern HDC PlayerDC;  // �÷��̾� ĳ���� ���� DC��, �⺻ ���¿� ������ ���¸� �ٷ�
extern HDC PlatformDC;  // ���� DC
extern HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player, Pic_Platform;
extern BITMAP Bmp_Player, Bmp_Platform;

extern HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform;  // ���� ��Ʈ�� ���� ���� (������ ���� �뵵)

extern HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

extern RECT rt; // ������ ũ�� ����� RECT ����ü

// �������� ���� ���: �޽� ���� �� ���� A �� �޽� ���� �� ���� B �� �޽� ���� �� ���� C �� ���� C Ŭ�����ϸ� ���� �¸� �޽��� â�� �Բ� ���� ����
extern int stage; // �������� ���� (Ȧ���� �޽� ����, ¦���� ������) (1���� �����ϸ�, ���� ���������� �Ѿ ������ 1�� ������Ŵ) (�����ϴ� ���������� ���� ��ġ �ٸ��� �ְ� �۾�)