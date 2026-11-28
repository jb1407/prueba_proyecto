#include "CombatSystem.hpp"
#include <cmath>
#include <deque>

MoveSetData::MoveSetData() {}

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

MoveSetWeapons MoveSetData::NeonSwordRedMoves(AssetManager& AssetManager, Vector2* V1, Vector2* V2, Vector2* V3)
{
    Move* Thrust = new Move("Thrust", AssetManager.Search("NSR_Normal1(Character)"), AssetManager.Search("NSR_Normal1(Sword)"), nullptr, V1, V2, nullptr);
    Move* RotaryCut = new Move("RotaryCut", AssetManager.Search("NSR_Normal2(Character)"), AssetManager.Search("NSR_Normal2(Sword)"), nullptr, V1, V2, nullptr);
    Move* CrimsonOrbit = new Move("CrimsonOrbit", AssetManager.Search("NSR_Special1(Character)"), AssetManager.Search("NSR_Special1(Sword)"), AssetManager.Search("NSR_Special1(Circle)"), V1, V2, V3);
    Move* NeonBurst = new Move("NeonBurst", AssetManager.Search("NSR_Special2(Character)"), AssetManager.Search("NSR_Special2(Sword)"), AssetManager.Search("NSR_Special2(Courteous)"), V1, V2, V3);
    Move* PiercedByLight = new Move("PiercedByLight", AssetManager.Search("NSR_Ultimate(Character)"), AssetManager.Search("NSR_Ultimate(Sword)"), nullptr, V1, V2, nullptr);

    Thrust->AddAllowedFollowUp("RotaryCut");
    RotaryCut->AddAllowedFollowUp("CrimsonOrbit");
    RotaryCut->AddAllowedFollowUp("NeonBurst");
    CrimsonOrbit->AddAllowedFollowUp("RotaryCut");
    NeonBurst->AddAllowedFollowUp("RotaryCut");

    AllMoves.push_back(Thrust);
    AllMoves.push_back(RotaryCut);
    AllMoves.push_back(CrimsonOrbit);
    AllMoves.push_back(NeonBurst);
    AllMoves.push_back(PiercedByLight);

    return MoveSetWeapons{Thrust, RotaryCut, CrimsonOrbit, NeonBurst, PiercedByLight};
}

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
float Combat::GetRotation() const { return Rotation; }

