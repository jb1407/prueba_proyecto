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
    Vector2 position;
    Vector2 speed;
    Vector2 dashSpeed;
    Vector2 activeBackVel = { 0.0f, 0.0f };
    Vector2 size;
    Vector2 lastTargetPos = { 0.0f, 0.0f };

    float attackRange = 135.0f;
    float alertRange = 520.0f;
    AIAction currentAction = AIAction::IDLE;
    float decisionTimer = 0.0f;
    float decisionInterval = 0.25f;

    Difficulty currentDifficulty = Difficulty::MEDIUM;
    int playerAttackCounter = 0;
    int consecutiveAttacks = 0;
    const int maxConsecutiveAttacks = 2;
    bool pendingPostAttackRetreat = false;
    float forwardDashTimer;
    float forwardDashDirX;

    MoveSetData moveSetData;
    MoveSetBase moveSet;
    CombatManager cpuCombat{nullptr, nullptr, CombatMode::UnArmed};

    int depthSwapStart = 0;
    int depthSwapEnd = 0;
    float rotation = 0.0f;
    bool flipX = false;

    bool isAttacking = false;
    bool isBacking = false;

    MTreeNode<DecisionKey>* decisionTreeRoot = nullptr;

    MTreeNode<DecisionKey>* BuildEasyTree();
    MTreeNode<DecisionKey>* BuildMediumTree();
    MTreeNode<DecisionKey>* BuildHardTree();
    MTreeNode<DecisionKey>* BuildDecisionTree(Difficulty diff);

    void HandlePostAttack(int screenWidth, int screenHeight);
    void ClampToScreen(int screenWidth, int screenHeight);
    void RunMove(Move* targetMove, float dx, float dy);
    void FaceTarget(Vector2 targetPos);

public:
    EnemyAI(float startX, float startY, AssetManager& assets, Difficulty diff = Difficulty::MEDIUM);
    ~EnemyAI();

    // Evalúa el árbol de decisiones y actualiza/ejecuta el movimiento del enemigo
    void Update(Vector2 targetPos, bool isTargetAttacking, int screenWidth, int screenHeight);
    AIAction EvaluateTree(Vector2 targetPos, bool isTargetAttacking) const;

    // Ejecuta una acción simple en función de la posición del objetivo
    void ExecuteAction(AIAction action, Vector2 targetPos, int screenWidth, int screenHeight);

    // Movimientos básicos directos (útiles para pruebas o llamadas puntuales)
    void MoveUp(int screenWidth, int screenHeight);
    void MoveDown(int screenWidth, int screenHeight);
    void MoveLeft(int screenWidth, int screenHeight);
    void MoveRight(int screenWidth, int screenHeight);
    void AttackPunch(int screenWidth, int screenHeight);
    void DashBackward(float dirX, float dirY, int screenWidth, int screenHeight);
    void StandIdle(int screenWidth, int screenHeight);

    void UpdateFrame(int screenWidth, int screenHeight);

    Vector2 GetPosition() const { return position; }
    AIAction GetCurrentAction() const { return currentAction; }
    Difficulty GetDifficulty() const { return currentDifficulty; }
    bool IsBusy() const { return isAttacking || isBacking; }
};

#endif