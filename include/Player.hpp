#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "raylib.h" // Necesario para Vector2 y funciones de teclado

class Player {
private:
    float x, y;         // Posición en la pantalla
    float speed;        // Velocidad de movimiento

public:
    // Constructor
    Player(float startX, float startY);

    // Método de actualización que maneja el teclado automáticamente
    void Update();

    // Métodos de movimiento individuales
    void moverIzquierda();
    void moverDerecha();
    void moverArriba();
    void moverAbajo();

    // Métodos para consultar la posición actual
    float getX() const;
    float getY() const;
    Vector2 getPosition() const; // ¡Añadido para el bot y colisiones!
};

#endif