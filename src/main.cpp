#include "raylib.h"
#include "menu.hpp"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D");
    SetTargetFPS(60);

    GameScreen currentScreen = SCREEN_MAIN_MENU;
    int selectedDifficulty = 0;
    bool esDosJugadores = false; // <--- 1. Declaramos la variable aquí

    // Variables para la barra de carga simulada
    float loadingProgress = 0.0f; // Va de 0.0 a 1.0 (0% a 100%)
    const float loadingDuration = 2.0f; // Duración total en segundos

    while (!WindowShouldClose()) {
        // 1. Actualización lógica según la pantalla
        if (currentScreen == SCREEN_LOADING) {
            // Incrementamos el progreso de manera constante
            loadingProgress += GetFrameTime() / loadingDuration;
            
            if (loadingProgress >= 1.0f) {
                loadingProgress = 0.0f;          // Reiniciamos por si se vuelve a usar
                currentScreen = SCREEN_GAME_PLAYING; // Pasamos al juego
            }
        } else {
            // 2. Pasamos el puntero de la variable a la función ActualizarMenu
            ActualizarMenu(&currentScreen, &selectedDifficulty, &esDosJugadores);
        }

        // 2. Renderizado
        if (currentScreen == SCREEN_GAME_PLAYING) {
            BeginDrawing();
            ClearBackground((Color){15, 15, 18, 255});
            DrawText("¡COMBATE EN CURSO!", screenWidth / 2 - MeasureText("¡COMBATE EN CURSO!", 24) / 2, screenHeight / 2 - 20, 24, (Color){245, 220, 40, 255});
            DrawText(TextFormat("Dificultad: %d", selectedDifficulty), screenWidth / 2 - MeasureText("Dificultad: %d", 16) / 2, screenHeight / 2 + 20, 16, (Color){180, 180, 190, 255});
            EndDrawing();
        } 
        else if (currentScreen == SCREEN_LOADING) {
            // Pantalla de carga simulada con texto central y barrita inferior
            BeginDrawing();
            ClearBackground((Color){15, 15, 18, 255});

            // Texto "Cargando..." centrado
            const char* textoCarga = "CARGANDO PARTIDA...";
            DrawText(textoCarga, screenWidth / 2 - MeasureText(textoCarga, 20) / 2, screenHeight / 2 - 30, 20, (Color){230, 230, 235, 255});

            // Barra de carga (Contenedor)
            float barWidth = 400.0f;
            float barHeight = 20.0f;
            float barX = (screenWidth - barWidth) / 2;
            float barY = screenHeight / 2 + 20;

            DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, (Color){180, 180, 190, 255});
            
            // Relleno de la barra según el progreso constante
            float fillWidth = barWidth * loadingProgress;
            DrawRectangle((int)barX + 2, (int)barY + 2, (int)(fillWidth - 4), (int)(barHeight - 4), (Color){245, 220, 40, 255});

            EndDrawing();
        } 
        else {
            DibujarMenu(currentScreen, selectedDifficulty);
        }
    }

    CloseWindow();
    return 0;
}