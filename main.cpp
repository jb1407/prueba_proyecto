#include <raylib.h>
#include "enemyAI.hpp"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Prueba 2D - Rango de Ataque y Movimiento en Stun");

    Vector2 playerPos = { 100.0f, 200.0f };
    Vector2 playerSize = { 50.0f, 50.0f };
    float playerSpeed = 4.0f;

    // Variables de recuperación (stun / parpadeo)
    bool isStunned = false;
    float stunTimer = 0.0f;
    const float stunDuration = 3.0f; 

    // Inicializar el bot
    EnemyAI enemy(650.0f, 200.0f, Difficulty::MEDIUM);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // 1. Actualizar temporizador de recuperación (el jugador sigue parpadeando pero YA PUEDE MOVERSE)
        if (isStunned) {
            stunTimer -= deltaTime;
            if (stunTimer <= 0.0f) {
                isStunned = false; 
            }
        }

        // 2. Controles del jugador (LIBRES, incluso durante la recuperación)
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))  playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPos.x += playerSpeed;
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))    playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))  playerPos.y += playerSpeed;

        // Limitar al jugador dentro de la pantalla
        if (playerPos.x < 0) playerPos.x = 0;
        if (playerPos.y < 0) playerPos.y = 0;
        if (playerPos.x + playerSize.x > screenWidth) playerPos.x = screenWidth - playerSize.x;
        if (playerPos.y + playerSize.y > screenHeight) playerPos.y = screenHeight - screenHeight;

        // Detectar si el jugador ataca
        bool isPlayerAttacking = IsKeyDown(KEY_SPACE);

        // 3. Actualizar la IA
        enemy.Update(playerPos, isPlayerAttacking, screenWidth, screenHeight);

        // --- COLISIÓN CON EL RANGO DE ATAQUE DEL BOT ---
        Rectangle playerRect = { playerPos.x, playerPos.y, playerSize.x, playerSize.y };
        Rectangle enemySpriteRect = enemy.GetRectangle();
        
        // Creamos un rectángulo ampliado que representa el "radio / área de ataque" del bot
        float attackRadius = enemy.GetAttackRange();
        Rectangle enemyAttackRangeRect = {
            enemySpriteRect.x - attackRadius,
            enemySpriteRect.y - attackRadius,
            enemySpriteRect.width + (attackRadius * 2),
            enemySpriteRect.height + (attackRadius * 2)
        };

        // Si el jugador entra en el radio de ataque del bot:
        if (CheckCollisionRecs(playerRect, enemyAttackRangeRect)) {
            // A. Rebote físico suave en el perímetro del rango para mantener distancia
            if (playerPos.x < enemySpriteRect.x) {
                playerPos.x -= 2.0f;
            } else {
                playerPos.x += 2.0f;
            }

            // B. Si el bot está atacando activamente y el jugador no estaba en cooldown -> Daño y Knockback
            AIAction botAction = enemy.GetCurrentAction();
            if ((botAction == AIAction::ATTACK_LIGHT || botAction == AIAction::ATTACK_HEAVY) && !isStunned) {
                isStunned = true;
                stunTimer = stunDuration; // Activa el titileo por 3 segundos

                // Retroceso (Knockback) al ser alcanzado por el ataque
                if (playerPos.x < enemySpriteRect.x) {
                    playerPos.x -= 40.0f;
                } else {
                    playerPos.x += 40.0f;
                }
            }
        }

        // --- DIBUJADO ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Usa W, A, S, D para moverte. ESPACIO para atacar.", 20, 20, 14, DARKGRAY);
        if (isStunned) {
            DrawText("¡RECUPERÁNDOSE! (Titilando pero con movilidad)", 20, 45, 14, RED);
        }

        // Renderizado del Jugador con efecto de parpadeo (titileo) durante la recuperación
        Color playerColor = BLUE;
        if (isStunned) {
            int flashState = (int)(GetTime() * 15); 
            if (flashState % 2 == 0) {
                playerColor = Fade(BLUE, 0.2f); 
            }
        }

        DrawRectangleV(playerPos, playerSize, playerColor);
        DrawText("Jugador", playerPos.x - 5, playerPos.y - 20, 10, DARKGRAY);

        // Dibujar al bot (limpio, sin texto flotante encima)
        enemy.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}