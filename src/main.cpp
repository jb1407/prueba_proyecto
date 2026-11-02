#include "raylib.h"
#include "Player.hpp"
#include "enemyAI.hpp"

int main() {
    // 1. Inicializar la ventana y la tasa de refresco
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D - Test Integración");
    SetTargetFPS(60);

    // 2. Instanciar al jugador y al bot
    Player player(150.0f, 300.0f);
    EnemyAI enemy(600.0f, 300.0f, Difficulty::MEDIUM);

    // 3. Bucle principal del juego
    while (!WindowShouldClose()) {
        
        // --- ACTUALIZACIÓN DE LÓGICA ---
        player.Update();

        // Actualizamos la IA pasándole:
        // 1. La posición del jugador
        // 2. Si el jugador está atacando (le pasamos false por ahora)
        // 3. El ancho y alto de la pantalla para los límites
        enemy.Update(player.getPosition(), false, screenWidth, screenHeight);

        // --- RENDERIZADO (DIBUJO) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Suelo temporal de referencia
        DrawRectangle(0, 360, screenWidth, 90, DARKGRAY);

        // Dibujar al jugador (Azul)
        DrawRectangle((int)player.getX(), (int)player.getY(), 40, 60, BLUE);

        // Dibujar al Bot (Rojo) usando directamente el Rectangle que devuelve su método GetRectangle()
        DrawRectangleRec(enemy.GetRectangle(), RED);

        // Textos informativos
        DrawText("Usa las Flechas o WASD para mover al Jugador", 20, 20, 18, DARKGRAY);
        DrawText("Jugador = Azul | Bot = Rojo", 20, 45, 14, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}