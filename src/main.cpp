#include <cstdlib>
#include <ctime>
#include <memory>
#include "raylib.h"
#include "LoadAssets.hpp"
#include "CombatSystem.hpp"
#include "enemyAI.hpp"

// Si PlayerPosition ya está definida globalmente en otro .cpp o en Mechanics.hpp,
// deja solo la declaración extern o quita esta definición si da "multiple definition".

namespace {
const char* ActionToString(AIAction action) {
    switch (action) {
        case AIAction::IDLE:         return "IDLE";
        case AIAction::APPROACH:     return "APPROACH";
        case AIAction::DASH_FORWARD: return "DASH_FORWARD";
        case AIAction::RETREAT:      return "RETREAT";
        case AIAction::ATTACK_LIGHT: return "ATTACK_LIGHT";
        case AIAction::ATTACK_HEAVY: return "ATTACK_HEAVY";
        case AIAction::BLOCK:        return "BLOCK";
        default:                     return "UNKNOWN";
    }
}
} // namespace

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Prueba EnemyAI - Jugador CombatSystem");
    SetTargetFPS(60);

    AssetManager assetManager(15);
    LoadAssetsToTable(assetManager);

    float playerStartX = screenWidth * 0.22f;
    float playerStartY = (screenHeight * 0.5f) - 100.0f;

    float enemyStartX = screenWidth * 0.68f;
    float enemyStartY = (screenHeight * 0.5f) - 100.0f;

    PlayerPosition = { playerStartX, playerStartY };

    MoveSetData moveSetData;
    MoveSetBase playerMoveBase = moveSetData.CharacterMoveSetDefault(assetManager, &PlayerPosition);

    Player player(100, 0, 100, 0, nullptr, nullptr, false, playerMoveBase);
    Combat playerCombat(&player, 0, 0, false, 0.0f, false, 0);

    Difficulty currentDiff = Difficulty::MEDIUM;
    auto enemy = std::make_unique<EnemyAI>(enemyStartX, enemyStartY, assetManager, currentDiff);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) {
            currentDiff = Difficulty::EASY;
            enemy = std::make_unique<EnemyAI>(enemy->GetPosition().x, enemy->GetPosition().y, assetManager, currentDiff);
        } else if (IsKeyPressed(KEY_TWO)) {
            currentDiff = Difficulty::MEDIUM;
            enemy = std::make_unique<EnemyAI>(enemy->GetPosition().x, enemy->GetPosition().y, assetManager, currentDiff);
        } else if (IsKeyPressed(KEY_THREE)) {
            currentDiff = Difficulty::HARD;
            enemy = std::make_unique<EnemyAI>(enemy->GetPosition().x, enemy->GetPosition().y, assetManager, currentDiff);
        }

        // Mantener al jugador dentro de la pantalla y orientarlo respecto al enemigo
        if (PlayerPosition.x < 0.0f) PlayerPosition.x = 0.0f;
        if (PlayerPosition.y < 0.0f) PlayerPosition.y = 0.0f;
        if (PlayerPosition.x > (float)screenWidth - 115.0f) PlayerPosition.x = (float)screenWidth - 115.0f;
        if (PlayerPosition.y > (float)screenHeight - 200.0f) PlayerPosition.y = (float)screenHeight - 200.0f;

        if (!player.GetIsAttack()) {
            playerCombat.SetFlipX(enemy->GetPosition().x < PlayerPosition.x);
        }

        Vector2 playerCenter = {
            PlayerPosition.x + 57.5f,
            PlayerPosition.y + 100.0f
        };

        BeginDrawing();
        ClearBackground((Color){ 28, 30, 36, 255 });

        // Actualiza y dibuja el movimiento/animación del jugador vía CombatSystem
        playerCombat.UpdateMoveBasePlayer1();

        // Actualiza y dibuja al enemigo
        enemy->Update(playerCenter, player.GetIsAttack(), screenWidth, screenHeight);

        const char* diffLabel =
            (currentDiff == Difficulty::EASY) ? "EASY" :
            (currentDiff == Difficulty::MEDIUM) ? "MEDIUM" : "HARD";

        DrawText(TextFormat("Dificultad [1: EASY | 2: MEDIUM | 3: HARD]: %s", diffLabel), 20, 20, 22, YELLOW);
        DrawText(TextFormat("Jugador Pos: (%.0f, %.0f) | Enemigo Pos: (%.0f, %.0f) | Accion: %s",
                 PlayerPosition.x, PlayerPosition.y,
                 enemy->GetPosition().x, enemy->GetPosition().y,
                 ActionToString(enemy->GetCurrentAction())),
                 20, 50, 20, RAYWHITE);
        DrawText("WASD: Mover jugador | I: Punch | O: Backward", 20, 80, 18, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}