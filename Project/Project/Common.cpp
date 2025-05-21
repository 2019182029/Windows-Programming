#include "Common.h"

HINSTANCE g_hinst; 

std::default_random_engine dre; 



HDC mainDC; 
HDC MapDC;
HDC PlayerDC; 
HDC PlatformDC;
HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_Player[6], Pic_Platform;
BITMAP Bmp_Player[6], Bmp_Platform;

HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform;

HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

RECT rt;

int stage = 4;