#include "hitbox.hpp"

Hitbox::Hitbox(float x, float y, float width, float height, bool isActive) {
    rect = { x, y, width, height };
    active = isActive;
}

void Hitbox::UpdatePosition(float parentX, float parentY, float offsetX, float offsetY) {
    rect.x = parentX + offsetX;
    rect.y = parentY + offsetY;
}

bool Hitbox::CheckCollision(const Rectangle& target) const {
    if (!active) return false; // Si no está activa, no colisiona
    return CheckCollisionRecs(rect, target);
}

void Hitbox::Draw(Color color) const {
    if (active) {
        // Dibuja el contorno y un relleno translúcido para ver la hitbox claramente
        DrawRectangleRec(rect, Fade(color, 0.4f));
        DrawRectangleLinesEx(rect, 1.5f, color);
    }
}