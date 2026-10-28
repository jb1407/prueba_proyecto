#include "enemyAI.hpp"
#include <cmath>

EnemyAI::EnemyAI(float startX, float startY) {
    position = { startX, startY };
    speed = { 3.0f, 0.0f };
    health = 100;
    maxHealth = 100;
    currentAction = AIAction::IDLE;
    
    attackRange = 60.0f;       // Rango para golpe cuerpo a cuerpo
    retreatDistance = 200.0f;   // Distancia de peligro/alerta

    alertRange = 300.0f;       // Si el jugador está a más de 300px, patrulla
    patrolCenter = startX;     // Toma su posición inicial como el centro de su área
    patrolLimit = 150.0f;      // Se moverá 150 píxeles a la izquierda y derecha de ese centro
    movingRight = true;        // Empieza caminando hacia la derecha
}

EnemyAI::~EnemyAI() {
    // Liberación del árbol MTreeNode cuando lo integremos
}

void EnemyAI::BuildDecisionTree() {
    // Aquí construiremos el árbol multinodo de DeSiGNAR
}

void EnemyAI::EvaluateTree(Vector2 playerPosition, bool isPlayerAttacking) {
    // 1. Calculamos la distancia actual en el eje X respecto al jugador
    float distance = std::abs(playerPosition.x - position.x);

    // 2. Definimos un rango de alerta o visión (ej. 300 píxeles)
    float alertRange = 300.0f;

    // 3. Lógica de decisiones jerárquica
    if (isPlayerAttacking && distance < attackRange + 20.0f) {
        currentAction = AIAction::BLOCK;
    } 
    else if (distance <= attackRange) {
        currentAction = AIAction::ATTACK_LIGHT;
    } 
    else if (distance > attackRange && distance < alertRange) {
        // El jugador está cerca pero fuera de rango: ¡Aproximarse!
        currentAction = AIAction::APPROACH;
    } 
    else {
        // --- AQUÍ VA EL COMPORTAMIENTO NPC / PATRULLA ---
        // El jugador está muy lejos, el bot entra en modo pasivo/patrulla
        currentAction = AIAction::PATROL;
    
        // Movimiento de ida y vuelta en un área definida
        if (movingRight) {
            position.x += speed.x * 0.5f; // Camina más lento cuando patrulla
            if (position.x > patrolCenter + patrolLimit) movingRight = false;
        } else {
            position.x -= speed.x * 0.5f;
            if (position.x < patrolCenter - patrolLimit) movingRight = true;
        }
    }
}

void EnemyAI::Update(Vector2 playerPosition, bool isPlayerAttacking) {
    EvaluateTree(playerPosition, isPlayerAttacking);

    // Aplicar movimiento según la acción decidida por el árbol
    switch (currentAction) {
        case AIAction::APPROACH:
            if (position.x < playerPosition.x) position.x += speed.x;
            else if (position.x > playerPosition.x) position.x -= speed.x;
            break;

        case AIAction::RETREAT:
            if (position.x < playerPosition.x) position.x -= speed.x;
            else if (position.x > playerPosition.x) position.x += speed.x;
            break;

        default:
            break;
        case AIAction::PATROL:
            // Movimiento pasivo de ida y vuelta
            // (puedes ajustar la velocidad con un multiplicador menor, ej. * 0.5f)
            position.x += speed.x * 0.5f; 
            break;
    }
}

void EnemyAI::Draw() {
    // Renderizado provisorio (Rectángulo rojo para la IA)
    DrawRectangleV(position, { 40, 80 }, RED);
    
    // Indicador visual de la acción actual
    const char* actionText = "IDLE";
    if (currentAction == AIAction::APPROACH) actionText = "ACERCÁNDOSE";
    else if (currentAction == AIAction::ATTACK_LIGHT) actionText = "ATACANDO";
    else if (currentAction == AIAction::BLOCK) actionText = "BLOQUEANDO";

    DrawText(actionText, position.x - 10, position.y - 20, 10, DARKGRAY);
}