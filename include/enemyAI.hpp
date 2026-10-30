#ifndef ENEMY_AI_HPP
#define ENEMY_AI_HPP

#include <raylib.h>
#include <functional>
#include "nodesdef.hpp" // Ajusta la ruta según donde tengas tu librería

using namespace Designar;

enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

enum class AIAction {
    IDLE,
    APPROACH,
    RETREAT,       // <-- Nuevo estado de retroceso táctico
    ATTACK_LIGHT,
    ATTACK_HEAVY,
    BLOCK,
    PATROL
};

enum class NodeType {
    CONDITION,
    ACTION
};

// Lo que guardará cada nodo del MTreeNode
struct DecisionKey {
    NodeType type;
    AIAction action; // Solo se usa si type == ACTION
    
    // Función de condición (solo si type == CONDITION)
    std::function<bool(Vector2 pos, Vector2 playerPos, bool isAttacking, int atkCount)> condition;

    // Constructor para Acciones (Hojas)
    DecisionKey(AIAction act) : type(NodeType::ACTION), action(act) {}

    // Constructor para Condiciones (Nodos internos)
    DecisionKey(std::function<bool(Vector2, Vector2, bool, int)> cond) 
        : type(NodeType::CONDITION), action(AIAction::IDLE), condition(cond) {}
};

class EnemyAI {
private:
    Vector2 position;
    Vector2 initialPosition;
    Vector2 speed;
    Vector2 size;
    int health;
    int maxHealth;
    
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

    // --- EL ÁRBOL USANDO MTreeNode DE DESIGNAR ---
    MTreeNode<DecisionKey>* decisionTreeRoot;

    // Métodos constructores separados por dificultad
    MTreeNode<DecisionKey>* BuildEasyTree();
    MTreeNode<DecisionKey>* BuildMediumTree();
    MTreeNode<DecisionKey>* BuildHardTree();

    MTreeNode<DecisionKey>* BuildDecisionTree(Difficulty diff = Difficulty::MEDIUM);
    void ClampToScreen(int screenWidth, int screenHeight);

public:
    EnemyAI(float startX, float startY, Difficulty diff);
    ~EnemyAI();

    void Update(Vector2 playerPosition, bool isPlayerAttacking, int screenWidth, int screenHeight);
    void Draw();
    void EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking);
    AIAction GetCurrentAction() const { return currentAction; }

    Rectangle GetRectangle() const { 
        return { position.x, position.y, size.x, size.y }; 
    }
    float GetAttackRange() const { return attackRange; }
};

#endif