#ifndef ENEMYAI_HPP
#define ENEMYAI_HPP

#include <raylib.h>
#include <functional>
#include "mtreenode.hpp"
#include "LoadAssets.hpp"
#include "hitbox.hpp"
#include "Weapon.hpp"

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
    std::function<bool(Vector2 pos, Vector2 playerPos, bool isAttacking, int atkCount)> condition;

    DecisionKey(AIAction act)
        : type(NodeType::ACTION), action(act), condition(nullptr) {}

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

    float reactionTimer;
    float reactionDelay;
    float actionDurationTimer;

    int playerAttackCounter;
    Difficulty currentDifficulty;

    Hitbox hurtbox;
    Hitbox attackHitbox;

    WeaponType equippedWeapon;
    float weaponDuration;
    bool hasWeapon;

    MTreeNode<DecisionKey>* decisionTreeRoot;

    MTreeNode<DecisionKey>* BuildEasyTree();
    MTreeNode<DecisionKey>* BuildMediumTree();
    MTreeNode<DecisionKey>* BuildHardTree();
    MTreeNode<DecisionKey>* BuildDecisionTree(Difficulty diff);

    void UpdateHitboxes();
    void ClampToScreen(int screenWidth, int screenHeight);

public:
    EnemyAI(float startX, float startY, Difficulty diff = Difficulty::MEDIUM);
    ~EnemyAI();

    void Update(Vector2 playerPos, bool isPlayerAttacking, int screenWidth, int screenHeight);
    void Draw(AssetManager& assets, Vector2 playerPos);

    void EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking);
    void ExecuteAction(AIAction action, Vector2 playerPos, float dt);

    void TakeDamage(float amount);
    bool IsDead() const { return health <= 0.0f; }

    void EquipWeapon(WeaponType type, float duration);
    void UpdateWeapon(float dt);
    bool HasWeapon() const { return hasWeapon; }
    WeaponType GetEquippedWeapon() const { return equippedWeapon; }

    Rectangle GetRectangle() const { return { position.x, position.y, size.x, size.y }; }
    Hitbox GetHurtbox() const { return hurtbox; }
    Hitbox GetAttackHitbox() const { return attackHitbox; }

    Vector2 GetPosition() const { return position; }
    float GetHealth() const { return health; }
    float GetMaxHealth() const { return maxHealth; }
    float GetDamageMultiplier() const { return damageMultiplier; }
    AIAction GetCurrentAction() const { return currentAction; }
    Difficulty GetDifficulty() const { return currentDifficulty; }
};

#endif