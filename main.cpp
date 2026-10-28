#include "raylib.h"

int main() {
    // Definir dimensiones de la pantalla fija
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Inicializar la ventana de Raylib
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D - Programacion 3");

    SetTargetFPS(60);

    // Bucle principal del juego
    while (!WindowShouldClose()) {
        
        // --- 1. ACTUALIZACIÓN DE LÓGICA (Por ahora vacío) ---

        // --- 2. DIBUJADO ---
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Texto temporal de bienvenida
            DrawText("¡Arena Fighter 2D en construccion!", 220, 180, 20, DARKGRAY);
            DrawText("Rama Principal (main) - Equipo listo", 240, 220, 16, LIGHTGRAY);

        EndDrawing();
    }

    // Cerrar ventana y liberar recursos
    CloseWindow();
    return 0;
}