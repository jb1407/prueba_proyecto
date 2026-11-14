#include "raylib.h"
#include "menu.hpp"

int main() {
    // Inicialización de la ventana (ajustada a la resolución estándar de 800x450 usada en el menú)
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D - Menu Test");
    SetTargetFPS(60);

    // Estado inicial y variables de configuración del menú
    GameScreen currentScreen = SCREEN_MAIN_MENU;
    int selectedDifficulty = 0; // 0: Fácil, 1: Medio, 2: Difícil

    // Bucle principal del juego
    while (!WindowShouldClose()) {
        // 1. Actualizar lógica de botones y pantallas
        ActualizarMenu(&currentScreen, &selectedDifficulty);

        // 2. Renderizado según el estado actual
        if (currentScreen == SCREEN_GAME_PLAYING) {
            // Pantalla de prueba cuando inicia el combate
            BeginDrawing();
            ClearBackground((Color){15, 15, 18, 255});
            DrawText("¡COMBATE EN CURSO (JUEGO)!", screenWidth / 2 - MeasureText("¡COMBATE EN CURSO (JUEGO)!", 24) / 2, screenHeight / 2 - 20, 24, (Color){245, 220, 40, 255});
            DrawText(TextFormat("Dificultad seleccionada: %d", selectedDifficulty), screenWidth / 2 - MeasureText("Dificultad seleccionada: %d", 16) / 2, screenHeight / 2 + 20, 16, (Color){180, 180, 190, 255});
            EndDrawing();
        } else {
            // Renderizar las pantallas del menú mediante tu archivo menu.cpp
            DibujarMenu(currentScreen, selectedDifficulty);
        }
    }

    // Cerrar ventana y recursos
    CloseWindow();
    return 0;
}