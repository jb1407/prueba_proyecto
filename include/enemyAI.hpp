#ifndef ENEMYAI_HPP
#define ENEMYAI_HPP

#include <raylib.h>
#include <functional>
#include "CombatSystem.hpp"
#include "mtreenode.hpp"

using namespace Designar;

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

enum class AIAction {
    IDLE,
    APPROACH,
    DASH_FORWARD,
    RETREAT,
    ATTACK_LIGHT,
    ATTACK_HEAVY,
    BLOCK
};

enum class NodeType {
    CONDITION,
    ACTION
};

struct DecisionKey {
    NodeType type;
    AIAction action;
    std::function<bool(Vector2 pos, Vector2 targetPos, bool isTargetAttacking, int atkCount)> condition;

    DecisionKey(AIAction act)
        : type(NodeType::ACTION), action(act), condition(nullptr) {}

    DecisionKey(std::function<bool(Vector2, Vector2, bool, int)> cond)
        : type(NodeType::CONDITION), action(AIAction::IDLE), condition(cond) {}
};

class EnemyAI {
private:
    Vector2 CPUPosition;
    Vector2 position;
    Vector2 lastTargetPos = { 0.0f, 0.0f };
    Vector2 speed;
    Vector2 size;
    Vector2 moveDir = { 0.0f, 0.0f };

    float health = 100.0f;
    float maxHealth = 100.0f;
    float damageMultiplier = 1.0f;

    AIAction currentAction = AIAction::IDLE;
    float attackRange = 70.0f;
    float alertRange = 600.0f;

    float reactionTimer = 0.0f;
    float reactionDelay = 0.25f;
    float actionDurationTimer = 0.0f;

    int playerAttackCounter = 0;
    Difficulty currentDifficulty = Difficulty::MEDIUM;

    // Sistema de movimiento y animaciones actual
    MoveSetData moveSetData;
    MoveSetBase moveSet;
    CombatManager cpuCombat{ nullptr, nullptr, CombatMode::UnArmed };

    int depthSwapStart = 0;
    int depthSwapEnd = 0;
    float rotation = 0.0f;
    bool flipX = false;

    MTreeNode<DecisionKey>* decisionTreeRoot = nullptr;

    MTreeNode<DecisionKey>* BuildEasyTree();
    MTreeNode<DecisionKey>* BuildMediumTree();
    MTreeNode<DecisionKey>* BuildHardTree();
    MTreeNode<DecisionKey>* BuildDecisionTree(Difficulty diff);

    void ClampToScreen(int screenWidth, int screenHeight);
    void RunMove(Move* targetMove, float dx, float dy);
    void FaceTarget(Vector2 targetPos);

public:
    EnemyAI(float startX, float startY, AssetManager& assets, Difficulty diff = Difficulty::MEDIUM);
    ~EnemyAI();

    void Update(Vector2 targetPos, bool isTargetAttacking, int screenWidth, int screenHeight);
    AIAction EvaluateTree(Vector2 targetPos, bool isTargetAttacking);
    void ExecuteAction(AIAction action, Vector2 targetPos, float dt, int screenWidth, int screenHeight);

    void TakeDamage(float amount);
    bool IsDead() const { return health <= 0.0f; }

    Vector2 GetPosition() const { return position; }
    float GetHealth() const { return health; }
    float GetMaxHealth() const { return maxHealth; }
    float GetDamageMultiplier() const { return damageMultiplier; }
    AIAction GetCurrentAction() const { return currentAction; }
    Difficulty GetDifficulty() const { return currentDifficulty; }
};

#endif