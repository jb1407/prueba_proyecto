#ifndef MENU_HPP
#define MENU_HPP

#include "raylib.h"

// Estados de las pantallas del juego y el menú principal
typedef enum {
    SCREEN_MAIN_MENU = 0,
    SCREEN_MODE_SELECT,
    SCREEN_DIFFICULTY_SELECT,
    SCREEN_CHARACTER_SELECT,
    SCREEN_INSTRUCTIONS,
    SCREEN_SETTINGS,
    SCREEN_GAME_PLAYING
} GameScreen;

// Declaración de las funciones públicas del menú
void ActualizarMenu(GameScreen *currentScreen, int *selectedDifficulty);
void DibujarMenu(GameScreen currentScreen, int selectedDifficulty);

#endif