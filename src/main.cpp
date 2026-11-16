#include "raylib.h"
#include "menu.hpp"
#include "LoadAssets.hpp"
#include "Player.hpp"
#include "enemyAI.hpp"
#include "hud.hpp"

// Estado del juego para controlar el combate y el fin de partida
enum GameState {
    PLAYING,
    VICTORY,
    GAME_OVER
};

int main() {
    // Usamos la resolución de 1600x900 para que el combate y el menú luzcan perfectamente adaptados
    const int screenWidth = 1600;
    const int screenHeight = 900;
    
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D");
    SetTargetFPS(60);

    GameScreen currentScreen = SCREEN_MAIN_MENU;
    int selectedDifficulty = 0;
    bool esDosJugadores = false; // Control de flujo para 1P o 2P

    // Variables para la barra de carga simulada
    float loadingProgress = 0.0f;
    const float loadingDuration = 2.0f;

    // --- POSICIONES DINÁMICAS PARA EL COMBATE ---
    float playerStartX = screenWidth * 0.15f;
    float playerStartY = (screenHeight / 2.0f) - 20.0f; 

    float enemyStartX = (screenWidth * 0.85f) - 40.0f; 
    float enemyStartY = (screenHeight / 2.0f) - 20.0f;

    // Cargar assets y fuentes del sistema
    AssetManager assetManager(15); 
    LoadAssetsToTable(assetManager);
    Font menuFont = GetFontDefault(); 

    // Punteros para los personajes de combate (se inicializan al terminar la carga)
    Player* player = nullptr;
    EnemyAI* enemy = nullptr;
    GameState gameState = PLAYING;

    while (!WindowShouldClose()) {
        // ==========================================
        // 1. ACTUALIZACIÓN LÓGICA SEGÚN LA PANTALLA
        // ==========================================
        if (currentScreen == SCREEN_LOADING) {
            loadingProgress += GetFrameTime() / loadingDuration;
            
            if (loadingProgress >= 1.0f) {
                loadingProgress = 0.0f;
                currentScreen = SCREEN_GAME_PLAYING;
                
                // Limpiar instancias previas si existían para evitar fugas de memoria
                if (player) delete player;
                if (enemy) delete enemy;

                // Mapear la dificultad seleccionada en el menú a la IA
                Difficulty diff = (selectedDifficulty == 0) ? Difficulty::EASY : 
                                  (selectedDifficulty == 1) ? Difficulty::MEDIUM : Difficulty::HARD;

                // Instanciar al jugador y al enemigo con las posiciones y dificultad correctas
                player = new Player(playerStartX, playerStartY);
                enemy = new EnemyAI(enemyStartX, enemyStartY, diff);
                gameState = PLAYING; // Asegurar estado inicial de combate
            }
        } 
        else if (currentScreen == SCREEN_GAME_PLAYING) {
            // Lógica de combate activo
            if (gameState == PLAYING && player && enemy) {
                player->Update(assetManager, screenWidth, screenHeight);
                enemy->Update(player->getPosition(), IsKeyDown(KEY_SPACE), screenWidth, screenHeight);

                if (player->GetAttackHitbox().CheckCollision(enemy->GetHurtbox().GetRect())) {
                    enemy->TakeDamage(0.5f); 
                }
                if (enemy->GetAttackHitbox().CheckCollision(player->GetHurtbox().GetRect())) {
                    player->TakeDamage(2.0f); 
                }

                // Comprobar condiciones de fin de partida
                if (player->IsDead()) {
                    gameState = GAME_OVER;
                } else if (enemy->IsDead()) {
                    gameState = VICTORY;
                }
            }
            // Si el combate terminó, permitir volver al menú principal con la tecla ESC
            else if (gameState != PLAYING) {
                if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = SCREEN_MAIN_MENU;
                    gameState = PLAYING;
                }
            }
        } 
        else {
            // Actualizar lógica del menú (con el flujo de 1 o 2 jugadores que corregimos)
            ActualizarMenu(&currentScreen, &selectedDifficulty, &esDosJugadores);
        }

        // ==========================================
        // 2. RENDERIZADO (DIBUJADO EN PANTALLA)
        // ==========================================
        if (currentScreen == SCREEN_LOADING) {
            BeginDrawing();
            ClearBackground((Color){15, 15, 18, 255});

            const char* textoCarga = "CARGANDO PARTIDA...";
            DrawText(textoCarga, screenWidth / 2 - MeasureText(textoCarga, 28) / 2, screenHeight / 2 - 40, 28, (Color){230, 230, 235, 255});

            float barWidth = 500.0f;
            float barHeight = 25.0f;
            float barX = (screenWidth - barWidth) / 2;
            float barY = screenHeight / 2 + 20;

            DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, (Color){180, 180, 190, 255});
            float fillWidth = barWidth * loadingProgress;
            DrawRectangle((int)barX + 2, (int)barY + 2, (int)(fillWidth - 4), (int)(barHeight - 4), (Color){245, 220, 40, 255});

            EndDrawing();
        } 
        else if (currentScreen == SCREEN_GAME_PLAYING) {
            BeginDrawing();
            ClearBackground(GRAY);

            // Elementos estéticos y controles del escenario de combate
            DrawText("SISTEMA DE COMBATE ACTIVO", 10, 10, 20, YELLOW);
            DrawText("WASD/Flechas: Mover Jugador | SPACE: Atacar (Punch) | U: Esquivar (Backward)", 10, 35, 16, LIGHTGRAY);

            if (player && enemy) {
                // Dibujar al enemigo, hitboxes, hurtboxes y HUD de vida
                enemy->Draw(assetManager, player->getPosition());

                /*DrawRectangleRec(player->GetHurtbox().GetRect(), Fade(BLUE, 0.4f));
                DrawRectangleRec(enemy->GetHurtbox().GetRect(), Fade(BLUE, 0.4f));

                player->GetAttackHitbox().Draw(RED);
                enemy->GetAttackHitbox().Draw(RED);
                */                

                HUD::Draw(
                    screenWidth, screenHeight, 
                    player->GetHealth(), player->GetMaxHealth(), 
                    enemy->GetHealth(), enemy->GetMaxHealth(), 
                    menuFont
                );
            }

            // Pantallas de fin de partida (Game Over / Victoria)
            if (gameState == GAME_OVER) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
                DrawText("¡GAME OVER!", screenWidth / 2.0f - MeasureText("¡GAME OVER!", 60) / 2.0f, screenHeight / 2.0f - 60, 60, RED);
                DrawText("Has sido derrotado. Presiona ESC para volver al menú.", screenWidth / 2.0f - MeasureText("Has sido derrotado. Presiona ESC para volver al menú.", 20) / 2.0f, screenHeight / 2.0f + 20, 20, LIGHTGRAY);
            } 
            else if (gameState == VICTORY) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
                DrawText("¡VICTORIA!", screenWidth / 2.0f - MeasureText("¡VICTORIA!", 60) / 2.0f, screenHeight / 2.0f - 60, 60, GREEN);
                DrawText("¡Has vencido al enemigo! Presiona ESC para volver al menú.", screenWidth / 2.0f - MeasureText("¡Has vencido al enemigo! Presiona ESC para volver al menú.", 20) / 2.0f, screenHeight / 2.0f + 20, 20, LIGHTGRAY);
            }

            EndDrawing();
        } 
        else {
            // Renderizar las pantallas del menú (manejan su propio ciclo de dibujo de manera independiente)
            DibujarMenu(currentScreen, selectedDifficulty);
        }
    }

    // Limpieza de memoria al cerrar la aplicación
    if (player) delete player;
    if (enemy) delete enemy;

    CloseWindow();
    return 0;
}