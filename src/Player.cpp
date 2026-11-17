#include "Player.hpp"
#include <cmath>

Player::Player(float startX, float startY) : hurtbox(startX, startY, 115.0f, 200.0f, true),
    attackHitbox(startX, startY, 45.0f, 30.0f, false) {
    position = { startX, startY };
    size = { 115.0f, 200.0f };
    speed = 4.0f; 
    maxHealth = 100.0f;
    health = 100.0f;
    
    currentMove = Standing;
    flipX = false;

    dodgeDir = {0.0f, 0.0f};
    dodgeSpeed = speed * 3.0f;
    dodgeTimer = 0.0f; // <-- Inicializado en 0

    invincibilityTimer = 0.0f;
    isInvincible = false;
    hasHitEnemy = false;
}

void Player::ClampToScreen(int screenWidth, int screenHeight) {
    if (position.x < 0) position.x = 0;
    if (position.y < 0) position.y = 0;
    // --- LÍNEA MODIFICADA PARA EL LADO DERECHO ---
    float margenDerecho = 50.0f; // Puedes subir este número (ej. 70 o 90) si todavía se sale un poco, o bajarlo si frena muy temprano
    if (position.x + size.x + margenDerecho > screenWidth) {
        position.x = screenWidth - size.x - margenDerecho;
    }
    
    float alturaSuelo = 120.0f; // Ajusta este valor probando si sube o baja el piso visualmente
    if (position.y + size.y > screenHeight - alturaSuelo) {
        position.y = screenHeight - alturaSuelo - size.y;
    }
}

void Player::Update(AssetManager& assetManager, int screenWidth, int screenHeight) {
    float dt = GetFrameTime();

    if (isInvincible) {
        invincibilityTimer -= dt;
        if (invincibilityTimer <= 0.0f) {
            isInvincible = false;
        }
    }

    // 1. Control estricto del temporizador del Backward
    if (dodgeTimer > 0.0f) {
        dodgeTimer -= dt;
        position.x += dodgeDir.x * dodgeSpeed;
        position.y += dodgeDir.y * dodgeSpeed;

        // Cuando se acaba el tiempo del impulso, liberamos el bloqueo sí o sí
        if (dodgeTimer <= 0.0f) {
            dodgeTimer = 0.0f;
            IsActionLocked = false;
            currentMove = Standing;
        }
    }
    // 2. Si hay otra acción bloqueada (ej. Punch)
    else if (IsActionLocked) {
        // Por seguridad, si por algún motivo quedó atrapado en Backward sin timer, lo liberamos
        if (currentMove == Backward) {
            IsActionLocked = false;
            currentMove = Standing;
        }
    }
    // 3. Captura de inputs libres (solo cuando no hay acciones bloqueadas)
    else {
        if (IsKeyPressed(KEY_U)) {
            currentMove = Backward;
            IsActionLocked = true;
            MoveSetBaseLocked = currentMove;
            dodgeTimer = 0.40f; // Duración exacta del deslizamiento (ajusta a 0.20f o 0.30f si deseas)

            // Capturar dirección inicial del toque
            dodgeDir = {0.0f, 0.0f};
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { dodgeDir.x += 1.0f; flipX = false; }
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { dodgeDir.x -= 1.0f; flipX = true; }
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) { dodgeDir.y -= 1.0f; }
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) { dodgeDir.y += 1.0f; }

            if (dodgeDir.x == 0.0f && dodgeDir.y == 0.0f) {
                dodgeDir.x = flipX ? 1.0f : -1.0f;
            } else {
                float length = std::sqrt(dodgeDir.x * dodgeDir.x + dodgeDir.y * dodgeDir.y);
                if (length > 0.0f) {
                    dodgeDir.x /= length;
                    dodgeDir.y /= length;
                }
            }
        }
        else if (IsKeyPressed(KEY_SPACE)) {
            currentMove = Punch;
            hasHitEnemy = false;
        }
        else {
            Vector2 direction = { 0.0f, 0.0f };

            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  { direction.x = -1.0f; }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { direction.x =  1.0f; }
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    { direction.y = -1.0f; }
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  { direction.y =  1.0f; }

            if (direction.x != 0.0f || direction.y != 0.0f) {
                float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (length > 0.0f) {
                    direction.x /= length;
                    direction.y /= length;
                }

                position.x += direction.x * speed;
                position.y += direction.y * speed;

                if (direction.x != 0.0f) {
                    currentMove = Walk;
                    flipX = (direction.x < 0.0f);
                } else {
                    currentMove = (direction.y < 0.0f) ? WalkUP : WalkDown;
                }
            } else {
                currentMove = Standing;
            }
        }
    }

    // 4. LÍMITES DE PANTALLA Y HITBOXES SIEMPRE ACTIVOS (Sin returns prematuros)
    ClampToScreen(screenWidth, screenHeight);
    float hurtboxWidth = size.x;
    float hurtboxHeight = size.y;
    float hurtboxOffsetX = 0.0f;
    float hurtboxOffsetY = 0.0f;

    // Si está ejecutando el deslizamiento/agachado (Backward), la caja se hace más baja
    if (currentMove == Backward) {
        hurtboxHeight = size.y * 0.55f; // Reduce la altura a algo más de la mitad
        hurtboxOffsetY = size.y * 0.45f; // Desplaza la caja hacia abajo para cubrir el cuerpo deslizándose
    }

    if (flipX) {
        hurtboxOffsetX = -size.x - 30.0f; 
    }

    // Actualizamos tamaño y posición de la hurtbox del jugador
    hurtbox.SetSize(hurtboxWidth, hurtboxHeight);
    hurtbox.UpdatePosition(position.x, position.y, 0.0f, hurtboxOffsetY);

    // Actualización de la hitbox de ataque (Punch) se mantiene igual
    if (currentMove == Punch) {
        attackHitbox.SetActive(true);
        float attackWidth = 60.0f; 
        float offsetX = flipX ? (-attackWidth - 10.0f) : (size.x + 55.0f);
        float offsetY = size.y * 0.6f; 
        attackHitbox.UpdatePosition(position.x, position.y, offsetX, offsetY);
    } else {
        attackHitbox.SetActive(false);
    }

    // 5. Renderizado y parpadeo de invencibilidad
    bool shouldDraw = true;
    if (isInvincible) {
        int flicker = (int)(invincibilityTimer * 15);
        if (flicker % 2 == 0) {
            shouldDraw = false;
        }
    }

    if (shouldDraw) {
        combatManager.RequestMove(currentMove, assetManager, position, flipX);
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