#include "Mechanics.hpp"

// Función auxiliar estática o wrapper para el hash de strings en DESIGNAR
static Designar::nat_t hashString(const std::string& k) {return Designar::super_fast_hash(k);}

Move::Move(const std::string& name, int state, Animation* a1, Animation* a2, Animation* a3, Vector2& off1, Vector2& off2, Vector2& off3) : At_Name(name), State(state), Anim1(a1), Anim2(a2), Anim3(a3), OffSet1(off1), OffSet2(off2), OffSet3(off3), AllowedFollowUps(11, FollowUpComparator{}, hashString) {}

Animation* Move::GetAnimation1() const {return Anim1;}
Animation* Move::GetAnimation2() const {return Anim2;}
Animation* Move::GetAnimation3() const {return Anim3;}

bool Move::GetChainable() {return !AllowedFollowUps.is_empty();}

std::string Move::GetAtName() {return At_Name;}

void Move::SetAtName(std::string name) {At_Name = name;}
void Move::SetState(int state) {State = state;}

void Move::AddAllowedFollowUp(const std::string& nextMoveName) {AllowedFollowUps.insert(nextMoveName);}

bool Move::CanChainInto(const std::string& nextMoveName) const {return AllowedFollowUps.has(nextMoveName);}

void Move::AnimationLogicAndDraw(int frameDepthSwapStart, int frameDepthSwapEnd, bool flipX, float rotation, bool Finished) //No es adivina, ser conciente de que si la animacion es de 1 sprite, hay que colocarla en Anim1 y asi sucesivamente
{
    if(!Finished)
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

            Rectangle SourceRec3 = { ((float)Anim3->currentFrame*Anim3->frameWidth), 0.0f, SourceWidth3, SourceHeight3};
            Rectangle SourceRec2 = { ((float)Anim2->currentFrame*Anim2->frameWidth), 0.0f, SourceWidth2, SourceHeight2};
            Rectangle SourceRec1 = { ((float)Anim1->currentFrame*Anim1->frameWidth), 0.0f, SourceWidth1, SourceHeight1};

            Rectangle DestRec3 = {OffSet3.x, OffSet3.y, (float)Anim3->frameWidth, (float)Anim3->frameHeight};
            Rectangle DestRec2 = {OffSet2.x, OffSet2.y, (float)Anim2->frameWidth, (float)Anim2->frameHeight};
            Rectangle DestRec1 = {OffSet1.x, OffSet1.y, (float)Anim1->frameWidth, (float)Anim1->frameHeight};

            if(Anim1->currentFrame >= frameDepthSwapStart && Anim1->currentFrame < frameDepthSwapEnd)
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

            if(Anim3->tickCounter == Anim3->frameSpeed)
            {
                if(!(Anim1->currentFrame == (Anim1->totalFrames)-1 )) Anim1->currentFrame++;
                if(!(Anim2->currentFrame == (Anim2->totalFrames)-1 )) Anim2->currentFrame++;
                if(!(Anim3->currentFrame == (Anim3->totalFrames)-1 )) Anim3->currentFrame++;
                Anim3->tickCounter = 0;
            }
            else{Anim3->tickCounter++;}
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

            Rectangle SourceRec2 = { ((float)Anim2->currentFrame*Anim2->frameWidth), 0.0f, SourceWidth2, SourceHeight2};
            Rectangle SourceRec1 = { ((float)Anim1->currentFrame*Anim1->frameWidth), 0.0f, SourceWidth1, SourceHeight1};

            Rectangle DestRec2 = {OffSet2.x, OffSet2.y, (float)Anim2->frameWidth, (float)Anim2->frameHeight};
            Rectangle DestRec1 = {OffSet1.x, OffSet1.y, (float)Anim1->frameWidth, (float)Anim1->frameHeight};

            if(Anim1->currentFrame >= frameDepthSwapStart && Anim1->currentFrame < frameDepthSwapEnd)
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

            if(Anim2->tickCounter == Anim2->frameSpeed)
            {
                if(!(Anim1->currentFrame == (Anim1->totalFrames)-1 )) Anim1->currentFrame++;
                if(!(Anim2->currentFrame == (Anim2->totalFrames)-1 )) Anim2->currentFrame++;
                Anim2->tickCounter = 0;
            }
            else{Anim2->tickCounter++;}
        }
        else if(Anim1 != nullptr) //1 sprite
        {
            float SourceWidth1 = (float)Anim1->frameWidth;
            float SourceHeight1 = (float)Anim1->frameHeight;

            if(flipX){SourceWidth1 = -SourceWidth1;}

            Rectangle SourceRec1 = { ((float)Anim1->currentFrame*Anim1->frameWidth), 0.0f, SourceWidth1, SourceHeight1};

            Rectangle DestRec1 = {OffSet1.x, OffSet1.y, (float)Anim1->frameWidth, (float)Anim1->frameHeight};

            DrawTexturePro(Anim1->texture, SourceRec1, DestRec1, {0, 0}, rotation, WHITE);

            if(Anim1->tickCounter == Anim1->frameSpeed)
            {
                if(!(Anim1->currentFrame == (Anim1->totalFrames)-1 )) Anim1->currentFrame++;
                Anim1->tickCounter = 0;
            }
            else{Anim1->tickCounter++;}
        }
    }
    else
    {
        if(Anim1 != nullptr) {Anim1->currentFrame = 0; Anim1->tickCounter = 0;}
        if(Anim2 != nullptr) {Anim2->currentFrame = 0; Anim2->tickCounter = 0;}
        if(Anim3 != nullptr) {Anim3->currentFrame = 0; Anim3->tickCounter = 0;}
        return;
    }
}

Move::~Move() {}

// CombatManager
CombatManager::CombatManager(Move* prev, Move* initialMove, int mode) : PrevMove(prev), CurrentMove(initialMove), CurrentMode(mode) {}

bool CombatManager::ValidateMove() {return (CurrentMove != nullptr);}

bool CombatManager::ValidatePrevMove()
{
    if(PrevMove == nullptr){return true;} //al no haber movimiento previo, puede continuar
    if(CurrentMove == nullptr){return false;}
    if(PrevMove->CanChainInto(CurrentMove->GetAtName())){return true;} //El movimiento se encontró en la tabla hash (encadenable)
    return false; //si ninguna de las dos anteriores es valida, entonces no hay encadenamiento
}

int CombatManager::GetMode()const {return CurrentMode;}

void CombatManager::SetPrevMove(Move* NewPrev){PrevMove = NewPrev;}
void CombatManager::SetCurrentMove(Move* NewCurrent){CurrentMove = NewCurrent;}
void CombatManager::SetMode(int NewMode){CurrentMode = NewMode;}

void CombatManager::ExecuteMove(int frameDepthSwapStart, int frameDepthSwapEnd, bool flipX, float rotation, bool Finished) //Verifica si el movimiento es canselable, luego dibuja la animacion en caso de no serlo
{
    if(CurrentMove != nullptr){CurrentMove->AnimationLogicAndDraw(frameDepthSwapStart, frameDepthSwapEnd, flipX, rotation, Finished);} //Depende de la implementacion del sistema del jugador si el movimiento es o no es cancelable (explicito)
}