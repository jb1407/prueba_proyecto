#include "raylib.h"
#include "CombatSystem.hpp"
#include "enemyAI.hpp"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Test CombatSystem + EnemyAI"); // Debe ir antes de cargar texturas y antes de destruir AssetManager
    SetTargetFPS(60);

    // 1. Cargar assets en la tabla hash
    AssetManager assets;
    LoadAssetsToTable(assets);

    // 2. Generar moveset base del jugador apuntando a PlayerPosition
    PlayerPosition = { screenWidth * 0.35f, screenHeight * 0.5f };
    MoveSetData moveSetData;
    MoveSetBase baseMoves = moveSetData.CharacterMoveSetDefault(assets, &PlayerPosition);

    // 3. Crear jugador de prueba (sin armas equipadas por ahora)
    Player player1(
        100,            // Health
        0,              // Energy
        100,            // ShieldEnergy
        0,              // TimeStun
        nullptr,        // Arm1
        nullptr,        // Arm2
        false,          // IsAttack
        baseMoves       // MoveSetBase
    );

    // 4. Inicializar sistema de combate del jugador 1
    // (Player*, depthSwapStart, depthSwapEnd, flipX, rotation, moveEndOrReset, frameMoveEnd)
    Combat combatP1(&player1, 0, 0, false, 0.0f, false, 0);

    // 5. Crear enemigo de prueba (inicializa CPUPosition y su MoveSetBase)
    EnemyAI enemy(screenWidth * 0.65f, screenHeight * 0.5f, assets);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Jugador 1
        combatP1.UpdateMoveBasePlayer1();

        // Prueba manual de movimientos básicos del enemigo
        if (enemy.IsBusy())
        {
            enemy.UpdateFrame(screenWidth, screenHeight);
        }
        else if (IsKeyPressed(KEY_K))
        {
            enemy.AttackPunch(screenWidth, screenHeight);
        }
        else if (IsKeyPressed(KEY_L))
        {
            float dirX = 0.0f;
            float dirY = 0.0f;

            if (IsKeyDown(KEY_RIGHT)) dirX = 1.0f;
            else if (IsKeyDown(KEY_LEFT)) dirX = -1.0f;

            if (IsKeyDown(KEY_UP)) dirY = -1.0f;
            else if (IsKeyDown(KEY_DOWN)) dirY = 1.0f;

            // Si no hay ninguna dirección presionada, retrocede hacia la izquierda por defecto
            if (dirX == 0.0f && dirY == 0.0f)
            {
                dirX = -1.0f;
            }

            enemy.DashBackward(dirX, dirY, screenWidth, screenHeight);
        }
        else if (IsKeyDown(KEY_UP))
        {
            enemy.MoveUp(screenWidth, screenHeight);
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            enemy.MoveDown(screenWidth, screenHeight);
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            enemy.MoveLeft(screenWidth, screenHeight);
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            enemy.MoveRight(screenWidth, screenHeight);
        }
        else
        {
            enemy.StandIdle(screenWidth, screenHeight);
        }

        DrawText("P1: WASD, I (Punch), O (BackWard) | CPU: Flechas, K (Punch), L (BackWard)", 20, 20, 20, RAYWHITE);
        EndDrawing();
    }

    // Importante: que assets se destruya antes de CloseWindow() para liberar texturas con el contexto activo
    assets.~AssetManager();
    CloseWindow();

    return 0;
}