#ifndef ENEMYAI_HPP
#define ENEMYAI_HPP

#include <raylib.h>
#include <functional>
#include "mtreenode.hpp"
#include "Mechanics.hpp"
#include "hitbox.hpp"

using namespace Designar;

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

enum class AIAction {
    IDLE,
    APPROACH,
    DASH_FORWARD, // Impulso rápido hacia el jugador para cerrar distancia
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
    std::function<bool(Vector2 pos, Vector2 playerPos, bool isAttacking, int atkCount)> condition;

    DecisionKey(AIAction act) : type(NodeType::ACTION), action(act) {}
    DecisionKey(std::function<bool(Vector2, Vector2, bool, int)> cond) 
        : type(NodeType::CONDITION), action(AIAction::IDLE), condition(cond) {}
};

class EnemyAI {
private:
    Vector2 position;
    Vector2 initialPosition;
    Vector2 speed;
    Vector2 size;
    Vector2 moveDir;
    
    float health;
    float maxHealth;
    float damageMultiplier;
    AIAction currentAction;
    
    float attackRange;
    float alertRange;
    int playerAttackCounter;
    Difficulty currentDifficulty;

    float reactionTimer = 0.0f;
    float reactionDelay = 0.2f;
    float actionDurationTimer = 0.0f; // Bloquea el cambio de decisión durante amagues o dashes

    // Cajas de colisión del Enemigo
    Hitbox hurtbox;
    Hitbox attackHitbox;

    MTreeNode<DecisionKey>* decisionTreeRoot;

    MTreeNode<DecisionKey>* BuildEasyTree();
    MTreeNode<DecisionKey>* BuildMediumTree();
    MTreeNode<DecisionKey>* BuildHardTree();

    MTreeNode<DecisionKey>* BuildDecisionTree(Difficulty diff = Difficulty::MEDIUM);
    void ClampToScreen(int screenWidth, int screenHeight);

public:
    EnemyAI(float startX, float startY, Difficulty diff);
    ~EnemyAI();

    void Update(Vector2 playerPosition, bool isPlayerAttacking, int screenWidth, int screenHeight);
    void Draw(AssetManager& manager, Vector2 playerPosition);
    void EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking);
    AIAction GetCurrentAction() const { return currentAction; }

    Rectangle GetRectangle() const { 
        return { position.x, position.y, size.x, size.y }; 
    }
    float GetAttackRange() const { return attackRange; }
    float GetHealth() const { return health; }
    float GetMaxHealth() const { return maxHealth; }

    Hitbox& GetHurtbox() { return hurtbox; }
    const Hitbox& GetAttackHitbox() const { return attackHitbox; }

    void TakeDamage(float amount);
    bool IsDead() const { return health <= 0.0f; }
};

#endif