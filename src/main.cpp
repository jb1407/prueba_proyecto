#include "raylib.h"
#include "LoadAssets.hpp"
#include "Mechanics.hpp"

int main()
{
    // 1. Inicializar la ventana y el juego base (Raylib)
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Sistema de Combate - DESIGNAR");
    SetTargetFPS(60);

    // 2. Inicializar el AssetManager y cargar todos los assets a la tabla hash
    AssetManager assetManager;
    LoadAssetsToTable(assetManager);

    // 3. Definir un offset por defecto
    Vector2 zeroOffset = {0.0f, 0.0f};

    // ==========================================
    // INSTANCIACIÓN DE MOVIMIENTOS
    // ==========================================
    // Movimientos de locomoción / base
    Move moveStanding("C_Standing", 1, false, assetManager.Search("C_Standing"), nullptr, nullptr, zeroOffset, zeroOffset, zeroOffset);
    Move moveWalk("C_Walk", 1, false, assetManager.Search("C_Walk"), nullptr, nullptr, zeroOffset, zeroOffset, zeroOffset);
    Move moveWalkUP("C_WalkUP", 1, false, assetManager.Search("C_WalkUP"), nullptr, nullptr, zeroOffset, zeroOffset, zeroOffset);
    Move moveWalkDown("C_WalkDown", 1, false, assetManager.Search("C_WalkDown"), nullptr, nullptr, zeroOffset, zeroOffset, zeroOffset);

    // Movimientos de Combate con Espada (Neon Sword Red)
    Move moveSwordNormal1("NSR_Normal1", 2, false, 
        assetManager.Search("NSR_Normal1(Character)"), 
        assetManager.Search("NSR_Normal1(Sword)"), 
        nullptr, zeroOffset, zeroOffset, zeroOffset);

    Move moveSwordNormal2("NSR_Normal2", 2, false, 
        assetManager.Search("NSR_Normal2(Character)"), 
        assetManager.Search("NSR_Normal2(Sword)"), 
        nullptr, zeroOffset, zeroOffset, zeroOffset);

    Move moveSwordSpecial1("NSR_Special1", 2, true, 
        assetManager.Search("NSR_Special1(Character)"), 
        assetManager.Search("NSR_Special1(Sword)"), 
        assetManager.Search("NSR_Special1(Circle)"), 
        zeroOffset, zeroOffset, zeroOffset);

    // ==========================================
    // CONFIGURACIÓN DE COMBOS (TABLA HASH DESIGNAR)
    // ==========================================
    // El golpe normal 1 permite encadenar directamente hacia el golpe normal 2
    moveSwordNormal1.AddAllowedFollowUp("NSR_Normal2");

    // ==========================================
    // VARIABLES DE CONTROL Y ESTADO
    // ==========================================
    CombatManager combatManager("C_Standing", &moveStanding, 2);
    std::string currentMoveName = "C_Standing";
    Move* activeMove = &moveStanding;

    float comboTimer = 0.0f;
    const float comboWindow = 0.6f; // Tiempo límite para mantener la cadena de combo

    // Variables para el esquive (Tecla I)
    bool isDodgeActive = false;
    float dodgeRotation = 0.0f;
    Vector2 playerVelocity = { 0, 0 };
    bool flipSpriteX = false;

    // ==========================================
    // BUCLE PRINCIPAL DEL JUEGO
    // ==========================================
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // ------------------------------------------
        // A. GESTIÓN DE INPUTS Y MOVIMIENTO DIAGONAL
        // ------------------------------------------
        Vector2 movementDir = { 0.0f, 0.0f };

        if (IsKeyDown(KEY_D)) { movementDir.x += 1.0f; flipSpriteX = false; }
        if (IsKeyDown(KEY_A)) { movementDir.x -= 1.0f; flipSpriteX = true;  }
        if (IsKeyDown(KEY_S)) { movementDir.y += 1.0f; }
        if (IsKeyDown(KEY_W)) { movementDir.y -= 1.0f; }

        // Regla de diagonales: si camina arriba/abajo puro usa su animación vertical,
        // si hay componente horizontal, prioriza la animación horizontal de caminar.
        if (!isDodgeActive) 
        {
            if (movementDir.y < 0 && movementDir.x == 0) {
                activeMove = &moveWalkUP;
            } 
            else if (movementDir.y > 0 && movementDir.x == 0) {
                activeMove = &moveWalkDown;
            } 
            else if (movementDir.x != 0) {
                activeMove = &moveWalk;
            } 
            else if (activeMove == &moveWalk || activeMove == &moveWalkUP || activeMove == &moveWalkDown) {
                activeMove = &moveStanding;
            }
        }

        // ------------------------------------------
        // B. GESTIÓN DE TEMPORIZADOR DE COMBOS
        // ------------------------------------------
        if (comboTimer > 0) {
            comboTimer -= deltaTime;
            if (comboTimer <= 0) {
                // Si pasa un tiempo considerable sin encadenar, se reinicia el estado base del combo
                currentMoveName = "C_Standing";
            }
        }

        // ------------------------------------------
        // C. INPUTS DE ATAQUE Y COMBOS (J, U)
        // ------------------------------------------
        // Tecla J: Ataque normal de espada / Encadenamiento
        if (IsKeyPressed(KEY_J)) 
        {
            // Verificamos si estamos en el primer ataque y la tabla hash permite encadenar al segundo
            if (currentMoveName == "NSR_Normal1" && activeMove->CanChainInto("NSR_Normal2")) 
            {
                activeMove = &moveSwordNormal2;
                currentMoveName = "NSR_Normal2";
                comboTimer = comboWindow;
            } 
            else 
            {
                // Primer golpe por defecto
                activeMove = &moveSwordNormal1;
                currentMoveName = "NSR_Normal1";
                comboTimer = comboWindow;
            }
        }

        // Tecla U: Golpe normal secundario genérico
        if (IsKeyPressed(KEY_U)) {
            // Lógica o asignación para el golpe normal con U
        }

        // Ataque Especial (Secuencia de direcciones A, S, D junto con J)
        if (IsKeyPressed(KEY_J) && IsKeyDown(KEY_D) && IsKeyDown(KEY_S) && IsKeyDown(KEY_A)) 
        {
            activeMove = &moveSwordSpecial1;
            currentMoveName = "NSR_Special1";
        }

        // ------------------------------------------
        // D. GESTIÓN DEL ESQUIVE (Tecla I - 360 Grados + Impulso)
        // ------------------------------------------
        if (IsKeyPressed(KEY_I) && !isDodgeActive) 
        {
            isDodgeActive = true;
            dodgeRotation = 0.0f;
            
            // Impulso hacia la dirección actual de movimiento (o hacia el frente si está estático)
            playerVelocity = { movementDir.x * 400.0f, movementDir.y * 400.0f };
            if (playerVelocity.x == 0 && playerVelocity.y == 0) {
                playerVelocity.x = flipSpriteX ? -400.0f : 400.0f;
            }
        }

        if (isDodgeActive) 
        {
            // Rota 360 grados rápidamente durante el esquive
            dodgeRotation += 720.0f * deltaTime; 
            
            // Simular un pequeño desplazamiento del personaje (puedes adaptarlo a tu lógica de posición global)
            // position.x += playerVelocity.x * deltaTime;

            // Duración del esquive (por ejemplo, al completar dos vueltas de rotación)
            if (dodgeRotation >= 720.0f) { 
                isDodgeActive = false;
                dodgeRotation = 0.0f;
            }
        }

        // ------------------------------------------
        // E. RENDERIZADO Y DIBUJADO EN PANTALLA
        // ------------------------------------------
        BeginDrawing();
        ClearBackground(BLUE);

        // Determinamos la rotación actual (si está esquivando rota, sino es 0)
        float currentRotation = isDodgeActive ? dodgeRotation : 0.0f;

        // Ejecutamos la lógica de animación del movimiento activo actual
        if (activeMove != nullptr) 
        {
            activeMove->AnimationLogicAndDraw(0, 0, flipSpriteX, currentRotation);
        }

        EndDrawing();
    }

    // 4. Cerrar ventana y liberar recursos de Raylib
    CloseWindow();
    return 0;
}