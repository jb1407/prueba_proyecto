#include <iostream>
#include "raylib.h"
#include "menu.hpp"

void ActualizarMenu(GameScreen *currentScreen, int *selectedDifficulty, bool *esDosJugadores) {
    Vector2 mousePos = GetMousePosition();
    static bool clickBloqueado = false;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (!clickBloqueado) {
            clickBloqueado = true;

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
                        *esDosJugadores = false;                    // Modo 1 Jugador
                        *currentScreen = SCREEN_DIFFICULTY_SELECT;  // Va a dificultad
                    }
                    else if (mousePos.x >= 300 && mousePos.x <= 500 && mousePos.y >= 240 && mousePos.y <= 280) {
                        *esDosJugadores = true;                     // Modo 2 Jugadores
                        *currentScreen = SCREEN_CHARACTER_SELECT;   // Salta directo a personajes
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

                case SCREEN_CHARACTER_SELECT: {
                    // Botón Volver (Izquierda)
                    Rectangle btnVolver = {50, 400, 80, 30};
                    // Botón Comenzar / Jugar (Derecha)
                    Rectangle btnComenzar = {670, 400, 80, 30};

                    if (mousePos.x >= btnVolver.x && mousePos.x <= btnVolver.x + btnVolver.width &&
                        mousePos.y >= btnVolver.y && mousePos.y <= btnVolver.y + btnVolver.height) {
                        
                        // --- SOLUCIÓN DE LÓGICA DE RETROCESO ---
                        if (*esDosJugadores) {
                            *currentScreen = SCREEN_MODE_SELECT;       // Si es 2P, vuelve directo al selector de modos
                        } else {
                            *currentScreen = SCREEN_DIFFICULTY_SELECT; // Si es 1P, vuelve a la dificultad
                        }
                    }
                    else if (mousePos.x >= btnComenzar.x && mousePos.x <= btnComenzar.x + btnComenzar.width &&
                             mousePos.y >= btnComenzar.y && mousePos.y <= btnComenzar.y + btnComenzar.height) {
                        *currentScreen = SCREEN_LOADING; // ¡Dispara la pantalla de carga!
                    }
                    break;
                }

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
        clickBloqueado = false;
    }
}

void DibujarMenu(GameScreen currentScreen, int selectedDifficulty) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    BeginDrawing();
    ClearBackground((Color){15, 15, 18, 255});

    Vector2 mousePos = GetMousePosition();

    Color grisMetal = (Color){180, 180, 190, 255};
    Color grisOscuro = (Color){40, 40, 45, 255};
    Color grisHover = (Color){65, 65, 75, 255};
    Color amarilloEspada = (Color){245, 220, 40, 255};
    Color textoClaro = (Color){230, 230, 235, 255};

    switch (currentScreen) {
        case SCREEN_MAIN_MENU: {
            int fontSize = 46; 
            int posY = 110;    

            DrawText("ARENA FIGHTER 2D", screenWidth / 2 - MeasureText("ARENA FIGHTER 2D", fontSize) / 2 + 2, posY + 2, fontSize, BLACK);
            DrawText("ARENA FIGHTER 2D", screenWidth / 2 - MeasureText("ARENA FIGHTER 2D", fontSize) / 2, posY, fontSize, amarilloEspada);

            Rectangle btnJugar = {300, 220, 200, 40};
            bool hoverJugar = (mousePos.x >= btnJugar.x && mousePos.x <= btnJugar.x + btnJugar.width &&
                               mousePos.y >= btnJugar.y && mousePos.y <= btnJugar.y + btnJugar.height);
            DrawRectangleRec(btnJugar, hoverJugar ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnJugar.x, (int)btnJugar.y, (int)btnJugar.width, (int)btnJugar.height, hoverJugar ? amarilloEspada : grisMetal);
            DrawText("Jugar", screenWidth / 2 - MeasureText("Jugar", 16) / 2, 232, 16, textoClaro);

            Rectangle btnInstrucciones = {300, 280, 200, 40};
            bool hoverInstrucciones = CheckCollisionPointRec(mousePos, btnInstrucciones);
            DrawRectangleRec(btnInstrucciones, hoverInstrucciones ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnInstrucciones.x, (int)btnInstrucciones.y, (int)btnInstrucciones.width, (int)btnInstrucciones.height, hoverInstrucciones ? amarilloEspada : grisMetal);
            DrawText("Instrucciones", screenWidth / 2 - MeasureText("Instrucciones", 16) / 2, 292, 16, textoClaro);
            break;
        }

        case SCREEN_MODE_SELECT: {
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

        case SCREEN_DIFFICULTY_SELECT: {
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
            DrawText("ELIGE TU JUGADOR", screenWidth/2 - MeasureText("ELIGE TU JUGADOR", 28)/2, 35, 28, amarilloEspada);
            
            DrawText("NOMBRE", 260 - MeasureText("NOMBRE", 16)/2, 95, 16, grisMetal);
            DrawRectangleLines(180, 120, 160, 130, grisMetal); 
            DrawText("<", 150, 170, 24, amarilloEspada);
            DrawText(">", 345, 170, 24, amarilloEspada);

            DrawText("OPONENTE", 540 - MeasureText("OPONENTE", 16)/2, 95, 16, grisMetal);
            DrawRectangleLines(460, 120, 160, 130, grisMetal); 
            DrawText("<", 430, 170, 24, amarilloEspada); 
            DrawText(">", 625, 170, 24, amarilloEspada); 

            Rectangle btnVolver = {50, 400, 80, 30};
            bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
            DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
            DrawText("Volver", 72, 408, 12, textoClaro);

            Rectangle btnComenzar = {670, 400, 80, 30};
            bool hoverComenzar = CheckCollisionPointRec(mousePos, btnComenzar);
            DrawRectangleRec(btnComenzar, hoverComenzar ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnComenzar.x, (int)btnComenzar.y, (int)btnComenzar.width, (int)btnComenzar.height, hoverComenzar ? amarilloEspada : grisMetal);
            DrawText("Jugar", 694, 408, 12, amarilloEspada);
            break;
        }

        case SCREEN_INSTRUCTIONS: {
            DrawText("INSTRUCCIONES", screenWidth / 2 - MeasureText("INSTRUCCIONES", 20) / 2, 35, 20, amarilloEspada);
            
            DrawText("- Movimiento: Usa las flechas o teclas A/D para desplazarte.", 80, 95, 13, textoClaro);
            DrawText("- Ataque básico: Presiona Espacio / Click para golpear con la espada.", 80, 130, 13, textoClaro);
            DrawText("- Sistema de Combate: Gestiona tus hitboxes y tiempos de recuperación.", 80, 165, 13, textoClaro);
            DrawText("- Armas: Las armas aparecen de forma aleatoria en el escenario.", 80, 200, 13, textoClaro);
            DrawText("- Duración: Las armas recolectadas tienen un tiempo de vida limitado.", 80, 235, 13, textoClaro);
            DrawText("- Objetivo: Reduce la barra de vida de tu oponente a cero para ganar.", 80, 270, 13, textoClaro);

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

    EndDrawing();
}