#include "Mechanics.hpp"

// Función auxiliar estática o wrapper para el hash de strings en DESIGNAR
static Designar::nat_t hashString(const std::string& k) {return Designar::super_fast_hash(k);}

Move::Move(const std::string& name, Animation* a1, Animation* a2, Animation* a3, Vector2* off1, Vector2* off2, Vector2* off3) : At_Name(name), Anim1(a1), Anim2(a2), Anim3(a3), OffSet1(off1), OffSet2(off2), OffSet3(off3), AllowedFollowUps(11, FollowUpComparator{}, hashString) {}

Vector2 PlayerPosition = {0, 0}; 
Vector2 PlayerOffSet1 = {0, 0};
Vector2 PlayerOffSet2 = {0, 0};

Vector2 Player2Position = {0, 0}; 
Vector2 Player2OffSet1 = {0, 0};
Vector2 Player2OffSet2 = {0, 0};

Vector2 CPUPosition = {0, 0}; 
Vector2 CPUOffSet1 = {0, 0};
Vector2 CPUOffSet2 = {0, 0};

Animation* Move::GetAnimation1() const {return Anim1;}
Animation* Move::GetAnimation2() const {return Anim2;}
Animation* Move::GetAnimation3() const {return Anim3;}

bool Move::GetChainable() {return !AllowedFollowUps.is_empty();}

std::string Move::GetAtName() {return At_Name;}

void Move::SetAtName(std::string name) {At_Name = name;}
void Move::AddAllowedFollowUp(const std::string& nextMoveName) {AllowedFollowUps.insert(nextMoveName);}

bool Move::CanChainInto(const std::string& nextMoveName) const {return AllowedFollowUps.has(nextMoveName);}

void Move::AnimationLogicAndDraw(int frameDepthSwapStart, int frameDepthSwapEnd, bool flipX, float rotation, AnimationState* State, bool FinishedOrReset) //No es adivina, ser conciente de que si la animacion es de 1 sprite, hay que colocarla en Anim1 y asi sucesivamente
{
    if(!FinishedOrReset)
    {
        if(Anim3 != nullptr) //Para animaciones de 3 sprites
        {
            float SourceWidth3 = (float)Anim3->frameWidth;
            float SourceHeight3 = (float)Anim3->frameHeight;
            float SourceWidth2 = (float)Anim2->frameWidth;
            float SourceHeight2 = (float)Anim2->frameHeight;
            float SourceWidth1 = (float)Anim1->frameWidth;
            float SourceHeight1 = (float)Anim1->frameHeight;

            if(flipX)
            {
                SourceWidth1 = -SourceWidth1;
                SourceWidth2 = -SourceWidth2;
                SourceWidth3 = -SourceWidth3;
            }

            Rectangle SourceRec3 = { ((float)State->CurrentFrame3*Anim3->frameWidth), 0.0f, SourceWidth3, SourceHeight3};
            Rectangle SourceRec2 = { ((float)State->CurrentFrame2*Anim2->frameWidth), 0.0f, SourceWidth2, SourceHeight2};
            Rectangle SourceRec1 = { ((float)State->CurrentFrame1*Anim1->frameWidth), 0.0f, SourceWidth1, SourceHeight1};

            Rectangle DestRec3 = {OffSet3->x, OffSet3->y, (float)Anim3->frameWidth, (float)Anim3->frameHeight};
            Rectangle DestRec2 = {OffSet2->x, OffSet2->y, (float)Anim2->frameWidth, (float)Anim2->frameHeight};
            Rectangle DestRec1 = {OffSet1->x, OffSet1->y, (float)Anim1->frameWidth, (float)Anim1->frameHeight};

            if(State->CurrentFrame1 >= frameDepthSwapStart && State->CurrentFrame1 < frameDepthSwapEnd)
            {
                DrawTexturePro(Anim1->texture, SourceRec1, DestRec1, {0, 0}, rotation, WHITE);
                DrawTexturePro(Anim3->texture, SourceRec3, DestRec3, {0, 0}, rotation, WHITE);
                DrawTexturePro(Anim2->texture, SourceRec2, DestRec2, {0, 0}, rotation, WHITE);
               
            }
            else
            {  
                DrawTexturePro(Anim3->texture, SourceRec3, DestRec3, {0, 0}, rotation, WHITE);
                DrawTexturePro(Anim2->texture, SourceRec2, DestRec2, {0, 0}, rotation, WHITE);   
                DrawTexturePro(Anim1->texture, SourceRec1, DestRec1, {0, 0}, rotation, WHITE);
            }

            //Verificamos cual sprite debe paralizarse y cuales continuan (En caso de que en una animacion haya un sprite con mas fotogramas que otros)

            if(State->TicksCounter3 == Anim3->frameSpeed)
            {
                if(!(State->CurrentFrame1 == (Anim1->totalFrames)-1 )) State->CurrentFrame1++;
                if(!(State->CurrentFrame2 == (Anim2->totalFrames)-1 )) State->CurrentFrame2++;
                if(!(State->CurrentFrame3 == (Anim3->totalFrames)-1 )) State->CurrentFrame3++;
                State->TicksCounter3 = 0;
            }
            else{State->TicksCounter3++;}
        }
        else if(Anim2 != nullptr) //Animaciones de 2 sprites
        {
            float SourceWidth2 = (float)Anim2->frameWidth;
            float SourceHeight2 = (float)Anim2->frameHeight;
            float SourceWidth1 = (float)Anim1->frameWidth;
            float SourceHeight1 = (float)Anim1->frameHeight;

            if(flipX)
            {
                SourceWidth1 = -SourceWidth1;
                SourceWidth2 = -SourceWidth2;
            }

            Rectangle SourceRec2 = { ((float)State->CurrentFrame2*Anim2->frameWidth), 0.0f, SourceWidth2, SourceHeight2};
            Rectangle SourceRec1 = { ((float)State->CurrentFrame1*Anim1->frameWidth), 0.0f, SourceWidth1, SourceHeight1};

            Rectangle DestRec2 = {OffSet2->x, OffSet2->y, (float)Anim2->frameWidth, (float)Anim2->frameHeight};
            Rectangle DestRec1 = {OffSet1->x, OffSet1->y, (float)Anim1->frameWidth, (float)Anim1->frameHeight};

            if(State->CurrentFrame1 >= frameDepthSwapStart && State->CurrentFrame1 < frameDepthSwapEnd)
            {
                DrawTexturePro(Anim1->texture, SourceRec1, DestRec1, {0, 0}, rotation, WHITE);
                DrawTexturePro(Anim2->texture, SourceRec2, DestRec2, {0, 0}, rotation, WHITE);
            }
            else
            {  
                DrawTexturePro(Anim2->texture, SourceRec2, DestRec2, {0, 0}, rotation, WHITE);   
                DrawTexturePro(Anim1->texture, SourceRec1, DestRec1, {0, 0}, rotation, WHITE);
            }

            //Verificamos cual sprite debe paralizarse y cuales continuan (En caso de que en una animacion haya un sprite con mas fotogramas que otros)

            if(State->TicksCounter2 == Anim2->frameSpeed)
            {
                if(!(State->CurrentFrame1 == (Anim1->totalFrames)-1 )) State->CurrentFrame1++;
                if(!(State->CurrentFrame2 == (Anim2->totalFrames)-1 )) State->CurrentFrame2++;
                State->TicksCounter2 = 0;
            }
            else{State->TicksCounter2++;}
        }
        else if(Anim1 != nullptr) //1 sprite
        {
            float SourceWidth1 = (float)Anim1->frameWidth;
            float SourceHeight1 = (float)Anim1->frameHeight;

            if(flipX){SourceWidth1 = -SourceWidth1;}

            Rectangle SourceRec1 = { ((float)State->CurrentFrame1*Anim1->frameWidth), 0.0f, SourceWidth1, SourceHeight1};

            Rectangle DestRec1 = {OffSet1->x, OffSet1->y, (float)Anim1->frameWidth, (float)Anim1->frameHeight};

            DrawTexturePro(Anim1->texture, SourceRec1, DestRec1, {0, 0}, rotation, WHITE);

            if(State->TicksCounter1 == Anim1->frameSpeed)
            {
                if(!(State->CurrentFrame1 == (Anim1->totalFrames)-1 )) State->CurrentFrame1++;
                State->TicksCounter1 = 0;
            }
            else{State->TicksCounter1++;}
        }
    }
    else
    {
        if(Anim1 != nullptr) {State->CurrentFrame1 = 0; State->TicksCounter1 = 0;}
        if(Anim2 != nullptr) {State->CurrentFrame2 = 0; State->TicksCounter2 = 0;}
        if(Anim3 != nullptr) {State->CurrentFrame3 = 0; State->TicksCounter3 = 0;}
        return;
    }
}

