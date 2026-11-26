#include "raylib.h"
#include "CombatSystem.hpp"
#include "enemyAI.hpp"

namespace {
const char* ActionToString(AIAction action) {
    switch (action) {
        case AIAction::IDLE:          return "IDLE";
        case AIAction::APPROACH:      return "APPROACH";
        case AIAction::DASH_FORWARD:  return "DASH_FORWARD";
        case AIAction::RETREAT:       return "RETREAT";
        case AIAction::ATTACK_LIGHT:  return "ATTACK_LIGHT";
        case AIAction::ATTACK_HEAVY:  return "ATTACK_HEAVY";
        case AIAction::BLOCK:         return "BLOCK";
        default:                      return "UNKNOWN";
    }
}
} // namespace

int main()
{
    const int screenWidth = 1600;
    const int screenHeight = 900;

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

    // 5. Crear enemigo de prueba (inicializa CPUPosition, su MoveSetBase y su árbol de decisiones)
    EnemyAI enemy(screenWidth * 0.65f, screenHeight * 0.5f, assets);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Jugador 1
        combatP1.UpdateMoveBasePlayer1();

        // Enemigo autónomo evaluando el árbol respecto a PlayerPosition
        // Si tu clase Player expone un getter o campo de ataque, cámbialo aquí (p. ej. player1.GetIsAttack())
        bool isP1Attacking = false;
        enemy.Update(PlayerPosition, isP1Attacking, screenWidth, screenHeight);

        DrawText("P1: WASD, I (Punch), O (BackWard) | CPU: Autonomo (Arbol de Decisiones)", 20, 20, 20, RAYWHITE);
        DrawText(TextFormat("CPU Action: %s", ActionToString(enemy.GetCurrentAction())), 20, 50, 20, YELLOW);
        EndDrawing();
    }

    // Importante: que assets se destruya antes de CloseWindow() para liberar texturas con el contexto activo
    assets.~AssetManager();
    CloseWindow();

    return 0;
}