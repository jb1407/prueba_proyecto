#include "Player.hpp"

Player::Player(float startX, float startY) {
    x = startX;
    y = startY;
    speed = 5.0f; // Velocidad de desplazamiento
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