Move::~Move() {}

// CombatManager
CombatManager::CombatManager(Move* prev, Move* initialMove, CombatMode mode) : PrevMove(prev), CurrentMove(initialMove), CurrentMode(mode) {}

bool CombatManager::ValidateMove() {return (CurrentMove != nullptr);}

bool CombatManager::ValidatePrevMove()
{
    if(PrevMove == nullptr){return true;} //al no haber movimiento previo, puede continuar
    if(CurrentMove == nullptr){return false;}
    if(PrevMove->CanChainInto(CurrentMove->GetAtName())){return true;} //El movimiento se encontró en la tabla hash (encadenable)
    return false; //si ninguna de las dos anteriores es valida, entonces no hay encadenamiento
}

int CombatManager::GetMode()const {return CurrentMode;}
AnimationState CombatManager::GetAnimState() const{return CurrentAnimState;}
Move* CombatManager::GetCurrentMove() const{return CurrentMove;}

void CombatManager::SetPrevMove(Move* NewPrev){PrevMove = NewPrev;}
void CombatManager::SetCurrentMove(Move* NewCurrent){CurrentMove = NewCurrent;}
void CombatManager::SetMode(CombatMode NewMode){CurrentMode = NewMode;}

void CombatManager::ExecuteMove(int frameDepthSwapStart, int frameDepthSwapEnd, bool flipX, float rotation, bool FinishedOrReset) //Verifica si el movimiento es cancelable, luego dibuja la animacion en caso de no serlo
{
    if(CurrentMove != nullptr){CurrentMove->AnimationLogicAndDraw(frameDepthSwapStart, frameDepthSwapEnd, flipX, rotation, &CurrentAnimState, FinishedOrReset);} //Depende de la implementacion del sistema del jugador si el movimiento es o no es cancelable (explicito) -> Se hizo en CombatSystem
}