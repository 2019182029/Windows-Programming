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


HDC mainDC; // ��� ��Ʈ���� mainDC�� ������ ��, �̸� �Ѳ����� hDC�� �Űܼ� ���� ���۸� ����
HDC MapDC; // �� ���� DC��, �����ʰ� �޽� ������ �ٷ�
HDC PlayerDC; // �÷��̾� ĳ���� ���� DC��, �⺻ ���¿� ������ ���¸� �ٷ�
HDC PlatformDC; // ���� DC
HDC PortalDC; // ��Ż
HDC HealDC; // ȸ�� ������
HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player, Pic_Platform, Pic_Portal, Pic_Heal;
// ���� ��Ʈ�� ���� ���� (������ ���� �뵵)
HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform, old_Pic_Portal, old_Pic_Heal;

HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

RECT rt; // ������ ũ�� ����� RECT ����ü

// �������� ���� ���: �޽� ���� -> ���� A -> �޽� ���� -> ���� B -> �޽� ���� -> ���� C -> ���� C Ŭ�����ϸ� ���� �¸� �޽��� â�� �Բ� ���� ����
// ������ Ŭ�����ϸ� stage�� 1 �����Ѵ�. ��, stage�� 7�� �Ǹ� ��� �޽����� ���� ��
int stage = 1; // �������� ���� (Ȧ���� �޽� ����, ¦���� ������) (1���� �����ϸ�, ���� ���������� �Ѿ ������ 1�� ������Ŵ) (�����ϴ� ���������� ���� ��ġ �ٸ��� �ְ� �۾�)