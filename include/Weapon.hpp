#ifndef WEAPON_HPP
#define WEAPON_HPP

#include <raylib.h>
#include "hitbox.hpp"

enum class WeaponType {
    NONE = -1, // Representa que no tiene ninguna arma equipada
    SWORD,
    SPEAR,
    BLASTER
};

class Weapon {
private:
    Vector2 position;
    Vector2 size;
    WeaponType type;
    bool isActive;
    float lifetimeTimer; // Tiempo antes de que desaparezca si no se recoge
    Hitbox pickupHitbox; // Caja para detectar cuando el jugador la toca

public:
    Weapon(float startX, float startY, WeaponType wType, float lifetime = 15.0f);

    void Update(float dt);
    void Draw();

    // Getters y Setters
    bool IsActive() const { return isActive; }
    void SetActive(bool active) { isActive = active; }
    
    Rectangle GetRectangle() const {
        return { position.x, position.y, size.x, size.y };
    }
    
    Hitbox& GetPickupHitbox() { return pickupHitbox; }
    WeaponType GetType() const { return type; }
};

#endif