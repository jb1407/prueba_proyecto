#include "raylib.h"
#include "LoadAssets.hpp"
#include "Mechanics.hpp"
#include "Player.hpp"
#include "enemyAI.hpp"
#include "hud.hpp"

/*int main() {
    // 1. Inicializar la ventana de Raylib
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Arena Fighter - Prueba de Integracion");
    SetTargetFPS(60);

    // 2. Inicializar el administrador de assets y cargar la tabla hash
    AssetManager assetManager;
    LoadAssetsToTable(assetManager);

    // 3. Crear una instancia del jugador en el centro de la pantalla
    // (Pasamos la posición inicial X y Y)
    Player player(screenWidth / 2.0f - 20.0f, screenHeight / 2.0f - 20.0f);

    // 4. Bucle principal del juego
    while (!WindowShouldClose()) {
        
        // --- Actualización de lógica e inputs ---
        // El jugador lee el teclado, actualiza su posición y le pide al CombatManager 
        // que dibuje la animación correspondiente usando la tabla hash.
        player.Update(assetManager);

        // --- Dibujado en pantalla ---
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Texto de guía en la interfaz
        DrawText("Usa W, A, S, D para moverte y ESPACIO para golpear", 20, 20, 18, LIGHTGRAY);
        
        // Opcional: Dibujar el rectángulo de colisión del jugador para depurar
        DrawRectangleLinesEx(player.GetRectangle(), 1, GREEN);

        EndDrawing();
    }

    // 5. Cierre de la aplicación
    // Nota: El destructor de AssetManager (~AssetManager) se encargará 
    // automáticamente de liberar las texturas de la GPU al salir del ámbito.
    CloseWindow();

    return 0;
}*/

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // --- POSICIONES DINÁMICAS ---
    // Jugador: 15% desde la izquierda, centrado verticalmente
    float playerStartX = screenWidth * 0.15f;
    float playerStartY = (screenHeight / 2.0f) - 20.0f; 

    // Enemigo: 85% desde la izquierda, centrado verticalmente
    float enemyStartX = (screenWidth * 0.85f) - 40.0f; 
    float enemyStartY = (screenHeight / 2.0f) - 20.0f;

    InitWindow(screenWidth, screenHeight, "Prueba Aislada - Sprites y Animaciones");
    SetTargetFPS(60);

    // 1. INICIALIZAR Y CARGAR LOS ASSETS
    AssetManager assetManager(10); 
    LoadAssetsToTable(assetManager);

    // Fuente para el HUD (puedes usar GetFontDefault() o cargar tu fuente personalizada del menú con LoadFont("ruta/fuente.ttf"))
    Font menuFont = GetFontDefault(); 

    // Posiciones, estadísticas e instancias básicas de prueba
    Vector2 playerPos = { playerStartX, playerStartY };
    Vector2 playerSize = { 40.0f, 40.0f };
    float playerSpeed = 4.0f;
    
    float playerHealth = 100.0f;
    float playerMaxHealth = 100.0f;

    EnemyAI enemy(enemyStartX, enemyStartY, Difficulty::MEDIUM);

    while (!WindowShouldClose()) {
        // Movimiento básico de prueba del jugador
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPos.x += playerSpeed;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) playerPos.y += playerSpeed;

        bool isPlayerAttacking = IsKeyPressed(KEY_SPACE);
        enemy.Update(playerPos, isPlayerAttacking, screenWidth, screenHeight);

        // --- SIMULACIÓN RÁPIDA DE DAÑO PARA PROBAR LAS BARRAS ---
        // (Opcional: puedes quitar esto luego cuando conectes las verdaderas colisiones de hitboxes)
        if (IsKeyPressed(KEY_ENTER)) {
            playerHealth -= 15.0f; // Si presionas Enter, el jugador pierde vida
            enemy.TakeDamage(20.0f); // Si presionas Enter, el enemigo pierde vida
        }

        BeginDrawing();
        ClearBackground(GRAY);

        DrawText("PRUEBA DE SPRITES - ENEMIGO IA", 10, 10, 20, YELLOW);
        DrawText("Flechas/WASD: Mover cuadro | SPACE: Simular Ataque | ENTER: Probar daño en barras", 10, 35, 16, LIGHTGRAY);

        // Dibujar jugador como rectángulo azul
        DrawRectangleV(playerPos, playerSize, BLUE);

        // 2. DIBUJAR AL ENEMIGO CON SU MÉTODO ANIMADO
        enemy.Draw(assetManager, playerPos);

        // 3. DIBUJAR LAS BARRAS DE VIDA (HUD) EN LAS ESQUINAS
        HUD::Draw(
            screenWidth, 
            screenHeight, 
            playerHealth, 
            playerMaxHealth, 
            enemy.GetHealth(), 
            enemy.GetMaxHealth(), 
            menuFont
        );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}