#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h" // Necesario para Vector2 y funciones de teclado

class Player {
private:
    Vector2 position;
    Vector2 size;
    float speed;
    float health;
    float maxHealth;

public:
    // Constructor
    Player(float startX, float startY);

    // Métodos principales
    void Update();
    void TakeDamage(float amount);
    bool IsDead() const { return health <= 0.0f; }

    // --- Getters compatibles ---
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    Vector2 getPosition() const { return position; }
    
    // ¡El codiciado GetRectangle idéntico al del EnemyAI!
    Rectangle GetRectangle() const { 
        return { position.x, position.y, size.x, size.y }; 
    }

    float GetHealth() const { return health; }
    float GetMaxHealth() const { return maxHealth; }
};

#endif