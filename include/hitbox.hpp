#pragma once
#include <raylib.h>

class Hitbox {
private:
    Rectangle rect;    // Posición y tamaño del rectángulo de colisión
    bool active;       // Si está activa (para ataques) o siempre activa (para hurtboxes)

public:
    // Constructor
    Hitbox(float x, float y, float width, float height, bool isActive = true);

    // Actualiza la posición de la hitbox en base a la posición del personaje
    void UpdatePosition(float parentX, float parentY, float offsetX, float offsetY);

    // Verifica si colisiona con otro rectángulo (como la hurtbox del rival)
    bool CheckCollision(const Rectangle& target) const;

    // Métodos para control y depuración visual
    void SetActive(bool status) { active = status; }
    bool IsActive() const { return active; }
    
    // Dibuja la caja en pantalla (súper útil para depurar y ver dónde golpea)
    void Draw(Color color) const;

    // Getter del rectángulo interno de Raylib
    Rectangle GetRect() const { return rect; }
};