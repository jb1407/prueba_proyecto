#ifndef PLAYER_HPP
#ifndef PLAYER_HPP
#define PLAYER_HPP

class Player {
private:
    float x, y;          // Posición en la pantalla
    float speed;         // Velocidad de movimiento

public:
    // Constructor
    Player(float startX, float startY);

    // Métodos para el movimiento con flechas
    void moverIzquierda();
    void moverDerecha();
    void moverArriba();
    void moverAbajo();

    // Métodos para consultar la posición actual
    float getX() const;
    float getY() const;
};

#endif