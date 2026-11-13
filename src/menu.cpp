#include <iostream> // Asegúrate de tener esto arriba en menu.cpp
#include "raylib.h"
#include "menu.hpp"

void ActualizarMenu(GameScreen *currentScreen, int *selectedDifficulty) {
    Vector2 mousePos = GetMousePosition();
    
    // Variable estática para evitar que se dispare múltiples veces seguidas mientras mantienes presionado
    static bool clickBloqueado = false;

    // Si el usuario presiona el click izquierdo del mouse
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (!clickBloqueado) {
            clickBloqueado = true; // Bloqueamos hasta que suelte el botón

            switch(*currentScreen) {
                case SCREEN_MAIN_MENU:
                    if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 220 && mousePos.y <= 260) {
                        *currentScreen = SCREEN_MODE_SELECT;
                    }
                    else if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 280 && mousePos.y <= 320) {
                        *currentScreen = SCREEN_INSTRUCTIONS;
                    }
                    break;

                case SCREEN_MODE_SELECT:
                    if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 180 && mousePos.y <= 220) {
                        *currentScreen = SCREEN_DIFFICULTY_SELECT;
                    }
                    else if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 240 && mousePos.y <= 280) {
                        *currentScreen = SCREEN_CHARACTER_SELECT;
                    }
                    else if (mousePos.x >= 50 && mousePos.x <= 130 && mousePos.y >= 400 && mousePos.y <= 430) {
                        *currentScreen = SCREEN_MAIN_MENU;
                    }
                    break;

                case SCREEN_DIFFICULTY_SELECT:
                    if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 160 && mousePos.y <= 200) { 
                        *selectedDifficulty = 0; 
                        *currentScreen = SCREEN_CHARACTER_SELECT; 
                    }
                    else if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 220 && mousePos.y <= 260) { 
                        *selectedDifficulty = 1; 
                        *currentScreen = SCREEN_CHARACTER_SELECT; 
                    }
                    else if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 280 && mousePos.y <= 320) { 
                        *selectedDifficulty = 2; 
                        *currentScreen = SCREEN_CHARACTER_SELECT; 
                    }
                    else if (mousePos.x >= 50 && mousePos.x <= 130 && mousePos.y >= 400 && mousePos.y <= 430) { 
                        *currentScreen = SCREEN_MODE_SELECT; 
                    }
                    break;

                case SCREEN_CHARACTER_SELECT:
                case SCREEN_INSTRUCTIONS:
                    if (mousePos.x >= 50 && mousePos.x <= 130 && mousePos.y >= 400 && mousePos.y <= 430) {
                        *currentScreen = SCREEN_MAIN_MENU;
                    }
                    break;

                default:
                    break;
            }
        }
    } 
    else {
        // Cuando el usuario suelta el botón del mouse, reseteamos el bloqueo
        clickBloqueado = false;
    }
}

