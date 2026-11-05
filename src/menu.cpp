#include "menu.hpp"

// Logica de actualización y navegación entre pantallas
void ActualizarMenu(GameScreen *currentScreen, int *selectedDifficulty) {
    Vector2 mousePos = GetMousePosition();

    switch(*currentScreen) {
        case SCREEN_MAIN_MENU:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                // Boton Jugar
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 180, 200, 40})) {
                    *currentScreen = SCREEN_MODE_SELECT;
                }
                // Boton Instrucciones
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 240, 200, 40})) {
                    *currentScreen = SCREEN_INSTRUCTIONS;
                }
            }
            break;

        case SCREEN_MODE_SELECT:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 160, 200, 40})) {
                    *currentScreen = SCREEN_DIFFICULTY_SELECT;
                }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 220, 200, 40})) {
                    *currentScreen = SCREEN_CHARACTER_SELECT;
                }
                else if (CheckCollisionPointRec(mousePos, {50, 400, 80, 30})) {
                    *currentScreen = SCREEN_MAIN_MENU;
                }
            }
            break;

        case SCREEN_DIFFICULTY_SELECT:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 150, 200, 40})) { *selectedDifficulty = 0; *currentScreen = SCREEN_CHARACTER_SELECT; }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 210, 200, 40})) { *selectedDifficulty = 1; *currentScreen = SCREEN_CHARACTER_SELECT; }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 270, 200, 40})) { *selectedDifficulty = 2; *currentScreen = SCREEN_CHARACTER_SELECT; }
                else if (CheckCollisionPointRec(mousePos, {50, 400, 80, 30})) { *currentScreen = SCREEN_MODE_SELECT; }
            }
            break;

        case SCREEN_CHARACTER_SELECT:
        case SCREEN_INSTRUCTIONS:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, {50, 400, 80, 30})) {
                    *currentScreen = SCREEN_MAIN_MENU;
                }
            }
            break;

        default:
            break;
    }
}