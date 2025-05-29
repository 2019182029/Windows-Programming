#include "Common.h"

HINSTANCE g_hinst; 

std::default_random_engine dre; 

HDC mainDC; 
HDC MapDC;
HDC PlayerDC; 
HDC PlatformDC;
HDC WeaponDC;
HDC BulletDC;
HDC PortalDC;
HDC HealDC; 
HDC DamageDC;

HBITMAP hBitmap, Pic_BossMap, Pic_RelaxMap, Pic_TutorialMap, Pic_Player[6], Pic_Platform, Pic_Weapon[4], Pic_Bullet, Pic_Portal, Pic_Heal, Pic_Damage;
BITMAP Bmp_Player[6], Bmp_Platform, Bmp_Weapon[4], Bmp_Bullet, Bmp_Portal, Bmp_Heal, Bmp_Damage;

HBITMAP old_hBitmap, old_Pic_BossMap, old_Pic_Player, old_Pic_Platform, old_Pic_Weapon, old_Pic_Bullet, old_Pic_Portal, old_Pic_Heal, old_Pic_Damage;

HBRUSH red_Brush, blue_Brush, lightgray_Brush, old_Brush;

RECT rt;

int stage = 1;