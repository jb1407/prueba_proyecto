#ifndef ENEMYAI_HPP
#define ENEMYAI_HPP

#include <raylib.h>
#include "CombatSystem.hpp"

class EnemyAI {
private:
    Vector2 position;
    Vector2 speed;
    Vector2 dashSpeed;
    Vector2 activeBackVel = { 0.0f, 0.0f };
    Vector2 size;

    MoveSetData moveSetData;
    MoveSetBase moveSet;
    CombatManager cpuCombat{nullptr, nullptr, CombatMode::UnArmed};

    int depthSwapStart = 0;
    int depthSwapEnd = 0;
    float rotation = 0.0f;
    bool flipX = false;

    bool isAttacking = false;
    bool isBacking = false;

    void ClampToScreen(int screenWidth, int screenHeight);
    void RunMove(Move* targetMove, float dx, float dy);

public:
    EnemyAI(float startX, float startY, AssetManager& assets);
    ~EnemyAI() = default;

    void MoveUp(int screenWidth, int screenHeight);
    void MoveDown(int screenWidth, int screenHeight);
    void MoveLeft(int screenWidth, int screenHeight);
    void MoveRight(int screenWidth, int screenHeight);
    void AttackPunch(int screenWidth, int screenHeight);
    void DashBackward(float dirX, float dirY, int screenWidth, int screenHeight);
    void StandIdle(int screenWidth, int screenHeight);

    void UpdateFrame(int screenWidth, int screenHeight);

    Vector2 GetPosition() const { return position; }
    bool IsBusy() const { return isAttacking || isBacking; }
};

#endif