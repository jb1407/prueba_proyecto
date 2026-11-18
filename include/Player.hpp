#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h"
#include "Mechanics.hpp" // Esto ya incluye LoadAssets.hpp automáticamente
#include "hitbox.hpp"    // <-- NUEVO: Incluimos el módulo de hitboxes
#include "Weapon.hpp"

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
    float dodgeTimer;

    // --- NUEVO: Cajas de colisión del Jugador ---
    Hitbox hurtbox;       // El cuerpo del jugador (para recibir daño)
    Hitbox attackHitbox;  // El área de impacto de su ataque

    float invincibilityTimer;
    bool isInvincible;
    bool hasHitEnemy;

    WeaponType equippedWeapon = WeaponType::NONE;
    float weaponDurationTimer = 0.0f;

    void ClampToScreen(int screenWidth, int screenHeight);
public:
    // Constructor
    Player(float startX, float startY);

    // Métodos principales
    void Update(AssetManager& assetManager, int screenWidth, int screenHeight);
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

    // En Player.hpp, reemplaza la línea de GetHurtbox por:
    Hitbox& GetHurtbox() { return hurtbox; }
    const Hitbox& GetAttackHitbox() const { return attackHitbox; }

    bool HasHitEnemy() const { return hasHitEnemy; }
    void SetHasHitEnemy(bool hit) { hasHitEnemy = hit; }

    void EquipWeapon(WeaponType type, float duration = 10.0f) {
        equippedWeapon = type;
        weaponDurationTimer = duration;
        // Aquí en el futuro llamaremos a una función genérica: ApplyWeaponEffect();
    }

    bool HasWeapon() const {
        return equippedWeapon != WeaponType::NONE;
    }

    WeaponType GetEquippedWeapon() const {
        return equippedWeapon;
    }

    // Método para actualizar el estado del arma (desgaste, temporizador, etc.)
    void UpdateWeapon(float dt) {
        if (HasWeapon()) {
            weaponDurationTimer -= dt;
            if (weaponDurationTimer <= 0.0f) {
                equippedWeapon = WeaponType::NONE; // Se acaba el efecto del arma
            }
        }
    }
};

#endif