// Lógica de renderizado visual con estilo oscuro y acentos amarillos
void DibujarMenu(GameScreen currentScreen, int selectedDifficulty)
{
    const int screenWidth = 800;

    BeginDrawing();
    ClearBackground((Color){15, 15, 18, 255}); // Fondo oscuro profundo

    // Obtenemos la posición del mouse una sola vez por frame para evaluar colisiones
    Vector2 mousePos = GetMousePosition();

    Color grisMetal = (Color){180, 180, 190, 255};
    Color grisOscuro = (Color){40, 40, 45, 255};
    Color grisHover = (Color){65, 65, 75, 255}; // Color cuando el mouse está encima
    Color amarilloEspada = (Color){245, 220, 40, 255};
    Color textoClaro = (Color){230, 230, 235, 255};

    switch (currentScreen)
    {
    case SCREEN_MAIN_MENU:
    {
        DrawText("ARENA FIGHTER 2D", screenWidth / 2 - MeasureText("ARENA FIGHTER 2D", 32) / 2 + 2, 82, 32, BLACK);
        DrawText("ARENA FIGHTER 2D", screenWidth / 2 - MeasureText("ARENA FIGHTER 2D", 32) / 2, 80, 32, amarilloEspada);

        // 1. Definimos el rectángulo del botón una sola vez para usarlo tanto en el dibujo como en la colisión
        Rectangle btnJugar = {300, 220, 200, 40};

        // 2. Comprobación manual de la hitbox (reemplazando a CheckCollisionPointRec)
        bool hoverJugar = (mousePos.x >= btnJugar.x &&
                           mousePos.x <= (btnJugar.x + btnJugar.width) &&
                           mousePos.y >= btnJugar.y &&
                           mousePos.y <= (btnJugar.y + btnJugar.height));

        // 3. Dibujado usando esa misma variable
        DrawRectangleRec(btnJugar, hoverJugar ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnJugar.x, (int)btnJugar.y, (int)btnJugar.width, (int)btnJugar.height, hoverJugar ? amarilloEspada : grisMetal);
        DrawText("Jugar", screenWidth / 2 - MeasureText("Jugar", 16) / 2, 232, 16, textoClaro);

        // Botón Instrucciones
        Rectangle btnInstrucciones = {300, 280, 200, 40};
        bool hoverInstrucciones = CheckCollisionPointRec(mousePos, btnInstrucciones);
        DrawRectangleRec(btnInstrucciones, hoverInstrucciones ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnInstrucciones.x, (int)btnInstrucciones.y, (int)btnInstrucciones.width, (int)btnInstrucciones.height, hoverInstrucciones ? amarilloEspada : grisMetal);
        DrawText("Instrucciones", screenWidth / 2 - MeasureText("Instrucciones", 16) / 2, 292, 16, textoClaro);
        break;
    }

    case SCREEN_MODE_SELECT:
    {
        DrawText("SELECCIONA MODO", screenWidth / 2 - MeasureText("SELECCIONA MODO", 22) / 2, 80, 22, amarilloEspada);

        Rectangle btnModo1 = {300, 180, 200, 40};
        bool hoverModo1 = CheckCollisionPointRec(mousePos, btnModo1);
        DrawRectangleRec(btnModo1, hoverModo1 ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnModo1.x, (int)btnModo1.y, (int)btnModo1.width, (int)btnModo1.height, hoverModo1 ? amarilloEspada : grisMetal);
        DrawText("1 Jugador (vs Bot)", screenWidth / 2 - MeasureText("1 Jugador (vs Bot)", 14) / 2, 193, 14, textoClaro);

        Rectangle btnModo2 = {300, 240, 200, 40};
        bool hoverModo2 = CheckCollisionPointRec(mousePos, btnModo2);
        DrawRectangleRec(btnModo2, hoverModo2 ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnModo2.x, (int)btnModo2.y, (int)btnModo2.width, (int)btnModo2.height, hoverModo2 ? amarilloEspada : grisMetal);
        DrawText("2 Jugadores (Local)", screenWidth / 2 - MeasureText("2 Jugadores (Local)", 14) / 2, 253, 14, textoClaro);

        Rectangle btnVolver = {50, 400, 80, 30};
        bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
        DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
        DrawText("Volver", 72, 408, 12, textoClaro);
        break;
    }

    case SCREEN_DIFFICULTY_SELECT:
    {
        DrawText("ELIGE LA DIFICULTAD", screenWidth / 2 - MeasureText("ELIGE LA DIFICULTAD", 20) / 2, 80, 20, amarilloEspada);

        Rectangle btnFacil = {300, 160, 200, 40};
        bool hoverFacil = CheckCollisionPointRec(mousePos, btnFacil);
        DrawRectangleRec(btnFacil, hoverFacil ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnFacil.x, (int)btnFacil.y, (int)btnFacil.width, (int)btnFacil.height, hoverFacil ? amarilloEspada : grisMetal);
        DrawText("Facil", screenWidth / 2 - MeasureText("Facil", 14) / 2, 173, 14, textoClaro);

        Rectangle btnMedio = {300, 220, 200, 40};
        bool hoverMedio = CheckCollisionPointRec(mousePos, btnMedio);
        DrawRectangleRec(btnMedio, hoverMedio ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnMedio.x, (int)btnMedio.y, (int)btnMedio.width, (int)btnMedio.height, hoverMedio ? amarilloEspada : grisMetal);
        DrawText("Medio", screenWidth / 2 - MeasureText("Medio", 14) / 2, 233, 14, textoClaro);

        Rectangle btnDificil = {300, 280, 200, 40};
        bool hoverDificil = CheckCollisionPointRec(mousePos, btnDificil);
        DrawRectangleRec(btnDificil, hoverDificil ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnDificil.x, (int)btnDificil.y, (int)btnDificil.width, (int)btnDificil.height, hoverDificil ? amarilloEspada : grisMetal);
        DrawText("Dificil", screenWidth / 2 - MeasureText("Dificil", 14) / 2, 293, 14, textoClaro);

        Rectangle btnVolver = {50, 400, 80, 30};
        bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
        DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
        DrawText("Volver", 72, 408, 12, textoClaro);
        break;
    }

    case SCREEN_CHARACTER_SELECT: {
            // 1. Título más grande (cambiamos el tamaño de 22 a 28)
            DrawText("ELIGE TU JUGADOR", screenWidth/2 - MeasureText("ELIGE TU JUGADOR", 28)/2, 35, 28, amarilloEspada);
            
            // --- JUGADOR 1 (Izquierda) ---
            DrawText("NOMBRE", 260 - MeasureText("NOMBRE", 16)/2, 95, 16, grisMetal);
            // Cuadro más grande: ancho 160, alto 130 (antes era 120x100)
            DrawRectangleLines(180, 120, 160, 130, grisMetal); 
            DrawText("<", 150, 170, 24, amarilloEspada); // Flecha izquierda desplazada acorde al tamaño
            DrawText(">", 345, 170, 24, amarilloEspada); // Flecha derecha desplazada acorde al tamaño

            // --- OPONENTE (Derecha) ---
            DrawText("OPONENTE", 540 - MeasureText("OPONENTE", 16)/2, 95, 16, grisMetal);
            // Cuadro más grande: ancho 160, alto 130
            DrawRectangleLines(460, 120, 160, 130, grisMetal); 
            DrawText("<", 430, 170, 24, amarilloEspada); 
            DrawText(">", 625, 170, 24, amarilloEspada); 

            // Botón Volver (esquina inferior izquierda)
            Rectangle btnVolver = {50, 400, 80, 30};
            bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
            DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
            DrawText("Volver", 72, 408, 12, textoClaro);
            break;
        }

    case SCREEN_INSTRUCTIONS:
    {
        DrawText("INSTRUCCIONES", screenWidth / 2 - MeasureText("INSTRUCCIONES", 20) / 2, 40, 20, amarilloEspada);
        DrawText("- Combate a mano limpia y con moveset base defensivo.", 100, 110, 14, textoClaro);
        DrawText("- Las armas aparecen aleatoriamente en el escenario.", 100, 140, 14, textoClaro);
        DrawText("- Las armas tienen tiempo de vida limitado.", 100, 170, 14, textoClaro);

        Rectangle btnVolver = {50, 400, 80, 30};
        bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
        DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
        DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
        DrawText("Volver", 72, 408, 12, textoClaro);
        break;
    }

    default:
        break;
    }

    // Opcional: Esto te muestra las coordenadas del mouse en una esquina para verificar la precisión
    DrawText(TextFormat("X: %.0f Y: %.0f", mousePos.x, mousePos.y), 10, 430, 10, GRAY);

    EndDrawing();
}