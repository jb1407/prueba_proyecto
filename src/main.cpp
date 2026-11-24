#include "raylib.h"
#include "CombatSystem.hpp"

int main()
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Test CombatSystem"); // Debe ir antes de cargar texturas y antes de destruir AssetManager
    SetTargetFPS(60);

    // 1. Cargar assets en la tabla hash
    AssetManager assets;
    LoadAssetsToTable(assets); //[cite: 38]

    // 2. Generar moveset base apuntando a PlayerPosition
    PlayerPosition = { screenWidth * 0.4f, screenHeight * 0.5f }; //[cite: 39]
    MoveSetData moveSetData; //[cite: 33]
    MoveSetBase baseMoves = moveSetData.CharacterMoveSetDefault(assets, &PlayerPosition); //

    // 3. Crear jugador de prueba (sin armas equipadas por ahora)
    Player player1(
        100,            // Health[cite: 36, 40]
        0,              // Energy[cite: 36, 40]
        100,            // ShieldEnergy[cite: 36, 40]
        0,              // TimeStun[cite: 36, 40]
        nullptr,        // Arm1[cite: 36, 40]
        nullptr,        // Arm2[cite: 36, 40]
        false,          // IsAttack[cite: 36, 40]
        baseMoves       // MoveSetBase[cite: 36, 40]
    );

    // 4. Inicializar sistema de combate del jugador 1
    // (Player*, depthSwapStart, depthSwapEnd, flipX, rotation, moveEndOrReset, frameMoveEnd)[cite: 33, 37]
    Combat combatP1(&player1, 0, 0, false, 0.0f, false, 0);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Actualiza inputs, movimiento y dibuja el sprite activo del jugador 1
        combatP1.UpdateMoveBasePlayer1(); //[cite: 37]

        DrawText("WASD: Mover | I: Punch | O: BackWard", 20, 20, 20, RAYWHITE);
        EndDrawing();
    }

    // Importante: que assets se destruya antes de CloseWindow() para liberar texturas con el contexto activo[cite: 38]
    assets.~AssetManager();
    CloseWindow();

    return 0;
}