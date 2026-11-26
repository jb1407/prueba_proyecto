#include <cstdlib>
#include <ctime>
#include <memory>
#include "raylib.h"
#include "LoadAssets.hpp"
#include "enemyAI.hpp"

enum class DummyPlayerState {
    IDLE,
    MOVE,
    PUNCH,
    BACKWARD
};

struct DummyPlayer {
    Vector2 pos;
    Vector2 size = { 115.0f, 200.0f };
    float moveSpeed = 5.5f;
    float backwardSpeed = 9.0f;
    int facingDir = 1; // 1 = derecha, -1 = izquierda

    DummyPlayerState state = DummyPlayerState::IDLE;
    float stateTimer = 0.0f;
    int animFrame = 0;
    float animTick = 0.0f;

    void Update(float dt, int screenWidth, int screenHeight) {
        if (stateTimer > 0.0f) {
            stateTimer -= dt;
            if (state == DummyPlayerState::BACKWARD) {
                pos.x -= (float)facingDir * backwardSpeed;
            }
            if (stateTimer <= 0.0f) {
                state = DummyPlayerState::IDLE;
                stateTimer = 0.0f;
            }
        } else {
            bool moved = false;
            float dx = 0.0f;
            float dy = 0.0f;

            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  { dx -= moveSpeed; moved = true; }
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) { dx += moveSpeed; moved = true; }
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    { dy -= moveSpeed; moved = true; }
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  { dy += moveSpeed; moved = true; }

            if (IsKeyPressed(KEY_SPACE)) {
                state = DummyPlayerState::PUNCH;
                stateTimer = 0.25f;
                animFrame = 0;
                animTick = 0.0f;
            } else if (IsKeyPressed(KEY_U) || IsKeyPressed(KEY_LEFT_SHIFT)) {
                state = DummyPlayerState::BACKWARD;
                stateTimer = 0.22f;
                animFrame = 0;
                animTick = 0.0f;
            } else if (moved) {
                pos.x += dx;
                pos.y += dy;
                state = DummyPlayerState::MOVE;
            } else {
                state = DummyPlayerState::IDLE;
            }
        }

        if (pos.x < 0.0f) pos.x = 0.0f;
        if (pos.y < 0.0f) pos.y = 0.0f;
        if (pos.x > (float)screenWidth - size.x) pos.x = (float)screenWidth - size.x;
        if (pos.y > (float)screenHeight - size.y) pos.y = (float)screenHeight - size.y;

        animTick += dt;
        float frameInterval = (state == DummyPlayerState::PUNCH) ? 0.06f : 0.12f;
        if (animTick >= frameInterval) {
            animTick = 0.0f;
            animFrame = (animFrame + 1) % 4;
        }
    }

    void FaceToward(Vector2 enemyPos) {
        if (state == DummyPlayerState::IDLE || state == DummyPlayerState::MOVE) {
            facingDir = (enemyPos.x >= pos.x) ? 1 : -1;
        }
    }

    bool IsAttacking() const {
        return state == DummyPlayerState::PUNCH;
    }

    void Draw() const {
        Color bodyColor = SKYBLUE;
        const char* label = "IDLE";

        if (state == DummyPlayerState::MOVE) {
            bodyColor = BLUE;
            label = "MOVE";
        } else if (state == DummyPlayerState::PUNCH) {
            bodyColor = RED;
            label = "PUNCH";
        } else if (state == DummyPlayerState::BACKWARD) {
            bodyColor = GOLD;
            label = "BACKWARD";
        }

        float pulseY = (state == DummyPlayerState::MOVE || state == DummyPlayerState::BACKWARD)
            ? (float)(animFrame % 2) * -4.0f
            : 0.0f;

        Rectangle bodyRect = { pos.x, pos.y + pulseY, size.x, size.y };
        DrawRectangleRec(bodyRect, Fade(bodyColor, 0.35f));
        DrawRectangleLinesEx(bodyRect, 3.0f, bodyColor);

        if (state == DummyPlayerState::PUNCH) {
            float punchWidth = 55.0f;
            float punchHeight = 36.0f;
            float punchX = (facingDir == 1) ? (pos.x + size.x) : (pos.x - punchWidth);
            float punchY = pos.y + 55.0f;
            Rectangle fistRect = { punchX, punchY, punchWidth, punchHeight };
            DrawRectangleRec(fistRect, Fade(RED, 0.7f));
            DrawRectangleLinesEx(fistRect, 2.0f, MAROON);
        }

        DrawText(label, (int)pos.x, (int)pos.y - 24, 18, bodyColor);
    }
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Prueba EnemyAI - Jugador Generico");
    SetTargetFPS(60);

    AssetManager assetManager(15);
    LoadAssetsToTable(assetManager);

    float playerStartX = screenWidth * 0.22f;
    float playerStartY = (screenHeight * 0.5f) - 100.0f;

    float enemyStartX = screenWidth * 0.68f;
    float enemyStartY = (screenHeight * 0.5f) - 100.0f;

    DummyPlayer player;
    player.pos = { playerStartX, playerStartY };

    Difficulty currentDiff = Difficulty::HARD;
    auto enemy = std::make_unique<EnemyAI>(enemyStartX, enemyStartY, assetManager, currentDiff);

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

        player.FaceToward(enemy->GetPosition());
        player.Update(dt, screenWidth, screenHeight);

        Vector2 playerCenter = {
            player.pos.x + player.size.x * 0.5f,
            player.pos.y + player.size.y * 0.5f
        };

        BeginDrawing();
        ClearBackground((Color){ 28, 30, 36, 255 });

        player.Draw();
        enemy->Update(playerCenter, player.IsAttacking(), screenWidth, screenHeight);

        const char* diffLabel =
            (currentDiff == Difficulty::EASY) ? "EASY" :
            (currentDiff == Difficulty::MEDIUM) ? "MEDIUM" : "HARD";

        DrawText(TextFormat("Dificultad [1: EASY | 2: MEDIUM | 3: HARD]: %s", diffLabel), 20, 20, 22, YELLOW);
        DrawText(TextFormat("Enemigo Pos: (%.0f, %.0f) | Ocupado: %s",
                 enemy->GetPosition().x, enemy->GetPosition().y, enemy->IsBusy() ? "SI" : "NO"),
                 20, 50, 20, RAYWHITE);
        DrawText("WASD / Flechas: Mover jugador | SPACE: Punch | U o Shift: Backward", 20, 80, 18, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}