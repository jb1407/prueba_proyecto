#include "Mechanics.hpp"

enum class FacingDir {
    RIGHT,
    LEFT
};

struct TimedInput {
    char dir; // 'U', 'D', 'L', 'R', 'N'
    double timestamp;
};

static char GetCurrentDirToken() {
    bool up    = IsKeyDown(KEY_W);
    bool down  = IsKeyDown(KEY_S);
    bool left  = IsKeyDown(KEY_A);
    bool right = IsKeyDown(KEY_D);

    if (down && right) return 'F'; // Diagonal abajo-adelante (derecha)
    if (down && left)  return 'B'; // Diagonal abajo-atras (izquierda)
    if (down)          return 'D';
    if (right)         return 'R';
    if (left)          return 'L';
    if (up)            return 'U';
    return 'N';
}

static bool HasQuarterCircleForwardInput(const std::deque<TimedInput>& history, FacingDir facing, double now, double windowSec = 0.65) {
    // Secuencia esperada segun orientacion:
    // Derecha (adelante = R): R -> D (o diagonal hacia adelante) -> R
    // Izquierda (adelante = L): L -> D (o diagonal hacia adelante) -> L
    char fwd   = (facing == FacingDir::RIGHT) ? 'R' : 'L';
    char down  = 'D';
    char diagF = (facing == FacingDir::RIGHT) ? 'F' : 'B';

    int stage = 0; // 0: busca adelante, 1: busca abajo/diag, 2: busca adelante final
    for (const auto& item : history) {
        if ((now - item.timestamp) > windowSec) continue;

        if (stage == 0 && item.dir == fwd) {
            stage = 1;
        } else if (stage == 1 && (item.dir == down || item.dir == diagF)) {
            stage = 2;
        } else if (stage == 2 && item.dir == fwd) {
            return true;
        }
    }
    return false;
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Prueba Mechanics - Controles y Combos");
    SetTargetFPS(60);

    AssetManager assetManager(101);
    LoadAssetsToTable(assetManager);

    // Animaciones base
    Animation* idleAnim     = assetManager.Search("C_Standing");
    Animation* walkAnim     = assetManager.Search("C_Walk");
    Animation* walkUpAnim   = assetManager.Search("C_WalkUP");
    Animation* walkDownAnim = assetManager.Search("C_WalkDown");
    Animation* backAnim     = assetManager.Search("C_Backward");
    Animation* punchAnim    = assetManager.Search("C_Punch");

    // Animaciones de espada (Neon Sword Red)
    Animation* nsr1Char     = assetManager.Search("NSR_Normal1(Character)");
    Animation* nsr1Sword    = assetManager.Search("NSR_Normal1(Sword)");
    Animation* nsr2Char     = assetManager.Search("NSR_Normal2(Character)");
    Animation* nsr2Sword    = assetManager.Search("NSR_Normal2(Sword)");
    Animation* nsrSp1Char   = assetManager.Search("NSR_Special1(Character)");
    Animation* nsrSp1Sword  = assetManager.Search("NSR_Special1(Sword)");
    Animation* nsrSp1Circle = assetManager.Search("NSR_Special1(Circle)");

    // Posicion y offsets persistentes (Move almacena Vector2& por referencia)
    Vector2 pos = { 450.0f, 260.0f };
    Vector2 zeroOffset = { 0.0f, 0.0f };

    // Crear objetos Move[cite: 3, 6]
    Move idleMove("Idle", 0, idleAnim, nullptr, nullptr, pos, zeroOffset, zeroOffset);
    Move walkMove("Walk", 1, walkAnim, nullptr, nullptr, pos, zeroOffset, zeroOffset);
    Move walkUpMove("WalkUp", 1, walkUpAnim, nullptr, nullptr, pos, zeroOffset, zeroOffset);
    Move walkDownMove("WalkDown", 1, walkDownAnim, nullptr, nullptr, pos, zeroOffset, zeroOffset);
    Move backdashMove("Backdash", 2, backAnim, nullptr, nullptr, pos, zeroOffset, zeroOffset);
    Move punchMove("Punch", 3, punchAnim, nullptr, nullptr, pos, zeroOffset, zeroOffset);

    Move nsrNormal1("NSR_Normal1", 4, nsr1Char, nsr1Sword, nullptr, pos, pos, zeroOffset);
    Move nsrNormal2("NSR_Normal2", 5, nsr2Char, nsr2Sword, nullptr, pos, pos, zeroOffset);
    Move nsrSpecial1("NSR_Special1", 6, nsrSp1Char, nsrSp1Sword, nsrSp1Circle, pos, pos, pos);

    // Reglas de encadenamiento:
    // Normal 1 -> Normal 2
    nsrNormal1.AddAllowedFollowUp("NSR_Normal2");
    // Normal 2 -> Special 1
    nsrNormal2.AddAllowedFollowUp("NSR_Special1");
    // Special 1 -> Normal 2 (encadenable de nuevo)
    nsrSpecial1.AddAllowedFollowUp("NSR_Normal2");

    CombatManager combat(nullptr, &idleMove, 0);
    Move* lastCompletedMove = nullptr;

    FacingDir facing = FacingDir::RIGHT;
    float walkSpeed = 4.5f;
    float backdashTimer = 0.0f;
    float backdashDirX = -1.0f;
    float backdashSpeed = 11.0f;

    std::deque<TimedInput> inputHistory;
    char lastDirToken = 'N';
    std::string status = "WASD: Mover | I: Backdash | O: Golpe | U: Espada N1/N2 | Fwd-Down-Fwd+U: Especial 1";

    auto activateMove = [&](Move* targetMove, bool requireChain) {
        if (targetMove == nullptr) return false;
        combat.SetPrevMove(lastCompletedMove);
        combat.SetCurrentMove(targetMove);

        if (requireChain && !combat.ValidatePrevMove()) {
            combat.SetCurrentMove(lastCompletedMove ? lastCompletedMove : &idleMove);
            return false;
        }

        // Reiniciar frames de las capas del nuevo movimiento
        if (targetMove->GetAnimation1()) { targetMove->GetAnimation1()->currentFrame = 0; targetMove->GetAnimation1()->tickCounter = 0; }
        if (targetMove->GetAnimation2()) { targetMove->GetAnimation2()->currentFrame = 0; targetMove->GetAnimation2()->tickCounter = 0; }
        if (targetMove->GetAnimation3()) { targetMove->GetAnimation3()->currentFrame = 0; targetMove->GetAnimation3()->tickCounter = 0; }

        lastCompletedMove = targetMove;
        return true;
    };

    while (!WindowShouldClose()) {
        double now = GetTime();

        // Registrar cambios de direccion para el movimiento especial
        char dirToken = GetCurrentDirToken();
        if (dirToken != 'N' && dirToken != lastDirToken) {
            inputHistory.push_back({ dirToken, now });
            if (inputHistory.size() > 12) inputHistory.pop_front();
        }
        lastDirToken = dirToken;

        // Backdash activo (tecla I)
        if (IsKeyPressed(KEY_I) && backdashTimer <= 0.0f) {
            if (IsKeyDown(KEY_A)) {
                backdashDirX = -1.0f;
            } else if (IsKeyDown(KEY_D)) {
                backdashDirX = 1.0f;
            } else {
                backdashDirX = (facing == FacingDir::RIGHT) ? -1.0f : 1.0f;
            }
            backdashTimer = 0.22f;
            activateMove(&backdashMove, false);
            status = (backdashDirX < 0.0f) ? "Backdash hacia izquierda (I)" : "Backdash hacia derecha (I)";
        }

        if (backdashTimer > 0.0f) {
            backdashTimer -= GetFrameTime();
            pos.x += backdashDirX * backdashSpeed;
            if (backdashTimer <= 0.0f) {
                activateMove(&idleMove, false);
            }
        } else {
            // Ataque especial 1: Adelante -> Abajo -> Adelante + U (solo encadenable desde Normal 2)
            if (IsKeyPressed(KEY_U) && HasQuarterCircleForwardInput(inputHistory, facing, now)) {
                inputHistory.clear();
                if (activateMove(&nsrSpecial1, true)) {
                    status = "Especial 1 ejecutado (encadenable de nuevo a Normal 2 con U)";
                } else {
                    status = "Especial 1 bloqueado: requiere haber ejecutado Normal 2 antes";
                }
            }
            // Ataques de espada con U: Normal 1 si no viene de Normal 1/Especial 1, o Normal 2 si es encadenable
            else if (IsKeyPressed(KEY_U)) {
                if (lastCompletedMove == &nsrNormal1 || lastCompletedMove == &nsrSpecial1) {
                    if (activateMove(&nsrNormal2, true)) {
                        status = "Espada Normal 2 encadenado (U -> Fwd-Down-Fwd+U para Especial 1)";
                    }
                } else {
                    activateMove(&nsrNormal1, false);
                    status = "Espada Normal 1 ejecutado (pulsa U de nuevo para Normal 2)";
                }
            }
            // Golpe normal con O
            else if (IsKeyPressed(KEY_O)) {
                activateMove(&punchMove, false);
                lastCompletedMove = nullptr; // Rompe cadena de espada al usar golpe normal
                status = "Golpe normal (O)";
            }
            // Movimiento con WASD
            else {
                bool moved = false;
                if (IsKeyDown(KEY_D)) {
                    pos.x += walkSpeed;
                    facing = FacingDir::RIGHT;
                    activateMove(&walkMove, false);
                    moved = true;
                } else if (IsKeyDown(KEY_A)) {
                    pos.x -= walkSpeed;
                    facing = FacingDir::LEFT;
                    activateMove(&walkMove, false);
                    moved = true;
                }

                if (IsKeyDown(KEY_W)) {
                    pos.y -= walkSpeed;
                    activateMove(&walkUpMove, false);
                    moved = true;
                } else if (IsKeyDown(KEY_S)) {
                    pos.y += walkSpeed;
                    activateMove(&walkDownMove, false);
                    moved = true;
                }

                if (!moved) {
                    activateMove(&idleMove, false);
                }
            }
        }

        // Mantener dentro de la pantalla
        if (pos.x < 0.0f) pos.x = 0.0f;
        if (pos.y < 0.0f) pos.y = 0.0f;
        if (pos.x > screenWidth - 120.0f) pos.x = screenWidth - 120.0f;
        if (pos.y > screenHeight - 150.0f) pos.y = screenHeight - 150.0f;

        bool flipX = (facing == FacingDir::LEFT);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText(status.c_str(), 20, 20, 20, RAYWHITE);
        DrawText("WASD: Caminar | I (+A/D): Backdash | O: Golpe | U: Espada N1 -> N2 | Fwd-Down-Fwd+U: Esp. 1 <-> N2", 20, 50, 17, LIGHTGRAY);

        if (combat.ValidateMove()) {
            combat.ExecuteMove(0, 0, flipX, 0.0f, false);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}