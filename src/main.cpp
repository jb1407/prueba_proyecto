#include "raylib.h"
#include "menu.hpp"
// #include "Player.hpp" // Tus otros headers de combate y jugador

int main()
{
    // 1. Inicializar ventana
    const int screenWidth = 800;
    const int screenHeight = 450;
    //SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D");
    SetTargetFPS(60);

    // Variables de estado iniciales
    GameScreen currentScreen = SCREEN_MAIN_MENU;
    int selectedDifficulty = 0; // 0: Fácil, 1: Medio, 2: Difícil

    // Bucle principal del juego
    while (!WindowShouldClose())
    {

        // ==========================================
        // PASO 1: ACTUALIZAR LÓGICA SEGÚN LA PANTALLA
        // ==========================================
        if (currentScreen == SCREEN_GAME_PLAYING)
        {
            // Marcador de posición temporal (Placeholder) solo para probar el menú
            BeginDrawing();
            ClearBackground(DARKGRAY);
            DrawText("¡AQUI IRA LA PELEA CONTRA EL BOT!", 200, 180, 20, RAYWHITE);
            DrawText("Presiona ESC para volver al menu principal", 220, 220, 14, LIGHTGRAY);
            EndDrawing();

            // Atajo temporal para regresar al menú con ESC
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = SCREEN_MAIN_MENU;
            }
        }
        else
        {
            // Aquí corre la lógica visual y de clics del menú de tu compañera
            ActualizarMenu(&currentScreen, &selectedDifficulty);

            BeginDrawing();
            DibujarMenu(currentScreen, selectedDifficulty);
            EndDrawing();
        }

        // ==========================================
        // PASO 2: RENDERIZAR / DIBUJAR EN PANTALLA
        // ==========================================
        BeginDrawing();
        ClearBackground(BLACK);

        if (currentScreen == SCREEN_GAME_PLAYING)
        {
            // --- DIBUJAR LA ARENA Y LOS COMBATIENTES ---
            DrawText("¡COMBATE EN CURSO!", 250, 200, 20, GREEN);
            DrawText("Presiona ESC para volver al menu", 230, 240, 14, GRAY);
            // DibujarPlayer();
            // DibujarEnemigo();
        }
        else
        {
            // Dibujar las pantallas del menú hechas por tu compañera
            DibujarMenu(currentScreen, selectedDifficulty);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}