#include "raylib.h"
#include "LoadAssets.hpp"
#include "Mechanics.hpp"
#include "Player.hpp"
#include "enemyAI.hpp"

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
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Prueba Aislada - Rectángulos y Colisiones");
    SetTargetFPS(60);

    // Posiciones e instancias básicas usando solo rectángulos de Raylib
    Vector2 playerPos = { 200.0f, 300.0f };
    Vector2 playerSize = { 40.0f, 40.0f };
    float playerSpeed = 4.0f;

    EnemyAI enemy(500.0f, 300.0f, Difficulty::MEDIUM);

    while (!WindowShouldClose()) {
        // Movimiento básico de prueba
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerPos.x += playerSpeed;
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) playerPos.x -= playerSpeed;
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) playerPos.y -= playerSpeed;
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) playerPos.y += playerSpeed;

        bool isPlayerAttacking = IsKeyPressed(KEY_SPACE);
        enemy.Update(playerPos, isPlayerAttacking, screenWidth, screenHeight);

        BeginDrawing();
        ClearBackground(GRAY);

        DrawText("PRUEBA DE RECTÁNGULOS", 10, 10, 20, YELLOW);
        DrawText("Flechas/WASD: Mover cuadro | SPACE: Simular Ataque", 10, 35, 16, LIGHTGRAY);

        // Dibujar jugador como rectángulo azul
        DrawRectangleV(playerPos, playerSize, BLUE);

        // Dibujar enemigo (utiliza su propio método Draw)
        enemy.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}