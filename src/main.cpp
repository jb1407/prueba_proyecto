#include <cstdlib>
#include <ctime>
#include <memory>
#include "raylib.h"
#include "CombatSystem.hpp"
#include "enemyAI.hpp"
#include "hitbox.hpp"

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

    InitWindow(screenWidth, screenHeight, "Prueba EnemyAI - Neon Sword Red");
    SetTargetFPS(60);

    // 1. Cargar assets (capacidad amplia para evitar colisiones excesivas en la tabla hash)
    AssetManager assetManager(101);
    LoadAssetsToTable(assetManager);

    float playerStartX = screenWidth * 0.22f;
    float playerStartY = (screenHeight * 0.5f) - 100.0f;

    float enemyStartX = screenWidth * 0.68f;
    float enemyStartY = (screenHeight * 0.5f) - 100.0f;

    PlayerPosition = { playerStartX, playerStartY };
    PlayerOffSet1  = PlayerPosition;
    PlayerOffSet2  = PlayerPosition;

    // 2. Construir MoveSets (base + Neon Sword Red con offsets del jugador)
    MoveSetData moveSetData;
    MoveSetBase playerMoveBase = moveSetData.CharacterMoveSetDefault(assetManager, &PlayerPosition);
    MoveSetWeapons nsrMoves = moveSetData.NeonSwordRedMoves(assetManager, &PlayerPosition, &PlayerOffSet1, &PlayerOffSet2);

    Weapon neonSwordRed("NeonSwordRed", nsrMoves, 0, true);

    Player player(100, 0, 100, 0, &neonSwordRed, nullptr, false, playerMoveBase);
    player.SetActualWeapon(Weapon1);

    Combat playerCombat(&player, 0, 0, false, 0.0f, false, 0);

    Difficulty currentDiff = Difficulty::MEDIUM;
    auto enemy = std::make_unique<EnemyAI>(enemyStartX, enemyStartY, assetManager, currentDiff);

    // Hitboxes / Hurtboxes de prueba
    Hitbox playerHurtbox(PlayerPosition.x, PlayerPosition.y, 115.0f, 200.0f, true);
    Hitbox enemyHurtbox(enemy->GetPosition().x, enemy->GetPosition().y, 115.0f, 200.0f, true);

    Hitbox playerPunchHitbox(0.0f, 0.0f, 55.0f, 45.0f, false);
    Hitbox enemyAttackHitbox(0.0f, 0.0f, 55.0f, 45.0f, false);

    float punchActiveTimer = 0.0f;
    bool playerPunchHitApplied = false;
    bool enemyAttackHitApplied = false;
    bool wasEnemyAttacking = false;
    float backwardCrouchTimer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

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

        // Detectar inicio de Punch (sin tocar CombatSystem.hpp)
        if (IsKeyPressed(KEY_I) && !player.GetIsAttack()) {
            punchActiveTimer = 0.22f;
            playerPunchHitApplied = false;
        }
        if (punchActiveTimer > 0.0f) {
            punchActiveTimer -= dt;
            if (punchActiveTimer < 0.0f) punchActiveTimer = 0.0f;
        }

        // Detectar Backward (deslizado con O) para reducir la hurtbox a la mitad
        if (IsKeyPressed(KEY_O) && !player.GetIsAttack()) {
            backwardCrouchTimer = 0.28f;
        }
        if (backwardCrouchTimer > 0.0f) {
            backwardCrouchTimer -= dt;
            if (backwardCrouchTimer < 0.0f) backwardCrouchTimer = 0.0f;
        }

        // Mantener al jugador dentro de los límites de pantalla
        if (PlayerPosition.x < 0.0f) PlayerPosition.x = 0.0f;
        if (PlayerPosition.y < 0.0f) PlayerPosition.y = 0.0f;
        if (PlayerPosition.x > (float)screenWidth - 115.0f) PlayerPosition.x = (float)screenWidth - 115.0f;
        if (PlayerPosition.y > (float)screenHeight - 200.0f) PlayerPosition.y = (float)screenHeight - 200.0f;

        // Mantener offsets alineados cuando no se está ejecutando un ataque multicapa
        if (!player.GetIsAttack()) {
            PlayerOffSet1 = PlayerPosition;
            PlayerOffSet2 = PlayerPosition;
            playerCombat.SetFlipX(enemy->GetPosition().x < PlayerPosition.x);
        }

        Vector2 playerCenter = {
            PlayerPosition.x + 57.5f,
            PlayerPosition.y + 100.0f
        };

        BeginDrawing();
        ClearBackground((Color){ 110, 116, 130, 255 });

        // Actualiza y dibuja el moveset NSR (con fallback automático a movimiento base)
        playerCombat.UpdateMoveNSRPlayer1();

        // Actualiza y dibuja al enemigo
        enemy->Update(playerCenter, player.GetIsAttack(), screenWidth, screenHeight);

        // 1. Actualizar hurtboxes base (reduciendo a la mitad durante retroceso/deslizado)
        bool isCrouchingBackward = (backwardCrouchTimer > 0.0f);
        float playerHurtboxHeight = isCrouchingBackward ? 100.0f : 200.0f;
        float playerHurtboxOffsetY = isCrouchingBackward ? 100.0f : 0.0f;
        playerHurtbox = Hitbox(PlayerPosition.x, PlayerPosition.y + playerHurtboxOffsetY, 115.0f, playerHurtboxHeight, true);

        bool isEnemyRetreating = (enemy->GetCurrentAction() == AIAction::RETREAT);
        float enemyHurtboxHeight = isEnemyRetreating ? 100.0f : 200.0f;
        float enemyHurtboxOffsetY = isEnemyRetreating ? 100.0f : 0.0f;
        enemyHurtbox = Hitbox(enemy->GetPosition().x, enemy->GetPosition().y + enemyHurtboxOffsetY, 115.0f, enemyHurtboxHeight, true);

        // 2. Hitbox temporal de Punch del jugador
        bool punchWindowActive = (punchActiveTimer > 0.0f && punchActiveTimer < 0.18f);
        playerPunchHitbox.SetActive(punchWindowActive);
        if (punchWindowActive) {
            float hitX = playerCombat.GetFlipX()
                ? (PlayerPosition.x - playerPunchHitbox.GetRect().width - 5.0f)
                : (PlayerPosition.x + 115.0f + 5.0f);
            float hitY = PlayerPosition.y + 55.0f;
            playerPunchHitbox.UpdatePosition(hitX, hitY, 0.0f, 0.0f);

            if (!playerPunchHitApplied && playerPunchHitbox.CheckCollision(enemyHurtbox.GetRect())) {
                enemy->TakeDamage(10.0f);
                playerPunchHitApplied = true;
            }
        } else if (punchActiveTimer == 0.0f) {
            playerPunchHitApplied = false;
        }

        // 3. Hitbox de ataque del enemigo (ATTACK_LIGHT / ATTACK_HEAVY)
        AIAction enemyAct = enemy->GetCurrentAction();
        bool isEnemyAttacking = (enemyAct == AIAction::ATTACK_LIGHT || enemyAct == AIAction::ATTACK_HEAVY);

        if (isEnemyAttacking && !wasEnemyAttacking) {
            enemyAttackHitApplied = false;
        }
        wasEnemyAttacking = isEnemyAttacking;

        enemyAttackHitbox.SetActive(isEnemyAttacking);
        if (isEnemyAttacking) {
            bool enemyFacesLeft = (enemy->GetPosition().x > PlayerPosition.x);
            float eHitX = enemyFacesLeft
                ? (enemy->GetPosition().x - enemyAttackHitbox.GetRect().width - 5.0f)
                : (enemy->GetPosition().x + 115.0f + 5.0f);
            float eHitY = enemy->GetPosition().y + 55.0f;
            enemyAttackHitbox.UpdatePosition(eHitX, eHitY, 0.0f, 0.0f);

            if (!enemyAttackHitApplied && enemyAttackHitbox.CheckCollision(playerHurtbox.GetRect())) {
                int dmg = (enemyAct == AIAction::ATTACK_HEAVY) ? 12 : 7;
                if (player.GetHealth() > dmg) player.SetHealth(player.GetHealth() - dmg);
                else player.SetHealth(0);
                enemyAttackHitApplied = true;
            }
        } else {
            enemyAttackHitApplied = false;
        }

        // 4. Dibujar cajas para calibrar en pantalla
        playerHurtbox.Draw(GREEN);
        enemyHurtbox.Draw(SKYBLUE);
        playerPunchHitbox.Draw(RED);
        enemyAttackHitbox.Draw(ORANGE);

        const char* diffLabel =
            (currentDiff == Difficulty::EASY) ? "EASY" :
            (currentDiff == Difficulty::MEDIUM) ? "MEDIUM" : "HARD";

        DrawText(TextFormat("Dificultad [1: EASY | 2: MEDIUM | 3: HARD]: %s", diffLabel), 20, 20, 22, YELLOW);
        DrawText(TextFormat("HP Jugador: %d | HP Enemigo: %.0f | Accion CPU: %s",
                 player.GetHealth(), enemy->GetHealth(), ActionToString(enemy->GetCurrentAction())),
                 20, 50, 20, RAYWHITE);
        DrawText(TextFormat("Jugador Pos: (%.0f, %.0f) | Enemigo Pos: (%.0f, %.0f)",
                 PlayerPosition.x, PlayerPosition.y,
                 enemy->GetPosition().x, enemy->GetPosition().y),
                 20, 76, 18, LIGHTGRAY);
        DrawText("WASD: Mover | I: Punch | O: Backward | U: Thrust -> U: RotaryCut", 20, 102, 18, LIGHTGRAY);
        DrawText("D,S,D+U: CrimsonOrbit | D,A,D+U: NeonBurst (invierte si miras izq)", 20, 125, 18, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}