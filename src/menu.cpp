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
                    // Botón Jugar (600, 440, 400, 80)
                    if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 440 && mousePos.y <= 520) {
                        *currentScreen = SCREEN_MODE_SELECT;
                    }
                    // Botón Instrucciones (600, 560, 400, 80)
                    else if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 560 && mousePos.y <= 640) {
                        *currentScreen = SCREEN_INSTRUCTIONS;
                    }
                    break;

                case SCREEN_MODE_SELECT:
                    // Modo 1 Jugador (600, 360, 400, 80)
                    if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 360 && mousePos.y <= 440) {
                        *esDosJugadores = false;                    
                        *currentScreen = SCREEN_DIFFICULTY_SELECT;  
                    }
                    // Modo 2 Jugadores (600, 480, 400, 80)
                    else if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 480 && mousePos.y <= 560) {
                        *esDosJugadores = true;                     
                        *currentScreen = SCREEN_CHARACTER_SELECT;   
                    }
                    // Botón Volver (100, 800, 160, 60)
                    else if (mousePos.x >= 100 && mousePos.x <= 260 && mousePos.y >= 800 && mousePos.y <= 860) {
                        *currentScreen = SCREEN_MAIN_MENU;
                    }
                    break;

                case SCREEN_DIFFICULTY_SELECT:
                    // Fácil (600, 320, 400, 80)
                    if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 320 && mousePos.y <= 400) { 
                        *selectedDifficulty = 0; 
                        *currentScreen = SCREEN_CHARACTER_SELECT; 
                    }
                    // Medio (600, 440, 400, 80)
                    else if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 440 && mousePos.y <= 520) { 
                        *selectedDifficulty = 1; 
                        *currentScreen = SCREEN_CHARACTER_SELECT; 
                    }
                    // Difícil (600, 560, 400, 80)
                    else if (mousePos.x >= 600 && mousePos.x <= 1000 && mousePos.y >= 560 && mousePos.y <= 640) { 
                        *selectedDifficulty = 2; 
                        *currentScreen = SCREEN_CHARACTER_SELECT; 
                    }
                    // Volver (100, 800, 160, 60)
                    else if (mousePos.x >= 100 && mousePos.x <= 260 && mousePos.y >= 800 && mousePos.y <= 860) { 
                        *currentScreen = SCREEN_MODE_SELECT; 
                    }
                    break;

                case SCREEN_CHARACTER_SELECT: {
                    Rectangle btnVolver = {100, 800, 160, 60};
                    Rectangle btnComenzar = {1340, 800, 160, 60};

                    if (mousePos.x >= btnVolver.x && mousePos.x <= btnVolver.x + btnVolver.width &&
                        mousePos.y >= btnVolver.y && mousePos.y <= btnVolver.y + btnVolver.height) {
                        
                        if (*esDosJugadores) {
                            *currentScreen = SCREEN_MODE_SELECT;       
                        } else {
                            *currentScreen = SCREEN_DIFFICULTY_SELECT; 
                        }
                    }
                    else if (mousePos.x >= btnComenzar.x && mousePos.x <= btnComenzar.x + btnComenzar.width &&
                             mousePos.y >= btnComenzar.y && mousePos.y <= btnComenzar.y + btnComenzar.height) {
                        *currentScreen = SCREEN_LOADING; 
                    }
                    break;
                }

                case SCREEN_INSTRUCTIONS:
                    if (mousePos.x >= 100 && mousePos.x <= 260 && mousePos.y >= 800 && mousePos.y <= 860) {
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
    const int screenWidth = 1600;
    const int screenHeight = 900;

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
            int fontSize = 92; 
            int posY = 220;    

            DrawText("ARENA FIGHTER 2D", screenWidth / 2 - MeasureText("ARENA FIGHTER 2D", fontSize) / 2 + 4, posY + 4, fontSize, BLACK);
            DrawText("ARENA FIGHTER 2D", screenWidth / 2 - MeasureText("ARENA FIGHTER 2D", fontSize) / 2, posY, fontSize, amarilloEspada);

            Rectangle btnJugar = {600, 440, 400, 80};
            bool hoverJugar = CheckCollisionPointRec(mousePos, btnJugar);
            DrawRectangleRec(btnJugar, hoverJugar ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnJugar.x, (int)btnJugar.y, (int)btnJugar.width, (int)btnJugar.height, hoverJugar ? amarilloEspada : grisMetal);
            DrawText("Jugar", screenWidth / 2 - MeasureText("Jugar", 32) / 2, 464, 32, textoClaro);

            Rectangle btnInstrucciones = {600, 560, 400, 80};
            bool hoverInstrucciones = CheckCollisionPointRec(mousePos, btnInstrucciones);
            DrawRectangleRec(btnInstrucciones, hoverInstrucciones ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnInstrucciones.x, (int)btnInstrucciones.y, (int)btnInstrucciones.width, (int)btnInstrucciones.height, hoverInstrucciones ? amarilloEspada : grisMetal);
            DrawText("Instrucciones", screenWidth / 2 - MeasureText("Instrucciones", 32) / 2, 584, 32, textoClaro);
            break;
        }

        case SCREEN_MODE_SELECT: {
            DrawText("SELECCIONA MODO", screenWidth / 2 - MeasureText("SELECCIONA MODO", 44) / 2, 160, 44, amarilloEspada);

            Rectangle btnModo1 = {600, 360, 400, 80};
            bool hoverModo1 = CheckCollisionPointRec(mousePos, btnModo1);
            DrawRectangleRec(btnModo1, hoverModo1 ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnModo1.x, (int)btnModo1.y, (int)btnModo1.width, (int)btnModo1.height, hoverModo1 ? amarilloEspada : grisMetal);
            DrawText("1 Jugador (vs Bot)", screenWidth / 2 - MeasureText("1 Jugador (vs Bot)", 28) / 2, 386, 28, textoClaro);

            Rectangle btnModo2 = {600, 480, 400, 80};
            bool hoverModo2 = CheckCollisionPointRec(mousePos, btnModo2);
            DrawRectangleRec(btnModo2, hoverModo2 ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnModo2.x, (int)btnModo2.y, (int)btnModo2.width, (int)btnModo2.height, hoverModo2 ? amarilloEspada : grisMetal);
            DrawText("2 Jugadores (Local)", screenWidth / 2 - MeasureText("2 Jugadores (Local)", 28) / 2, 506, 28, textoClaro);

            Rectangle btnVolver = {100, 800, 160, 60};
            bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
            DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
            DrawText("Volver", 136, 818, 22, textoClaro);
            break;
        }

        case SCREEN_DIFFICULTY_SELECT: {
            DrawText("ELIGE LA DIFICULTAD", screenWidth / 2 - MeasureText("ELIGE LA DIFICULTAD", 40) / 2, 160, 40, amarilloEspada);

            Rectangle btnFacil = {600, 320, 400, 80};
            bool hoverFacil = CheckCollisionPointRec(mousePos, btnFacil);
            DrawRectangleRec(btnFacil, hoverFacil ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnFacil.x, (int)btnFacil.y, (int)btnFacil.width, (int)btnFacil.height, hoverFacil ? amarilloEspada : grisMetal);
            DrawText("Facil", screenWidth / 2 - MeasureText("Facil", 28) / 2, 346, 28, textoClaro);

            Rectangle btnMedio = {600, 440, 400, 80};
            bool hoverMedio = CheckCollisionPointRec(mousePos, btnMedio);
            DrawRectangleRec(btnMedio, hoverMedio ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnMedio.x, (int)btnMedio.y, (int)btnMedio.width, (int)btnMedio.height, hoverMedio ? amarilloEspada : grisMetal);
            DrawText("Medio", screenWidth / 2 - MeasureText("Medio", 28) / 2, 466, 28, textoClaro);

            Rectangle btnDificil = {600, 560, 400, 80};
            bool hoverDificil = CheckCollisionPointRec(mousePos, btnDificil);
            DrawRectangleRec(btnDificil, hoverDificil ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnDificil.x, (int)btnDificil.y, (int)btnDificil.width, (int)btnDificil.height, hoverDificil ? amarilloEspada : grisMetal);
            DrawText("Dificil", screenWidth / 2 - MeasureText("Dificil", 28) / 2, 586, 28, textoClaro);

            Rectangle btnVolver = {100, 800, 160, 60};
            bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
            DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
            DrawText("Volver", 136, 818, 22, textoClaro);
            break;
        }

        case SCREEN_CHARACTER_SELECT: {
            DrawText("ELIGE TU JUGADOR", screenWidth/2 - MeasureText("ELIGE TU JUGADOR", 56)/2, 70, 56, amarilloEspada);
            
            // JUGADOR 1
            DrawText("NOMBRE", 520 - MeasureText("NOMBRE", 32)/2, 190, 32, grisMetal);
            DrawRectangleLines(360, 240, 320, 260, grisMetal); 
            DrawText("<", 300, 340, 48, amarilloEspada);
            DrawText(">", 700, 340, 48, amarilloEspada);

            // OPONENTE
            DrawText("OPONENTE", 1080 - MeasureText("OPONENTE", 32)/2, 190, 32, grisMetal);
            DrawRectangleLines(920, 240, 320, 260, grisMetal); 
            DrawText("<", 860, 340, 48, amarilloEspada); 
            DrawText(">", 1260, 340, 48, amarilloEspada); 

            Rectangle btnVolver = {100, 800, 160, 60};
            bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
            DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
            DrawText("Volver", 136, 818, 22, textoClaro);

            Rectangle btnComenzar = {1340, 800, 160, 60};
            bool hoverComenzar = CheckCollisionPointRec(mousePos, btnComenzar);
            DrawRectangleRec(btnComenzar, hoverComenzar ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnComenzar.x, (int)btnComenzar.y, (int)btnComenzar.width, (int)btnComenzar.height, hoverComenzar ? amarilloEspada : grisMetal);
            DrawText("Jugar", 1388, 818, 22, amarilloEspada);
            break;
        }

        case SCREEN_INSTRUCTIONS: {
            DrawText("INSTRUCCIONES", screenWidth / 2 - MeasureText("INSTRUCCIONES", 40) / 2, 70, 40, amarilloEspada);
            
            int startX = 160;
            int startY = 190;
            int spacing = 70;
            int fontSizeText = 26;

            DrawText("- Movimiento: Usa las flechas o teclas A/D para desplazarte.", startX, startY, fontSizeText, textoClaro);
            DrawText("- Ataque básico: Presiona Espacio / Click para golpear con la espada.", startX, startY + spacing, fontSizeText, textoClaro);
            DrawText("- Sistema de Combate: Gestiona tus hitboxes y tiempos de recuperación.", startX, startY + spacing * 2, fontSizeText, textoClaro);
            DrawText("- Armas: Las armas aparecen de forma aleatoria en el escenario.", startX, startY + spacing * 3, fontSizeText, textoClaro);
            DrawText("- Duración: Las armas recolectadas tienen un tiempo de vida limitado.", startX, startY + spacing * 4, fontSizeText, textoClaro);
            DrawText("- Objetivo: Reduce la barra de vida de tu oponente a cero para ganar.", startX, startY + spacing * 5, fontSizeText, textoClaro);

            Rectangle btnVolver = {100, 800, 160, 60};
            bool hoverVolver = CheckCollisionPointRec(mousePos, btnVolver);
            DrawRectangleRec(btnVolver, hoverVolver ? grisHover : grisOscuro);
            DrawRectangleLines((int)btnVolver.x, (int)btnVolver.y, (int)btnVolver.width, (int)btnVolver.height, hoverVolver ? amarilloEspada : grisMetal);
            DrawText("Volver", 136, 818, 22, textoClaro);
            break;
        }

        default:
            break;
    }

    EndDrawing();
}