#include "menu.hpp"
void ActualizarMenu(GameScreen *currentScreen, int *selectedDifficulty) {
    Vector2 mousePos = GetMousePosition();

    switch(*currentScreen) {
        case SCREEN_MAIN_MENU:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 220, 200, 40})) {
                    *currentScreen = SCREEN_MODE_SELECT;
                }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 280, 200, 40})) {
                    *currentScreen = SCREEN_INSTRUCTIONS;
                }
            }
            break;

        case SCREEN_MODE_SELECT:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 180, 200, 40})) {
                    *currentScreen = SCREEN_DIFFICULTY_SELECT;
                }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 240, 200, 40})) {
                    *currentScreen = SCREEN_CHARACTER_SELECT;
                }
                else if (CheckCollisionPointRec(mousePos, {50, 400, 80, 30})) {
                    *currentScreen = SCREEN_MAIN_MENU;
                }
            }
            break;

        case SCREEN_DIFFICULTY_SELECT:
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (CheckCollisionPointRec(mousePos, (Rectangle){300, 160, 200, 40})) { *selectedDifficulty = 0; *currentScreen = SCREEN_CHARACTER_SELECT; }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 220, 200, 40})) { *selectedDifficulty = 1; *currentScreen = SCREEN_CHARACTER_SELECT; }
                else if (CheckCollisionPointRec(mousePos, (Rectangle){300, 280, 200, 40})) { *selectedDifficulty = 2; *currentScreen = SCREEN_CHARACTER_SELECT; }
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

// Lógica de renderizado visual con estilo oscuro y acentos amarillos
void DibujarMenu(GameScreen currentScreen, int selectedDifficulty) {
    const int screenWidth = 800;

    BeginDrawing();
    ClearBackground((Color){15, 15, 18, 255}); // Fondo oscuro profundo

    Color grisMetal = (Color){180, 180, 190, 255};
    Color grisOscuro = (Color){40, 40, 45, 255};
    Color amarilloEspada = (Color){245, 220, 40, 255};
    Color textoClaro = (Color){230, 230, 235, 255};

    switch(currentScreen) {
        case SCREEN_MAIN_MENU:
            DrawText("ARENA FIGHTER 2D", screenWidth/2 - MeasureText("ARENA FIGHTER 2D", 32)/2 + 2, 82, 32, BLACK);
            DrawText("ARENA FIGHTER 2D", screenWidth/2 - MeasureText("ARENA FIGHTER 2D", 32)/2, 80, 32, amarilloEspada);
            
            DrawRectangleRec((Rectangle){300, 220, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 220, 200, 40, grisMetal);
            DrawText("Jugar", screenWidth/2 - MeasureText("Jugar", 16)/2, 232, 16, textoClaro);

            DrawRectangleRec((Rectangle){300, 280, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 280, 200, 40, grisMetal);
            DrawText("Instrucciones", screenWidth/2 - MeasureText("Instrucciones", 16)/2, 292, 16, textoClaro);
            break;

        case SCREEN_MODE_SELECT:
            DrawText("SELECCIONA MODO", screenWidth/2 - MeasureText("SELECCIONA MODO", 22)/2, 80, 22, amarilloEspada);
            
            DrawRectangleRec((Rectangle){300, 180, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 180, 200, 40, grisMetal);
            DrawText("1 Jugador (vs Bot)", screenWidth/2 - MeasureText("1 Jugador (vs Bot)", 14)/2, 193, 14, textoClaro);

            DrawRectangleRec((Rectangle){300, 240, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 240, 200, 40, grisMetal);
            DrawText("2 Jugadores (Local)", screenWidth/2 - MeasureText("2 Jugadores (Local)", 14)/2, 253, 14, textoClaro);

            DrawRectangleRec((Rectangle){50, 400, 80, 30}, grisOscuro);
            DrawRectangleLines(50, 400, 80, 30, grisMetal);
            DrawText("Volver", 72, 408, 12, textoClaro);
            break;

        case SCREEN_DIFFICULTY_SELECT:
            DrawText("ELIGE LA DIFICULTAD", screenWidth/2 - MeasureText("ELIGE LA DIFICULTAD", 20)/2, 80, 20, amarilloEspada);
            
            DrawRectangleRec((Rectangle){300, 160, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 160, 200, 40, grisMetal);
            DrawText("Facil", screenWidth/2 - MeasureText("Facil", 14)/2, 173, 14, textoClaro);

            DrawRectangleRec((Rectangle){300, 220, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 220, 200, 40, grisMetal);
            DrawText("Medio", screenWidth/2 - MeasureText("Medio", 14)/2, 233, 14, textoClaro);

            DrawRectangleRec((Rectangle){300, 280, 200, 40}, grisOscuro);
            DrawRectangleLines(300, 280, 200, 40, grisMetal);
            DrawText("Dificil", screenWidth/2 - MeasureText("Dificil", 14)/2, 293, 14, textoClaro);

            DrawRectangleRec((Rectangle){50, 400, 80, 30}, grisOscuro);
            DrawRectangleLines(50, 400, 80, 30, grisMetal);
            DrawText("Volver", 72, 408, 12, textoClaro);
            break;

        case SCREEN_CHARACTER_SELECT:
            DrawText("ELIGE TU JUGADOR", screenWidth/2 - MeasureText("ELIGE TU JUGADOR", 22)/2, 40, 22, amarilloEspada);
            
            DrawText("NOMBRE", 150, 140, 14, grisMetal);
            DrawRectangleLines(250, 110, 120, 100, grisMetal);
            DrawText("<", 230, 150, 20, amarilloEspada);
            DrawText(">", 380, 150, 20, amarilloEspada);

            DrawText("OPONENTE", 150, 270, 14, grisMetal);
            DrawRectangleLines(250, 240, 120, 100, grisMetal);
            DrawText("<", 230, 280, 20, amarilloEspada);
            DrawText(">", 380, 280, 20, amarilloEspada);

            DrawRectangleRec((Rectangle){50, 400, 80, 30}, grisOscuro);
            DrawRectangleLines(50, 400, 80, 30, grisMetal);
            DrawText("Volver", 72, 408, 12, textoClaro);
            break;

        case SCREEN_INSTRUCTIONS:
            DrawText("INSTRUCCIONES", screenWidth/2 - MeasureText("INSTRUCCIONES", 20)/2, 40, 20, amarilloEspada);
            DrawText("- Combate a mano limpia y con moveset base defensivo.", 100, 110, 14, textoClaro);
            DrawText("- Las armas aparecen aleatoriamente en el escenario.", 100, 140, 14, textoClaro);
            DrawText("- Las armas tienen tiempo de vida limitado.", 100, 170, 14, textoClaro);
            
            DrawRectangleRec((Rectangle){50, 400, 80, 30}, grisOscuro);
            DrawRectangleLines(50, 400, 80, 30, grisMetal);
            DrawText("Volver", 72, 408, 12, textoClaro);
            break;

        default:
            break;
    }

    EndDrawing();
}