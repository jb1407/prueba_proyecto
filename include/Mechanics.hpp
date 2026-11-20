#ifndef MECHANICS_HPP
#define MECHANICS_HPP

#include "LoadAssets.hpp"

class Move
{
    private:
    std::string At_Name;
    int State;      

    Animation* Anim1;
    Animation* Anim2;
    Animation* Anim3; 

    Vector2 OffSet1 = {0.0f, 0.0f};
    Vector2 OffSet2 = {0.0f, 0.0f};
    Vector2 OffSet3 = {0.0f, 0.0f};

    struct FollowUpComparator 
    {
        bool operator()(const std::string& a, const std::string& b) const { return a == b; }
    };

    Designar::SeparateChainingHashTable<std::string, FollowUpComparator> AllowedFollowUps;

    public:
    Move& operator=(const Move&) = delete;
    Move() = delete; 
    Move(const Move&) = delete;

    Move(const std::string&, int, Animation*, Animation*, Animation*, Vector2&, Vector2&, Vector2&);

    Animation* GetAnimation1() const;
    Animation* GetAnimation2() const;
    Animation* GetAnimation3() const;
   
    bool GetChainable();
    std::string GetAtName();

    void SetAtName(std::string);
    void SetState(int);

    void AddAllowedFollowUp(const std::string& nextMoveName); 
    bool CanChainInto(const std::string& nextMoveName) const;

    void AnimationLogicAndDraw(int, int, bool, float, bool); //los tres ultimos booleanos son quienes dicen si una animacion debe quedarse estática

    ~Move();
};

class CombatManager
{
    private:
    Move* PrevMove; 
    Move* CurrentMove;
    int CurrentMode; 

    public:
    CombatManager() = delete;
    CombatManager& operator=(const CombatManager&) = delete;
    CombatManager(CombatManager&) = delete;
    CombatManager(Move*, Move*, int);

    int GetMode() const;

    void SetPrevMove(Move*);
    void SetCurrentMove(Move*);
    void SetMode(int);

    bool ValidateMove();  //Valida que el movimiento actual no sea nullptr
    bool ValidatePrevMove(); //Valida si el movimiento actual es encadenable con el previo
    
    void ExecuteMove(int, int, bool, float, bool);
};

#endif