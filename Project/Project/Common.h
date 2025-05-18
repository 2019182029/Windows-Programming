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


HDC mainDC; // 모든 비트맵을 mainDC에 복사한 뒤, 이를 한꺼번에 hDC로 옮겨서 더블 버퍼링 구현
HDC MapDC; // 맵 관련 DC로, 보스맵과 휴식 공간을 다룸
HDC PlayerDC; // 플레이어 캐릭터 관련 DC로, 기본 상태와 구르기 상태를 다룸
HDC PlatformDC; // 발판 DC
HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player, Pic_Platform;
// 이전 비트맵 정보 저장 (안전한 제거 용도)
HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform;

HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

RECT rt; // 윈도우 크기 저장용 RECT 구조체

// 스테이지 구성 양상: 휴식 공간 -> 보스 A -> 휴식 공간 -> 보스 B -> 휴식 공간 -> 보스 C -> 보스 C 클리어하면 게임 승리 메시지 창과 함께 게임 종료
int stage = 6; // 스테이지 구분 (홀수면 휴식 공간, 짝수면 보스맵) (1부터 시작하며, 다음 스테이지로 넘어갈 때마다 1씩 증가시킴) (구현하는 스테이지에 따라 수치 다르게 주고 작업)