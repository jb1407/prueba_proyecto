#include "Player.hpp"

Player::Player(float startX, float startY) {
    position = { startX, startY };
    size = { 40.0f, 40.0f }; // Ancho y alto del rectángulo del jugador
    speed = 5.0f; 
    maxHealth = 100.0f;
    health = 100.0f;
}

void Player::Update() {
    // Movimiento directo usando position.x y position.y
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  position.x -= speed;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) position.x += speed;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    position.y -= speed;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  position.y += speed;
}

void Player::TakeDamage(float amount) {
    health -= amount;
    if (health < 0.0f) {
        health = 0.0f; // Evitamos valores negativos
    }
}