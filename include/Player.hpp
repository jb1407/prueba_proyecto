#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h"
#include "Mechanics.hpp" // Esto ya incluye LoadAssets.hpp automáticamente
#include "hitbox.hpp"    // <-- NUEVO: Incluimos el módulo de hitboxes

class Player {
private:
    Vector2 position;
    Vector2 size;
    float speed;
    float health;
    float maxHealth;

    MoveSetBase currentMove;
    CombatManager combatManager;
    bool flipX;

    Vector2 dodgeDir;
    float dodgeSpeed;

    // --- NUEVO: Cajas de colisión del Jugador ---
    Hitbox hurtbox;       // El cuerpo del jugador (para recibir daño)
    Hitbox attackHitbox;  // El área de impacto de su ataque

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

    // --- NUEVOS GETTERS PARA LAS HITBOXES ---
    Rectangle GetHurtbox() const { return hurtbox.GetRect(); }
    const Hitbox& GetAttackHitbox() const { return attackHitbox; }
};

#endif