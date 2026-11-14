#pragma once
#include <raylib.h>

class HUD {
public:
    static void Draw(int screenWidth, int screenHeight, float playerHealth, float playerMaxHealth, float enemyHealth, float enemyMaxHealth, Font menuFont);
};