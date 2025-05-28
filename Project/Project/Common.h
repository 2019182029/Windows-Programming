#pragma once

// 기타 여러 선언 및 정의를 수행하는 헤더

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
extern HINSTANCE g_hinst;  // 일단 보류 (현재 실행 중인 애플리케이션의 인스턴스 핸들을 담기 위한 g_hinst 변수)

extern std::default_random_engine dre;  // 랜덤 엔진

extern HDC mainDC;  // 모든 비트맵을 mainDC에 복사한 뒤, 이를 한꺼번에 hDC로 옮겨서 더블 버퍼링 구현
extern HDC MapDC;  // 맵 관련 DC로, 보스맵과 휴식 공간을 다룸
extern HDC PlayerDC;  // 플레이어 캐릭터 관련 DC로, 기본 상태와 구르기 상태를 다룸
extern HDC PlatformDC;  // 발판 DC
extern HDC WeaponDC;  // 무기 DC
extern HDC BulletDC;  // 총알 DC
extern HDC PlatformDC; // 발판 DC
extern HDC PortalDC; // 포탈
extern HDC HealDC; // 회복
extern HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player[6], Pic_Platform, Pic_Weapon[4], Pic_Bullet, Pic_Portal, Pic_Heal;
extern BITMAP Bmp_Player[6], Bmp_Platform, Bmp_Weapon[4], Bmp_Bullet, Bmp_Portal, Bmp_Heal;

extern HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform, old_Pic_Weapon, old_Pic_Bullet, old_Pic_Portal, old_Pic_Heal;  // 이전 비트맵 정보 저장 (안전한 제거 용도)

extern HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

extern RECT rt; // 윈도우 크기 저장용 RECT 구조체

// 스테이지 구성 양상: 휴식 공간 -> 보스 A -> 휴식 공간 -> 보스 B -> 휴식 공간 -> 보스 C -> 보스 C 클리어하면 게임 승리 메시지 창과 함께 게임 종료
// 보스를 클리어하면 stage가 1 증가한다. 즉, stage가 7이 되면 우승 메시지를 띄우면 됨
extern int stage; // 스테이지 구분 (홀수면 휴식 공간, 짝수면 보스맵) (1부터 시작하며, 다음 스테이지로 넘어갈 때마다 1씩 증가시킴) (구현하는 스테이지에 따라 수치 다르게 주고 작업)
