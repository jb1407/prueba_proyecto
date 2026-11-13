#include "Mechanics.hpp"

// Función auxiliar estática o wrapper para el hash de strings en DESIGNAR
static Designar::nat_t hashString(const std::string& k) {return Designar::super_fast_hash(k);}

Move::Move(const std::string& name, int state, bool coolDown, Animation* a1, Animation* a2, Animation* a3, Vector2& off1, Vector2& off2, Vector2& off3) : At_Name(name), State(state), CoolDown(coolDown), Anim1(a1), Anim2(a2), Anim3(a3), OffSet1(off1), OffSet2(off2), OffSet3(off3),AllowedFollowUps(11, FollowUpComparator{}, hashString) {}

Animation* Move::GetAnimation1() const { return Anim1; }
Animation* Move::GetAnimation2() const { return Anim2; }
Animation* Move::GetAnimation3() const { return Anim3; }

bool Move::GetCoolDown() { return CoolDown; }

bool Move::GetChainable() { return !AllowedFollowUps.is_empty(); }

std::string Move::GetAtName() { return At_Name; }

void Move::SetAtName(std::string name) { At_Name = name; }
void Move::SetState(int state) { State = state; }
void Move::SetCoolDown(bool coolDown) { CoolDown = coolDown; }

void Move::AddAllowedFollowUp(const std::string& nextMoveName) {AllowedFollowUps.insert(nextMoveName);}

bool Move::CanChainInto(const std::string& nextMoveName) const 
{
    return AllowedFollowUps.has(nextMoveName);
}

void Move::AnimationLogicAndDraw(int frameDepthSwapStart, int frameDepthSwapEnd, bool flipX, float rotation) 
{
    // Posición central de la pantalla (puedes cambiarla luego por la posición de tu jugador)
    Vector2 screenCenter = { (float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f };

    if (Anim1 != nullptr && Anim1->texture.id != 0) 
    {
        float sourceWidth = (float)Anim1->frameWidth;
        float sourceHeight = (float)Anim1->frameHeight;

        // Si flipX es true, invertimos el ancho para voltear el sprite a la izquierda
        if (flipX) sourceWidth = -sourceWidth;

        Rectangle sourceRec = { (float)(Anim1->currentFrame * Anim1->frameWidth), 0.0f, sourceWidth, sourceHeight };
        
        Vector2 destPosition = { screenCenter.x + OffSet1.x, screenCenter.y + OffSet1.y };
        Rectangle destRec = { destPosition.x, destPosition.y, (float)Anim1->frameWidth, (float)Anim1->frameHeight };
        Vector2 origin = { (float)Anim1->frameWidth / 2.0f, (float)Anim1->frameHeight / 2.0f };

        // Dibujar en pantalla con Raylib
        DrawTexturePro(Anim1->texture, sourceRec, destRec, origin, rotation, WHITE);

        // Control de animación (avantar frames según frameSpeed)
        Anim1->tickCounter++;
        if (Anim1->tickCounter >= Anim1->frameSpeed) 
        {
            Anim1->tickCounter = 0;
            Anim1->currentFrame++;
            if (Anim1->currentFrame >= Anim1->totalFrames) 
            {
                Anim1->currentFrame = 0; // Bucle de la animación
            }
        }
    }

    if (Anim2 != nullptr && Anim2->texture.id != 0) 
    {
        float sourceWidth2 = (float)Anim2->frameWidth;
        float sourceHeight2 = (float)Anim2->frameHeight;

        if (flipX) sourceWidth2 = -sourceWidth2;

        Rectangle sourceRec2 = { (float)(Anim2->currentFrame * Anim2->frameWidth), 0.0f, sourceWidth2, sourceHeight2 };
        Vector2 destPosition2 = { screenCenter.x + OffSet2.x, screenCenter.y + OffSet2.y };
        Rectangle destRec2 = { destPosition2.x, destPosition2.y, (float)Anim2->frameWidth, (float)Anim2->frameHeight };
        Vector2 origin2 = { (float)Anim2->frameWidth / 2.0f, (float)Anim2->frameHeight / 2.0f };

        DrawTexturePro(Anim2->texture, sourceRec2, destRec2, origin2, rotation, WHITE);

        Anim2->tickCounter++;
        if (Anim2->tickCounter >= Anim2->frameSpeed) 
        {
            Anim2->tickCounter = 0;
            Anim2->currentFrame++;
            if (Anim2->currentFrame >= Anim2->totalFrames) 
            {
                Anim2->currentFrame = 0;
            }
        }
    }

    if (Anim3 != nullptr && Anim3->texture.id != 0) 
    {
        float sourceWidth3 = (float)Anim3->frameWidth;
        float sourceHeight3 = (float)Anim3->frameHeight;

        if (flipX) sourceWidth3 = -sourceWidth3;

        Rectangle sourceRec3 = { (float)(Anim3->currentFrame * Anim3->frameWidth), 0.0f, sourceWidth3, sourceHeight3 };
        Vector2 destPosition3 = { screenCenter.x + OffSet3.x, screenCenter.y + OffSet3.y };
        Rectangle destRec3 = { destPosition3.x, destPosition3.y, (float)Anim3->frameWidth, (float)Anim3->frameHeight };
        Vector2 origin3 = { (float)Anim3->frameWidth / 2.0f, (float)Anim3->frameHeight / 2.0f };

        DrawTexturePro(Anim3->texture, sourceRec3, destRec3, origin3, rotation, WHITE);

        Anim3->tickCounter++;
        if (Anim3->tickCounter >= Anim3->frameSpeed) 
        {
            Anim3->tickCounter = 0;
            Anim3->currentFrame++;
            if (Anim3->currentFrame >= Anim3->totalFrames) 
            {
                Anim3->currentFrame = 0;
            }
        }
    }
}

Move::~Move() {}

// CombatManager
CombatManager::CombatManager(const std::string& prevName, Move* initialMove, int mode) : PrevMoveName(prevName), CurrentMove(initialMove), CurrentMode(mode) {}

bool CombatManager::ValidateMove(const Move&) { return true; }
int CombatManager::SetState(int mode) { return CurrentMode = mode; }
void CombatManager::ExecuteMove() {}
void CombatManager::UpdateAnExecute(int, int, bool, float) {}