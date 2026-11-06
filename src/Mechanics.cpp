#include "Mechanics.hpp"

void CombatManager::SetMode(int Mode){CurrentMode = Mode;}

bool IsActionLocked = false;
void CombatManager::RequestMove(MoveSetBase& CurrentMoveBase, AssetManager& manager, Vector2 position, bool flipX)
{
    switch(CurrentMode)
    {
        case 0:
        {
            if(IsActionLocked){CurrentMoveBase = MoveSetBaseLocked;}
            MoveSetBaseLogic(CurrentMoveBase, manager, position, flipX);
            
            break;
        }
        case 1:
        {

            break;
        }
        case 2:
        {

            break;
        }
        default: break;
    }
}

MoveSetBase MoveSetBaseLocked = Standing;
void MoveSetBaseLogic(MoveSetBase& CurrentMoveBase, AssetManager& manager, Vector2 position, bool flipX)
{
    std::string AnimKey = "";

    switch(CurrentMoveBase)
    {
        case Walk:
        {
            AnimKey = "C_Walk";
            break;
        }
        case WalkUP:
        {
            AnimKey = "C_WalkUP";
            break;
        }
        case WalkDown:
        {
            AnimKey = "C_WalkDown";
            break;
        }
        case Backward:
        {
            AnimKey = "C_Backward";
            IsActionLocked = true;
            MoveSetBaseLocked = CurrentMoveBase;
            break;
        }
        case Punch:
        {
            AnimKey = "C_Punch";
            IsActionLocked = true;
            MoveSetBaseLocked = CurrentMoveBase;
            break;
        }
        case Standing:
        {
            AnimKey = "C_Standing";
            break;
        }
    }

    Animation* Anim = manager.Search(AnimKey);

    if(Anim != nullptr)
    {
        UpdateAndDrawAnimation(*Anim, position, flipX, 0.0f);

        if(Anim->currentFrame >= (Anim->totalFrames) - 1) 
        {
            IsActionLocked = false; 
            Anim->currentFrame = 0;
            Anim->tickCounter = 0;
        }      
    }

}