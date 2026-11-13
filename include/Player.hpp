#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h"
#include "Mechanics.hpp" // Esto ya incluye LoadAssets.hpp automáticamente

class Player {
private:
    Vector2 position;
    Vector2 size;
    float speed;
    float health;
    float maxHealth;

    // --- Nuevos atributos para las animaciones y mecánicas ---
    MoveSetBase currentMove;
    CombatManager combatManager;
    bool flipX;

    // --- Atributos para el movimiento de Backward (esquive con impulso) ---
    Vector2 dodgeDir;
    float dodgeSpeed;

public:
    // Constructor
    Player(float startX, float startY);

    // Métodos principales
    void Update(AssetManager& assetManager);
    void TakeDamage(float amount);
    bool IsDead() const { return health <= 0.0f; }

    // --- Getters compatibles ---
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    Vector2 getPosition() const { return position; }
    
    Rectangle GetRectangle() const { 
        return { position.x, position.y, size.x, size.y }; 
    }

    float GetHealth() const { return health; }
    float GetMaxHealth() const { return maxHealth; }
};

#endif