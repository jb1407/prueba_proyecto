#ifndef MECHANICS_HPP
#define MECHANICS_HPP

#include "LoadAssets.hpp"

enum CombatMode
{
    UnArmed = 0,
    OneHand,
    DualWield
};

struct AnimationState
{
    int CurrentFrame1 = 0;
    int CurrentFrame2 = 0;
    int CurrentFrame3 = 0;

    int TicksCounter1 = 0;
    int TicksCounter2 = 0;
    int TicksCounter3 = 0;
};

//estos son para el jugador,
extern Vector2 PlayerPosition; //Lo lógico es que vaya en player.cpp, pero tomando en cuenta que trabajará en sintnía con los demás, lo declaro aquí por seguridad
extern Vector2 PlayerOffSet1;
extern Vector2 PlayerOffSet2; //en base a ellos, se moverán tanto el personaje como todas las animaciones multicapa

extern Vector2 Player2Position; 
extern Vector2 Player2OffSet1;
extern Vector2 Player2OffSet2; 


//Estos son para el CPU
extern Vector2 CPUPosition; 
extern Vector2 CPUOffSet1;
extern Vector2 CPUOffSet2;

class Move
{
    private:
    std::string At_Name;      

    Animation* Anim1;
    Animation* Anim2;
    Animation* Anim3;

    Vector2* OffSet1;
    Vector2* OffSet2;
    Vector2* OffSet3;

    struct FollowUpComparator 
    {
        bool operator()(const std::string& a, const std::string& b) const { return a == b; }
    };

    Designar::SeparateChainingHashTable<std::string, FollowUpComparator> AllowedFollowUps;

    public:
    Move& operator=(const Move&) = delete;
    Move() = delete; 
    Move(const Move&) = delete;

    Move(const std::string&, Animation*, Animation*, Animation*, Vector2*, Vector2*, Vector2*);

    Animation* GetAnimation1() const;
    Animation* GetAnimation2() const;
    Animation* GetAnimation3() const;
   
    bool GetChainable();
    std::string GetAtName();

    void SetAtName(std::string);

    void AddAllowedFollowUp(const std::string& nextMoveName); 
    bool CanChainInto(const std::string& nextMoveName) const;

    void AnimationLogicAndDraw(int, int, bool, float, AnimationState*, bool); //los tres ultimos booleanos son quienes dicen si una animacion debe quedarse estática

    ~Move();
};

class CombatManager
{
    private:
    Move* PrevMove = nullptr; 
    Move* CurrentMove = nullptr;
    CombatMode CurrentMode; 
    AnimationState CurrentAnimState{0, 0, 0, 0, 0, 0};
   
    public:
    CombatManager() = delete;
    CombatManager& operator=(const CombatManager&) = delete;
    CombatManager(CombatManager&) = delete;
    CombatManager(Move*, Move*, CombatMode);

    int GetMode() const;
    AnimationState GetAnimState() const;
    Move* GetCurrentMove() const;

    void SetPrevMove(Move*);
    void SetCurrentMove(Move*);
    void SetMode(CombatMode);

    bool ValidateMove();  //Valida que el movimiento actual no sea nullptr
    bool ValidatePrevMove(); //Valida si el movimiento actual es encadenable con el previo
    
    void ExecuteMove(int, int, bool, float, bool);
};

#endif