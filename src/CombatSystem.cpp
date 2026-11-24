#include "CombatSystem.hpp"

MoveSetData::MoveSetData(){};
MoveSetBase MoveSetData::CharacterMoveSetDefault(AssetManager& AssetManager, Vector2* V1)
{
    Move* C_Standing = new Move("Standing", AssetManager.Search("C_Standing"), nullptr, nullptr, V1, nullptr, nullptr);
    Move* C_Walk = new Move("Walk", AssetManager.Search("C_Walk"), nullptr, nullptr, V1, nullptr, nullptr);
    Move* C_WalkUP = new Move("WalkUP", AssetManager.Search("C_WalkUP"), nullptr, nullptr, V1, nullptr, nullptr);
    Move* C_WalkDown = new Move("WalkDown", AssetManager.Search("C_WalkDown"), nullptr, nullptr, V1, nullptr, nullptr);
    Move* C_Punch = new Move("Punch", AssetManager.Search("C_Punch"), nullptr, nullptr, V1, nullptr, nullptr);
    Move* C_Backward = new Move("BackWard", AssetManager.Search("C_Backward"), nullptr, nullptr, V1, nullptr, nullptr);

    AllMoves.push_back(C_Standing);
    AllMoves.push_back(C_Walk);
    AllMoves.push_back(C_WalkUP);
    AllMoves.push_back(C_WalkDown);
    AllMoves.push_back(C_Punch);
    AllMoves.push_back(C_Backward);

    return MoveSetBase{C_Standing, C_Walk, C_WalkUP, C_WalkDown, C_Punch, C_Backward};
}

MoveSetWeapons MoveSetData::NeonSwordRedMoves(AssetManager& AssetManager){return MoveSetWeapons{};}

MoveSetData::~MoveSetData()
{
    for (Move* movePtr : AllMoves)
    {
        delete movePtr;
    }
    AllMoves.clear();
}

Combat::Combat(Player* p, int depthSwapStart, int depthSwapEnd, bool flipX, float rotation, bool moveEndOrReset, int frameMoveEnd)
    : P(p),
      DepthSwapStart(depthSwapStart),
      DepthSwapEnd(depthSwapEnd),
      FlipX(flipX),
      Rotation(rotation),
      MoveEndOrReset(moveEndOrReset),
      FrameMoveEnd(frameMoveEnd) {}

void Combat::SetDepthSwapStart(int start) { DepthSwapStart = start; }
void Combat::SetDepthSwapEnd(int end) { DepthSwapEnd = end; }
void Combat::SetMoveEnd(bool finished) { MoveEndOrReset = finished; }
void Combat::SetFrameMoveEnd(int frameEnd) { FrameMoveEnd = frameEnd; }
void Combat::SetFlipX(bool flip) { FlipX = flip; }
void Combat::SetRotation(float rot) { Rotation = rot; }

int Combat::GetDepthSwapStart() const { return DepthSwapStart; }
int Combat::GetDepthSwapEnd() const { return DepthSwapEnd; }
bool Combat::GetMoveEnd() const { return MoveEndOrReset; }
int Combat::GetFrameMoveEnd() const { return FrameMoveEnd; }
bool Combat::GetFlipX() const { return FlipX; }
float Combat::GetRotation() const { return Rotation;}