void Combat::UpdateMoveBasePlayer1()
{
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
        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
        return;
    }

    // 3. Movimiento base (recto y diagonal) o estado neutro (Standing)
    Move* nextMove = base.Standing;
    float dx = 0.0f;
    float dy = 0.0f;

    if (moveLeft)  dx -= (float)GetScreenWidth() / 150.0f;
    if (moveRight) dx += (float)GetScreenWidth() / 150.0f;
    if (moveUp)    dy -= (float)GetScreenHeight() / 80.0f;
    if (moveDown)  dy += (float)GetScreenHeight() / 80.0f;

    const bool hasHorizontal = (moveLeft != moveRight);
    const bool hasVertical   = (moveUp != moveDown);

    if (hasHorizontal)
    {
        FlipX = moveLeft;
        nextMove = base.Walk;
    }
    else if (hasVertical)
    {
        nextMove = moveUp ? base.WalkUP : base.WalkDown;
    }

    if (dx != 0.0f && dy != 0.0f)
    {
        const float invSqrt2 = 0.70710678f;
        dx *= invSqrt2;
        dy *= invSqrt2;
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

// Neon Sword Red Moves
void Combat::UpdateMoveNSRPlayer1()
{
    struct DirStep
    {
        int dir;        // 1 = Forward, 2 = Backward, 3 = Down, 4 = Up
        float time;
    };

    static std::deque<DirStep> dirBuffer;
    static Move* bufferedNext = nullptr;
    static int specialCount = 0;
    static const float bufferWindow = 0.50f;
    static const int cancelPercent = 60;

    Weapon* activeWeapon = (P->GetActualWeapon() == Weapon1) ? P->GetWeapon1() : P->GetWeapon2();
    if (activeWeapon == nullptr)
    {
        UpdateMoveBasePlayer1();
        return;
    }

    if (IsBackward)
    {
        UpdateMoveBasePlayer1();
        return;
    }

    MoveSetWeapons nsr = activeWeapon->GetMoveSet();
    Move* current = PlayerCombat.GetCurrentMove();
    float now = (float)GetTime();

    const bool moveLeft  = IsKeyDown(KEY_A);
    const bool moveRight = IsKeyDown(KEY_D);

    while (!dirBuffer.empty() && (now - dirBuffer.front().time) > bufferWindow)
    {
        dirBuffer.pop_front();
    }

    KeyboardKey fwdKey  = FlipX ? KEY_A : KEY_D;
    KeyboardKey backKey = FlipX ? KEY_D : KEY_A;

    if (IsKeyPressed(fwdKey))  dirBuffer.push_back({1, now});
    if (IsKeyPressed(backKey)) dirBuffer.push_back({2, now});
    if (IsKeyPressed(KEY_S))   dirBuffer.push_back({3, now});
    if (IsKeyPressed(KEY_W))   dirBuffer.push_back({4, now});

    while (dirBuffer.size() > 8) dirBuffer.pop_front();

    auto hasSequence = [&](int a, int b, int c) -> bool {
        int step = 0;
        for (const auto& item : dirBuffer)
        {
            if ((now - item.time) > bufferWindow) continue;
            if (step == 0 && item.dir == a)      step = 1;
            else if (step == 1 && item.dir == b) step = 2;
            else if (step == 2 && item.dir == c) return true;
        }
        return false;
    };

    auto isSpecial = [&](Move* mv) -> bool {
        return (mv != nullptr) && (mv == nsr.Special1 || mv == nsr.Special2);
    };

    // =========================================================================
    // ZONA DE CONFIGURACIÓN POR MOVIMIENTO (Profundidad y Offsets de capas)
    // =========================================================================
    auto configureMoveVisuals = [&](Move* mv, bool flip) {
        float dirSign = flip ? -1.0f : 1.0f;

        DepthSwapStart = 0;
        DepthSwapEnd   = 0;
        PlayerOffSet1  = PlayerPosition;
        PlayerOffSet2  = PlayerPosition;

        auto applyLayerOffset = [&](Vector2& layerPos, float baseX, float baseY, float flipXOff, float flipYOff) {
            layerPos.x = PlayerPosition.x + (baseX * dirSign) + (flip ? flipXOff : 0.0f);
            layerPos.y = PlayerPosition.y + baseY + (flip ? flipYOff : 0.0f);
        };

        if (mv == nsr.Normal1) // --- THRUST ---
        {
            DepthSwapStart = 0;
            DepthSwapEnd   = 0;
            applyLayerOffset(PlayerOffSet1, 5.0f, 125.0f, -280.0f, 0.0f);
        }
        else if (mv == nsr.Normal2) // --- ROTARY CUT ---
        {
            DepthSwapStart = 5;
            DepthSwapEnd   = 12;
            applyLayerOffset(PlayerOffSet1, -160.0f, -90.0f, -350.0f, -10.0f);
        }
        else if (mv == nsr.Special1) // --- CRIMSON ORBIT ---
        {
            DepthSwapStart = 4;
            DepthSwapEnd   = 14;
            applyLayerOffset(PlayerOffSet1, 0.0f, 0.0f, 0.0f, 0.0f);
            applyLayerOffset(PlayerOffSet2, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        else if (mv == nsr.Special2) // --- NEON BURST ---
        {
            DepthSwapStart = 0;
            DepthSwapEnd   = 0;
            applyLayerOffset(PlayerOffSet1, 0.0f, 0.0f, 0.0f, 0.0f);
            applyLayerOffset(PlayerOffSet2, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        else if (mv == nsr.Ultimate) // --- PIERCED BY LIGHT ---
        {
            DepthSwapStart = 0;
            DepthSwapEnd   = 0;
            applyLayerOffset(PlayerOffSet1, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    };

    auto resolveMove = [&](Move* curr) -> Move* {
        if (hasSequence(1, 3, 1)) return nsr.Special1;
        if (hasSequence(1, 2, 1)) return nsr.Special2;

        if (P->GetIsAttack() && (curr == nsr.Normal1 || isSpecial(curr)))
        {
            return nsr.Normal2;
        }

        return nsr.Normal1;
    };

    auto startMove = [&](Move* next) {
        if (next == nullptr) return;

        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);

        if (moveLeft && !moveRight)      FlipX = true;
        else if (moveRight && !moveLeft) FlipX = false;

        configureMoveVisuals(next, FlipX);

        if (isSpecial(next))
        {
            specialCount++;
        }

        PlayerCombat.SetPrevMove(PlayerCombat.GetCurrentMove());
        PlayerCombat.SetCurrentMove(next);
        if (!PlayerCombat.ValidateMove()) return;

        P->SetIsAttack(true);
        bufferedNext = nullptr;
        dirBuffer.clear();
    };

    auto animFinished = [&](Move* mv, const AnimationState& st) -> bool {
        if (mv == nullptr) return true;
        bool f1 = (mv->GetAnimation1() == nullptr) || (st.CurrentFrame1 >= mv->GetAnimation1()->totalFrames - 1);
        bool f2 = (mv->GetAnimation2() == nullptr) || (st.CurrentFrame2 >= mv->GetAnimation2()->totalFrames - 1);
        bool f3 = (mv->GetAnimation3() == nullptr) || (st.CurrentFrame3 >= mv->GetAnimation3()->totalFrames - 1);
        return f1 && f2 && f3;
    };

    if (P->GetIsAttack() && current != nullptr)
    {
        AnimationState state = PlayerCombat.GetAnimState();

        if (IsKeyPressed(KEY_U) && bufferedNext == nullptr && specialCount < 2)
        {
            Move* candidate = resolveMove(current);
            if (candidate != nullptr && current->CanChainInto(candidate->GetAtName()))
            {
                bufferedNext = candidate;
            }
        }

        int total1 = current->GetAnimation1() ? current->GetAnimation1()->totalFrames : 1;
        int cancelFrame = (total1 * cancelPercent) / 100;

        if (bufferedNext != nullptr && state.CurrentFrame1 >= cancelFrame)
        {
            startMove(bufferedNext);
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
            return;
        }

        if (animFinished(current, state))
        {
            if (bufferedNext != nullptr)
            {
                startMove(bufferedNext);
                PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
                return;
            }

            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, true);
            PlayerCombat.SetPrevMove(nullptr);
            PlayerCombat.SetCurrentMove(nullptr);
            DepthSwapStart = 0;
            DepthSwapEnd   = 0;
            PlayerOffSet1  = PlayerPosition;
            PlayerOffSet2  = PlayerPosition;
            bufferedNext   = nullptr;
            specialCount   = 0;
            P->SetIsAttack(false);
            return;
        }

        PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
        return;
    }

    if (IsKeyPressed(KEY_U))
    {
        specialCount = 0;
        Move* initial = resolveMove(nullptr);
        if (initial == nsr.Normal1)
        {
            startMove(initial);
            PlayerCombat.ExecuteMove(DepthSwapStart, DepthSwapEnd, FlipX, Rotation, false);
            return;
        }
    }

    UpdateMoveBasePlayer1();
}