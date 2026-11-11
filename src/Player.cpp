#include "Player.hpp"
#include <cmath>

Player::Player(float startX, float startY) : hurtbox(startX, startY, 40.0f, 40.0f, true),
    attackHitbox(startX, startY, 30.0f, 20.0f, false) {
    position = { startX, startY };
    size = { 40.0f, 40.0f };
    speed = 4.0f; 
    maxHealth = 100.0f;
    health = 100.0f;
    
    currentMove = Standing; // Iniciamos de pie
    flipX = false;          // Mirando hacia la derecha por defecto

    dodgeDir = {0.0f, 0.0f};
    dodgeSpeed = speed * 2.5f; // Velocidad de impulso superior a la caminata
}

void Player::Update(AssetManager& assetManager) {
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

    // 2. Captura de inputs y asignación de estados de movimiento
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
    else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        currentMove = Walk;
        flipX = false;
        position.x += speed;
    }
    else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        currentMove = Walk;
        flipX = true;
        position.x -= speed;
    }
    else if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
        currentMove = WalkUP;
        position.y -= speed;
    }
    else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
        currentMove = WalkDown;
        position.y += speed;
    }
    else {
        currentMove = Standing; // Si no se presiona nada, queda de pie
    }

    // 3. Solicitamos al CombatManager que ejecute y dibuje la animación correspondiente
    combatManager.RequestMove(currentMove, assetManager, position, flipX);

    hurtbox.UpdatePosition(position.x, position.y, 0.0f, 0.0f);

    if (currentMove == Punch) {
        attackHitbox.SetActive(true);
        // Si flipX es verdadero mira a la izquierda, ajustamos el offset del ataque
        float offsetX = flipX ? -30.0f : size.x;
        attackHitbox.UpdatePosition(position.x, position.y, offsetX, 10.0f);
    } else {
        attackHitbox.SetActive(false);
    }
}

void Player::TakeDamage(float amount) {
    health -= amount;
    if (health < 0.0f) {
        health = 0.0f;
    }
}