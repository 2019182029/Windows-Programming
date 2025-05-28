#pragma once

// ��Ÿ ���� ���� �� ���Ǹ� �����ϴ� ���

#define _USE_MATH_DEFINES
#include <Windows.h>
#include <tchar.h>
#include <random>
#include <iostream>
#include <math.h>
#include <cmath>
#include <vector>
#include <thread>
#include <chrono>

#pragma comment(lib, "msimg32.lib")

constexpr float MAX_GRAVITY = 9.8f;

//////////////////////////////////////////////////
// PLAYER
#define PLAYER_MOVE 0

constexpr int IDLE = 0;
constexpr int UP = 1;
constexpr int DOWN = 2;
constexpr int LEFT = 3;
constexpr int RIGHT = 4;
constexpr int DODGE = 5;

//////////////////////////////////////////////////
// ITEM
#define ITEM_SPAWN 10
#define ITEM_MOVE 11

enum ITEM_TYPE { WEAPON, HEAL };

constexpr int PISTOL = 0;
constexpr int SMG = 1;
constexpr int SHOTGUN = 2;
constexpr int SNIPER = 3;

//////////////////////////////////////////////////
// WINDOWS
extern HINSTANCE g_hinst;  // �ϴ� ���� (���� ���� ���� ���ø����̼��� �ν��Ͻ� �ڵ��� ��� ���� g_hinst ����)

extern std::default_random_engine dre;  // ���� ����

extern HDC mainDC;  // ��� ��Ʈ���� mainDC�� ������ ��, �̸� �Ѳ����� hDC�� �Űܼ� ���� ���۸� ����
extern HDC MapDC;  // �� ���� DC��, �����ʰ� �޽� ������ �ٷ�
extern HDC PlayerDC;  // �÷��̾� ĳ���� ���� DC��, �⺻ ���¿� ������ ���¸� �ٷ�
extern HDC PlatformDC;  // ���� DC
extern HDC WeaponDC;  // ���� DC
extern HDC BulletDC;  // �Ѿ� DC
extern HDC PlatformDC; // ���� DC
extern HDC PortalDC; // ��Ż
extern HDC HealDC; // ȸ��
extern HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player[6], Pic_Platform, Pic_Weapon[4], Pic_Bullet, Pic_Portal, Pic_Heal;
extern BITMAP Bmp_Player[6], Bmp_Platform, Bmp_Weapon[4], Bmp_Bullet, Bmp_Portal, Bmp_Heal;

extern HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform, old_Pic_Weapon, old_Pic_Bullet, old_Pic_Portal, old_Pic_Heal;  // ���� ��Ʈ�� ���� ���� (������ ���� �뵵)

extern HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

extern RECT rt; // ������ ũ�� ����� RECT ����ü

// �������� ���� ���: �޽� ���� -> ���� A -> �޽� ���� -> ���� B -> �޽� ���� -> ���� C -> ���� C Ŭ�����ϸ� ���� �¸� �޽��� â�� �Բ� ���� ����
// ������ Ŭ�����ϸ� stage�� 1 �����Ѵ�. ��, stage�� 7�� �Ǹ� ��� �޽����� ���� ��
extern int stage; // �������� ���� (Ȧ���� �޽� ����, ¦���� ������) (1���� �����ϸ�, ���� ���������� �Ѿ ������ 1�� ������Ŵ) (�����ϴ� ���������� ���� ��ġ �ٸ��� �ְ� �۾�)
