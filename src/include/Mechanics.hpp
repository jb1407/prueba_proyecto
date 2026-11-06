#ifndef MECHANICS_HPP
#define MECHANICS_HPP

#include "LoadAssets.hpp"

extern bool IsActionLocked;

typedef enum MoveSetBase
{
    Walk = 0,
    WalkUP,
    WalkDown,
    Backward,
    Punch,
    Standing
}MoveSetBase;

extern MoveSetBase MoveSetBaseLocked;

void MoveSetBaseLogic(MoveSetBase&, AssetManager&, Vector2, bool);

class CombatManager
{
    private:

    int CurrentMode = 0; // 0 sin armas, 1 con una sola arma, 2 con 2 armas

    public:

    void SetMode(int);

    void RequestMove(MoveSetBase&, AssetManager&, Vector2, bool);
};



#endif