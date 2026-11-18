#include "Weapon.hpp"

Weapon::Weapon(float startX, float startY, WeaponType wType, float lifetime) 
    : pickupHitbox(startX, startY, 40.0f, 40.0f, false) {
    position = { startX, startY };
    size = { 40.0f, 40.0f }; // Tamaño estándar del ítem en el suelo
    type = wType;
    isActive = true;
    lifetimeTimer = lifetime;
}

void Weapon::Update(float dt) {
    if (!isActive) return;

    // Reducir el tiempo de vida del arma en el suelo
    lifetimeTimer -= dt;
    if (lifetimeTimer <= 0.0f) {
        isActive = false; // El arma desaparece si pasa el tiempo y no la recogen
    }

    // Actualizamos la posición de la caja para que coincida con el ítem
    pickupHitbox.UpdatePosition(position.x, position.y, 0.0f, 0.0f);
}

void Weapon::Draw() {
    if (!isActive) return;

    // Dibujo temporal con colores según el tipo de arma (mientras añades sprites o texturas)
    Color weaponColor = GOLD; // Por defecto para SWORD
    if (type == WeaponType::SPEAR) weaponColor = SKYBLUE;
    if (type == WeaponType::BLASTER) weaponColor = PURPLE;

    // Dibujamos un pequeño cuadrado que representa el arma en el suelo
    DrawRectangleV(position, size, weaponColor);
    DrawRectangleLines((int)position.x, (int)position.y, (int)size.x, (int)size.y, WHITE);
}