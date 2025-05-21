#include "Common.h"

HINSTANCE g_hinst; 

std::default_random_engine dre; 



HDC mainDC; 
HDC MapDC;
HDC PlayerDC; 
HDC PlatformDC;
HDC WeaponDC;
HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player[6], Pic_Platform, Pic_Weapon[4];
BITMAP Bmp_Player[6], Bmp_Platform, Bmp_Weapon[4];

HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform, old_Pic_Weapon;

HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

RECT rt;

int stage = 4;