void Combat::UpdateMoveBasePlayer1()
{
   /* if(!P->GetIsAttack() && !IsBackward)
    {
        if(IsKeyDown(KEY_W))
        {
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().WalkUP){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);} //Mejor se deja MoveEndOrReset para el manejo de combos
            PlayerCombat.SetCurrentMove(P->GetMoveBase().WalkUP);
            if(!PlayerCombat.ValidateMove()) return;
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().WalkUP->GetAnimation1()->totalFrames - 1)
            {
                PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
            }
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false); //El booleano MoveEnd tiene mas coherencia usarlo en combos de armas
            PlayerPosition.y -= 5.0f;
        }
        else if(IsKeyDown(KEY_S))
        {
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().WalkDown){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.SetCurrentMove(P->GetMoveBase().WalkDown);
            if(!PlayerCombat.ValidateMove()) return;
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().WalkDown->GetAnimation1()->totalFrames - 1){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false); //El booleano MoveEnd tiene mas coherencia usarlo en combos de armas
            PlayerPosition.y += 5.0f;
        }
        else if(IsKeyDown(KEY_A))
        {
            FlipX = true;
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().Walk){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.SetCurrentMove(P->GetMoveBase().Walk);
            if(!PlayerCombat.ValidateMove()) return;
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().Walk->GetAnimation1()->totalFrames - 1){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false); //El booleano MoveEnd tiene mas coherencia usarlo en combos de armas
            PlayerPosition.x -= 5.0f;
        }
        else if(IsKeyDown(KEY_D))
        {
            FlipX = false;
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().Walk){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.SetCurrentMove(P->GetMoveBase().Walk);
            if(!PlayerCombat.ValidateMove()) return;
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().Walk->GetAnimation1()->totalFrames - 1){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false); 
            PlayerPosition.x += 5.0f;

        }
        else if(IsKeyPressed(KEY_I))
        {
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().Punch){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.SetCurrentMove(P->GetMoveBase().Punch);
            if(!PlayerCombat.ValidateMove()) {return;}

            P->SetIsAttack(true);
        }
        else if(IsKeyPressed(KEY_O))
        {
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().BackWard){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.SetCurrentMove(P->GetMoveBase().BackWard);
            if(!PlayerCombat.ValidateMove()) {return;}

            IsBackward = true;
        }
        else
        {
            if(PlayerCombat.GetCurrentMove() != P->GetMoveBase().Standing){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.SetCurrentMove(P->GetMoveBase().Standing);
            if(!PlayerCombat.ValidateMove()) return;
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().Standing->GetAnimation1()->totalFrames - 1){PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);}
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false); //El booleano MoveEnd tiene mas coherencia usarlo en combos de armas
        }
    }
    else
    {
        if(PlayerCombat.GetCurrentMove() == P->GetMoveBase().Punch)
        {
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().Punch->GetAnimation1()->totalFrames - 1)
            {
                PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
                IsBackward = false;
                P->SetIsAttack(false);
            }

            if(IsKeyPressed(KEY_A)){FlipX = true;}
            if(IsKeyPressed(KEY_D)){FlipX = false;}

            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
        }
        else if(PlayerCombat.GetCurrentMove() == P->GetMoveBase().BackWard)
        {
            if(PlayerCombat.GetAnimState().CurrentFrame1 == P->GetMoveBase().BackWard->GetAnimation1()->totalFrames - 1)
            {
                PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
                IsBackward = false;
                P->SetIsAttack(false);
            }
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);

            if(IsKeyDown(KEY_A) && IsKeyPressed(KEY_O)){PlayerPosition.x -= DashVelocity.x;}
            if(IsKeyDown(KEY_D) && IsKeyPressed(KEY_O)){PlayerPosition.x += DashVelocity.x;}
            if(IsKeyDown(KEY_W) && IsKeyPressed(KEY_O)){PlayerPosition.y -= DashVelocity.y;}
            if(IsKeyDown(KEY_S) && IsKeyPressed(KEY_O)){PlayerPosition.y += DashVelocity.y;}
            if((IsKeyDown(KEY_A) && IsKeyDown(KEY_S)) && IsKeyPressed(KEY_O)){PlayerPosition.x -= DashVelocity.x; PlayerPosition.y += DashVelocity.y;}
            if((IsKeyDown(KEY_A) && IsKeyDown(KEY_W)) && IsKeyPressed(KEY_O)){PlayerPosition.x -= DashVelocity.x; PlayerPosition.y -= DashVelocity.y;}
            if((IsKeyDown(KEY_D) && IsKeyDown(KEY_S)) && IsKeyPressed(KEY_O)){PlayerPosition.x += DashVelocity.x; PlayerPosition.y += DashVelocity.y;}
            if((IsKeyDown(KEY_D) && IsKeyDown(KEY_W)) && IsKeyPressed(KEY_O)){PlayerPosition.x += DashVelocity.x; PlayerPosition.y -= DashVelocity.y;}
        }

       
    } */

    MoveSetBase base = P->GetMoveBase();
    Move* current = PlayerCombat.GetCurrentMove();

    const bool moveLeft  = IsKeyDown(KEY_A);
    const bool moveRight = IsKeyDown(KEY_D);
    const bool moveUp    = IsKeyDown(KEY_W);
    const bool moveDown  = IsKeyDown(KEY_S);

    // 1. Estados activos de acción / retroceso (Punch o BackWard)
    if (P->GetIsAttack() || IsBackward)
    {
        if (current == base.Punch)
        {
            if (PlayerCombat.GetAnimState().CurrentFrame1 == base.Punch->GetAnimation1()->totalFrames - 1)
            {
                PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
                IsBackward = false;
                P->SetIsAttack(false);
            }

            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
            return;
        }

        if (current == base.BackWard)
        {
            if (moveLeft)       FlipX = true;
            else if (moveRight) FlipX = false;

            if (moveLeft)  PlayerPosition.x -= DashVelocity.x;
            if (moveRight) PlayerPosition.x += DashVelocity.x;
            if (moveUp)    PlayerPosition.y -= DashVelocity.y;
            if (moveDown)  PlayerPosition.y += DashVelocity.y;

            if (PlayerCombat.GetAnimState().CurrentFrame1 == base.BackWard->GetAnimation1()->totalFrames - 1)
            {
                PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
                IsBackward = false;
                P->SetIsAttack(false);
            }

            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
            return;
        }
    }

    // 2. Disparadores de nuevas acciones (Punch con I / BackWard con O)
    if (IsKeyPressed(KEY_I))
    {
        if (current != base.Punch)
        {
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
        }

        PlayerCombat.SetCurrentMove(base.Punch);
        if (!PlayerCombat.ValidateMove()) return;

        if (moveLeft)       FlipX = true;
        else if (moveRight) FlipX = false;

        P->SetIsAttack(true);
        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
        return;
    }

    if (IsKeyPressed(KEY_O))
    {
        if (current != base.BackWard)
        {
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
        }

        PlayerCombat.SetCurrentMove(base.BackWard);
        if (!PlayerCombat.ValidateMove()) return;

        if (moveLeft)       FlipX = true;
        else if (moveRight) FlipX = false;

        IsBackward = true;
        P->SetIsAttack(true);
        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
        return;
    }

    // 3. Movimiento base o estado neutro (Standing)
    Move* nextMove = base.Standing;
    float dx = 0.0f;
    float dy = 0.0f;

    if (moveUp)
    {
        nextMove = base.WalkUP;
        dy -= 5.0f;
    }
    else if (moveDown)
    {
        nextMove = base.WalkDown;
        dy += 5.0f;
    }
    else if (moveLeft)
    {
        FlipX = true;
        nextMove = base.Walk;
        dx -= 5.0f;
    }
    else if (moveRight)
    {
        FlipX = false;
        nextMove = base.Walk;
        dx += 5.0f;
    }

    if (current != nextMove)
    {
        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
    }

    PlayerCombat.SetCurrentMove(nextMove);
    if (!PlayerCombat.ValidateMove()) return;

    if (PlayerCombat.GetAnimState().CurrentFrame1 == nextMove->GetAnimation1()->totalFrames - 1)
    {
        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
    }

    PlayerPosition.x += dx;
    PlayerPosition.y += dy;
    PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
}