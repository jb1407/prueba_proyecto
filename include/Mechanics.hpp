#ifndef MECHANICS_HPP
#define MECHANICS_HPP

#include "LoadAssets.hpp"

class Move
{
    private:
    std::string At_Name;
    int State;      
    bool CoolDown;

    Animation* Anim1 = nullptr;
    Animation* Anim2 = nullptr;
    Animation* Anim3 = nullptr; 

    Vector2 OffSet1 = {0.0f, 0.0f};
    Vector2 OffSet2 = {0.0f, 0.0f};
    Vector2 OffSet3 = {0.0f, 0.0f};

    // Declarada una sola vez
    struct FollowUpComparator 
    {
        bool operator()(const std::string& a, const std::string& b) const { return a == b; }
    };

    Designar::SeparateChainingHashTable<std::string, FollowUpComparator> AllowedFollowUps;

    public:
    Move& operator=(const Move&) = delete;
    Move() = delete; 
    Move(const Move&) = delete;

    Move(const std::string&, int, bool, Animation*, Animation*, Animation*, Vector2&, Vector2&, Vector2&);

    Animation* GetAnimation1() const;
    Animation* GetAnimation2() const;
    Animation* GetAnimation3() const;
   
    bool GetCoolDown();
    bool GetChainable();
    std::string GetAtName();

    void SetAtName(std::string);
    void SetState(int);
    void SetCoolDown(bool);

    void AddAllowedFollowUp(const std::string& nextMoveName); 
    bool CanChainInto(const std::string& nextMoveName) const;

    void AnimationLogicAndDraw(int, int, bool, float); 

    ~Move();
};

class CombatManager
{
    private:
    std::string PrevMoveName = " "; 
    Move* CurrentMove = nullptr;
    int CurrentMode; 

    public:
    CombatManager() = delete;
    CombatManager& operator=(const CombatManager&) = delete;
    CombatManager(CombatManager&) = delete;
    CombatManager(const std::string&, Move*, int);

    bool ValidateMove(const Move&); 
    int SetState(int);
    void ExecuteMove();
    void UpdateAnExecute(int, int, bool, float);
};

#endif