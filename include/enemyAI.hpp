#ifndef ENEMYAI_HPP
#define ENEMYAI_HPP

#include <raylib.h>
#include <functional>
#include "mtreenode.hpp" // Ajusta la ruta según donde tengas tu librería
#include "Mechanics.hpp"
#include "hitbox.hpp"    // <-- NUEVO: Incluimos el módulo de hitboxes

using namespace Designar;

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

enum class AIAction {
    IDLE,
    APPROACH,
    RETREAT,
    ATTACK_LIGHT,
    ATTACK_HEAVY,
    BLOCK,
    PATROL
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
    
    int health;
    int maxHealth;
    float damageMultiplier;
    AIAction currentAction;
    
    float attackRange;
    float retreatDistance;
    float alertRange;
    
    Vector2 patrolCenter;
    int patrolCornerIndex;
    float rectWidth;
    float rectHeight;
    Vector2 patrolDirection;

    int playerAttackCounter;
    Difficulty currentDifficulty;

    float reactionTimer = 0.0f;
    float reactionDelay = 0.2f;

    // --- NUEVO: Cajas de colisión del Enemigo ---
    Hitbox hurtbox;       // El cuerpo del enemigo (para recibir daño)
    Hitbox attackHitbox;  // El área de impacto cuando ataca

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

    // --- NUEVOS GETTERS PARA LAS HITBOXES ---
    Rectangle GetHurtbox() const { return hurtbox.GetRect(); }
    const Hitbox& GetAttackHitbox() const { return attackHitbox; }

    void TakeDamage(float amount);
    bool IsDead() const { return health <= 0.0f; }
};

#endif