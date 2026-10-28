#ifndef ENEMY_AI_HPP
#define ENEMY_AI_HPP

#include <raylib.h>

// Definición de las acciones que el bot puede tomar tras evaluar el árbol
enum class AIAction {
    IDLE,
    APPROACH,
    RETREAT,
    ATTACK_LIGHT,
    ATTACK_HEAVY,
    BLOCK,
    PATROL
};

class EnemyAI {
private:
    // Propiedades del bot
    Vector2 position;
    // Vector2 initialPosition;
    Vector2 speed;
    int health;
    int maxHealth;
    
    // Estado actual de decisión
    AIAction currentAction;
    
    // Parámetros de decisión (rangos en píxeles)
    float attackRange;
    float retreatDistance;

    float alertRange;     // Distancia a la que detecta al jugador
    float patrolCenter;   // Posición central de su zona de patrulla
    float patrolLimit;    // Qué tan lejos puede caminar a los lados
    bool movingRight;     // Dirección actual de la patrulla

public:
    EnemyAI(float startX, float startY);
    ~EnemyAI();

    // Ciclo principal de actualización y renderizado
    void Update(Vector2 playerPosition, bool isPlayerAttacking);
    void Draw();

    // Getters / Setters básicos
    Vector2 GetPosition() const { return position; }
    int GetHealth() const { return health; }
    AIAction GetCurrentAction() const { return currentAction; }

    // Métodos para construir y recorrer el árbol de decisión
    void BuildDecisionTree();
    void EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking);
};

#endif // ENEMY_AI_HPP