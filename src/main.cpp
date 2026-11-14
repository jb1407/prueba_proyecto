#include "raylib.h"
#include "menu.hpp"
#include "LoadAssets.hpp"
#include "Player.hpp"
#include "enemyAI.hpp"
#include "hud.hpp"

int main() {
    // Usamos la resolución de 1600x900 del sistema de combate
    const int screenWidth = 1600;
    const int screenHeight = 900;
    
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D");
    SetTargetFPS(60);

    GameScreen currentScreen = SCREEN_MAIN_MENU;
    int selectedDifficulty = 0;

    // Variables para la barra de carga simulada
    float loadingProgress = 0.0f;
    const float loadingDuration = 2.0f; 

    // Variables para la cuenta regresiva del combate
    float countdownTimer = 3.5f;
    bool combatStarted = false;

    // --- POSICIONES DINÁMICAS PARA LOS PERSONAJES ---
    float playerStartX = screenWidth * 0.15f;
    float playerStartY = (screenHeight / 2.0f) - 20.0f; 

    float enemyStartX = (screenWidth * 0.85f) - 40.0f; 
    float enemyStartY = (screenHeight / 2.0f) - 20.0f;

    // Inicializar assets y lógica de combate (se inicializan vacíos o listos para usarse tras la carga)
    AssetManager assetManager(15); 
    LoadAssetsToTable(assetManager);
    Font menuFont = GetFontDefault(); 

    // Punteros o instancias que se configurarán al entrar al juego
    Player* player = nullptr;
    EnemyAI* enemy = nullptr;

    while (!WindowShouldClose()) {
        // 1. Lógica según la pantalla actual
        if (currentScreen == SCREEN_LOADING) {
            loadingProgress += GetFrameTime() / loadingDuration;
            
            if (loadingProgress >= 1.0f) {
                loadingProgress = 0.0f;
                currentScreen = SCREEN_GAME_PLAYING;
                
                // Reiniciamos o creamos las instancias de los luchadores al arrancar la partida
                if (player) delete player;
                if (enemy) delete enemy;

                Difficulty diff = (selectedDifficulty == 0) ? Difficulty::EASY : 
                                  (selectedDifficulty == 1) ? Difficulty::MEDIUM : Difficulty::HARD;

                player = new Player(playerStartX, playerStartY);
                enemy = new EnemyAI(enemyStartX, enemyStartY, diff);

                // Reiniciar temporizador de cuenta regresiva
                countdownTimer = 3.5f;
                combatStarted = false;
            }
        } 
        else if (currentScreen == SCREEN_GAME_PLAYING) {
            if (!combatStarted) {
                countdownTimer -= GetFrameTime();
                if (countdownTimer <= 0.0f) {
                    combatStarted = true; // ¡Empieza el combate real!
                }
            } else if (player && enemy) {
                // --- LÓGICA DE COMBATE ACTIVO ---
                player->Update(assetManager);
                enemy->Update(player->getPosition(), IsKeyDown(KEY_SPACE), screenWidth, screenHeight);

                // Colisiones de ataques
                if (player->GetAttackHitbox().CheckCollision(enemy->GetHurtbox())) {
                    enemy->TakeDamage(0.5f); 
                }
                if (enemy->GetAttackHitbox().CheckCollision(player->GetHurtbox())) {
                    player->TakeDamage(2.0f); 
                }
            }
        } 
        else {
            ActualizarMenu(&currentScreen, &selectedDifficulty);
        }

        // 2. Renderizado general
        BeginDrawing();
        ClearBackground((Color){15, 15, 18, 255});

        if (currentScreen == SCREEN_LOADING) {
            // Pantalla de carga simulada
            const char* textoCarga = "CARGANDO PARTIDA...";
            DrawText(textoCarga, screenWidth / 2 - MeasureText(textoCarga, 28) / 2, screenHeight / 2 - 40, 28, (Color){230, 230, 235, 255});

            float barWidth = 500.0f;
            float barHeight = 25.0f;
            float barX = (screenWidth - barWidth) / 2;
            float barY = screenHeight / 2 + 20;

            DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, (Color){180, 180, 190, 255});
            float fillWidth = barWidth * loadingProgress;
            DrawRectangle((int)barX + 2, (int)barY + 2, (int)(fillWidth - 4), (int)(barHeight - 4), (Color){245, 220, 40, 255});
        } 
        else if (currentScreen == SCREEN_GAME_PLAYING) {
            // --- ESCENARIO Y COMBATE ---
            ClearBackground(GRAY);

            DrawText("SISTEMA DE COMBATE ACTIVO", 10, 10, 20, YELLOW);
            DrawText("WASD/Flechas: Mover | SPACE: Atacar | U: Esquivar", 10, 35, 16, LIGHTGRAY);

            if (player && enemy) {
                // Dibujar elementos del juego
                enemy->Draw(assetManager, player->getPosition());

                DrawRectangleRec(player->GetHurtbox(), Fade(BLUE, 0.4f));
                DrawRectangleRec(enemy->GetHurtbox(), Fade(BLUE, 0.4f));

                player->GetAttackHitbox().Draw(RED);
                enemy->GetAttackHitbox().Draw(RED);

                HUD::Draw(
                    screenWidth, screenHeight, 
                    player->GetHealth(), player->GetMaxHealth(), 
                    enemy->GetHealth(), enemy->GetMaxHealth(), 
                    menuFont
                );
            }

            // Cuenta regresiva central (3, 2, 1, ¡PELEA!)
            if (!combatStarted) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f)); // Oscurecer un poco el fondo
                int numero = (int)countdownTimer;
                const char* textoCuenta = "";
                int fontSize = 100;

                if (numero >= 1 && numero <= 3) {
                    textoCuenta = TextFormat("%d", numero);
                } else if (numero <= 0) {
                    textoCuenta = "¡PELEA!";
                    fontSize = 80;
                }

                int textWidth = MeasureText(textoCuenta, fontSize);
                DrawText(textoCuenta, screenWidth / 2 - textWidth / 2 + 4, screenHeight / 2 - 50 + 4, fontSize, BLACK);
                DrawText(textoCuenta, screenWidth / 2 - textWidth / 2, screenHeight / 2 - 50, fontSize, (Color){245, 220, 40, 255});
            }
        } 
        else {
            // Renderizar pantallas del menú
            EndDrawing();
            continue; 
        }

        EndDrawing();
    }

    // Limpieza de memoria
    if (player) delete player;
    if (enemy) delete enemy;

    CloseWindow();
    return 0;
}