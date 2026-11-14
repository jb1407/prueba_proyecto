#include "Player.hpp"
#include <cmath>

Player::Player(float startX, float startY) : hurtbox(startX, startY, 115.0f, 200.0f, true),
    attackHitbox(startX, startY, 45.0f, 30.0f, false) {
    position = { startX, startY };
    size = { 55.0f, 100.0f };
    speed = 4.0f; 
    maxHealth = 100.0f;
    health = 100.0f;
    
    currentMove = Standing; // Iniciamos de pie
    flipX = false;          // Mirando hacia la derecha por defecto

    dodgeDir = {0.0f, 0.0f};
    dodgeSpeed = speed * 2.5f; // Velocidad de impulso superior a la caminata

    // Inicializar variables de invencibilidad
    invincibilityTimer = 0.0f;
    isInvincible = false;
}

void Player::Update(AssetManager& assetManager) {
    if (isInvincible) {
        invincibilityTimer -= GetFrameTime();
        if (invincibilityTimer <= 0.0f) {
            isInvincible = false; // Termina el periodo de inmunidad
        }
    }

    // 1. Si hay una acción bloqueada (ej. un golpe o el Backward en curso),
    // mantenemos el impulso del esquive y dejamos que el CombatManager controle la animación.
    if (IsActionLocked) {
        if (currentMove == Backward) {
            position.x += dodgeDir.x * dodgeSpeed;
            position.y += dodgeDir.y * dodgeSpeed;
        }
        combatManager.RequestMove(currentMove, assetManager, position, flipX);
        return;
    }

    // 2. Captura de inputs y asignación de estados de movimiento con soporte diagonal
    if (IsKeyPressed(KEY_U)) {
        currentMove = Backward;
        IsActionLocked = true;
        MoveSetBaseLocked = currentMove;

        // Capturar la dirección del teclado al momento de activar el esquive
        dodgeDir = {0.0f, 0.0f};
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { dodgeDir.x += 1.0f; flipX = false; }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { dodgeDir.x -= 1.0f; flipX = true; }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) { dodgeDir.y -= 1.0f; }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) { dodgeDir.y += 1.0f; }

        // Si no se presiona ninguna tecla de dirección, se esquiva hacia atrás según la orientación actual (flipX)
        if (dodgeDir.x == 0.0f && dodgeDir.y == 0.0f) {
            dodgeDir.x = flipX ? 1.0f : -1.0f;
        } else {
            // Normalizar el vector para mantener velocidad constante en diagonales
            float length = std::sqrt(dodgeDir.x * dodgeDir.x + dodgeDir.y * dodgeDir.y);
            if (length > 0.0f) {
                dodgeDir.x /= length;
                dodgeDir.y /= length;
            }
        }

        combatManager.RequestMove(currentMove, assetManager, position, flipX);
        return;
    }
    else if (IsKeyPressed(KEY_SPACE)) {
        currentMove = Punch; // Dispara el golpe (activa IsActionLocked automáticamente)
    }
    else {
        // Lógica de movimiento libre en ejes independientes (permite diagonales)
        Vector2 direction = { 0.0f, 0.0f };

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  { direction.x = -1.0f; }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { direction.x =  1.0f; }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    { direction.y = -1.0f; }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  { direction.y =  1.0f; }

        // Si hay intención de movimiento
        if (direction.x != 0.0f || direction.y != 0.0f) {
            // Normalizar para evitar velocidad excesiva en diagonales
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length > 0.0f) {
                direction.x /= length;
                direction.y /= length;
            }

            position.x += direction.x * speed;
            position.y += direction.y * speed;

            // Selección de animación según tu regla:
            if (direction.x != 0.0f) {
                // Movimiento horizontal o en diagonal (prioriza animación horizontal y flipX)
                currentMove = Walk;
                flipX = (direction.x < 0.0f);
            } else {
                // Movimiento estrictamente vertical
                if (direction.y < 0.0f) {
                    currentMove = WalkUP;
                } else {
                    currentMove = WalkDown;
                }
            }
        } else {
            currentMove = Standing; // Si no se presiona nada, queda de pie
        }
    }

    bool shouldDraw = true;
    if (isInvincible) {
        int flicker = (int)(invincibilityTimer * 15); // Controla la velocidad del parpadeo
        if (flicker % 2 == 0) {
            shouldDraw = false; // Salta este frame para crear el efecto de titileo
        }
    }

    // Solo dibujamos si el filtro de invencibilidad lo permite
    if (shouldDraw) {
        combatManager.RequestMove(currentMove, assetManager, position, flipX);
    }

    hurtbox.UpdatePosition(position.x, position.y, 0.0f, 0.0f);

    if (currentMove == Punch) {
        attackHitbox.SetActive(true);
        
        // Supongamos que el ancho de tu hitbox de ataque es de 60.0f
        float attackWidth = 60.0f; 
        
        // Añadimos un pequeño margen de separación extra para garantizar que salga totalmente fuera de la caja azul del cuerpo.
        float offsetX = flipX ? (-attackWidth - 10.0f) : (size.x + 55.0f);
        
        // Mantener la altura a la altura del pecho/brazos
        float offsetY = size.y * 0.6f; 
        
        attackHitbox.UpdatePosition(position.x, position.y, offsetX, offsetY);
    } else {
        attackHitbox.SetActive(false);
    }
}

void Player::TakeDamage(float amount) {
    if (isInvincible) {
        return; // Si está en periodo de inmunidad, ignoramos el golpe
    }

    health -= amount;
    if (health < 0.0f) {
        health = 0.0f;
    }

    // Activamos la invencibilidad por 3 segundos
    isInvincible = true;
    invincibilityTimer = 3.0f; 
}