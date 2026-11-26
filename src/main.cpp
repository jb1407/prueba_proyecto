#include <cstdlib>
#include <ctime>
#include "raylib.h"
#include "CombatSystem.hpp"
#include "enemyAI.hpp"

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Prueba EnemyAI - Decision Tree");
    SetTargetFPS(60);

    AssetManager assets;
    // ... carga de assets y configuración del jugador ...

    Difficulty currentDiff = Difficulty::MEDIUM;
    auto enemy = std::make_unique<EnemyAI>(850.0f, 400.0f, assets, currentDiff); //[cite: 3]

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_ONE))
        {
            currentDiff = Difficulty::EASY;
            enemy = std::make_unique<EnemyAI>(enemy->GetPosition().x, enemy->GetPosition().y, assets, currentDiff);
        }
        else if (IsKeyPressed(KEY_TWO))
        {
            currentDiff = Difficulty::MEDIUM;
            enemy = std::make_unique<EnemyAI>(enemy->GetPosition().x, enemy->GetPosition().y, assets, currentDiff);
        }
        else if (IsKeyPressed(KEY_THREE))
        {
            currentDiff = Difficulty::HARD;
            enemy = std::make_unique<EnemyAI>(enemy->GetPosition().x, enemy->GetPosition().y, assets, currentDiff);
        }

        // ... actualización del jugador (playerPos, isPlayerAttacking) ...
        // enemy.Update(playerPos, isPlayerAttacking, screenWidth, screenHeight); //[cite: 3]

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // ... dibujar escenario, jugador y enemigo ...

        const char *diffLabel =
            (currentDiff == Difficulty::EASY) ? "EASY" : (currentDiff == Difficulty::MEDIUM) ? "MEDIUM"
                                                                                             : "HARD";

        DrawText(TextFormat("Dificultad [1/2/3]: %s", diffLabel), 20, 20, 20, BLACK);
        DrawText(TextFormat("Ocupado (Atk/Back): %s", enemy->IsBusy() ? "SI" : "NO"), 20, 46, 18, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}