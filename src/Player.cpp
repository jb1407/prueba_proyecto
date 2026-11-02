#include "Player.hpp"

Player::Player(float startX, float startY) {
    x = startX;
    y = startY;
    speed = 5.0f; // Velocidad de desplazamiento
}

void Player::Update() {
    // Aquí es donde vinculamos las teclas del teclado con tus métodos de movimiento
    // Puedes usar tanto las flechas como WASD
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  moverIzquierda();
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) moverDerecha();
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))     moverArriba();
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))   moverAbajo();
}

void Player::moverIzquierda() {
    x -= speed;
}

void Player::moverDerecha() {
    x += speed;
}

void Player::moverArriba() {
    y -= speed;
}

void Player::moverAbajo() {
    y += speed;
}

float Player::getX() const {
    return x;
}

float Player::getY() const {
    return y;
}

Vector2 Player::getPosition() const {
    return { x, y };
}