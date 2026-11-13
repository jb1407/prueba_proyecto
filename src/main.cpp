#include "raylib.h"
#include "LoadAssets.hpp"
#include "Player.hpp"
#include "enemyAI.hpp"
#include "hud.hpp"

// Estado del juego para controlar el fin de partida
enum GameState {
    PLAYING,
    VICTORY,
    GAME_OVER
};

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    // --- POSICIONES DINÁMICAS ---
    float playerStartX = screenWidth * 0.15f;
    float playerStartY = (screenHeight / 2.0f) - 20.0f; 

    float enemyStartX = (screenWidth * 0.85f) - 40.0f; 
    float enemyStartY = (screenHeight / 2.0f) - 20.0f;

    InitWindow(screenWidth, screenHeight, "Prueba Aislada - Combate, Hitboxes y HUD");
    SetTargetFPS(60);

    // 1. INICIALIZAR Y CARGAR LOS ASSETS
    AssetManager assetManager(15); 
    LoadAssetsToTable(assetManager);

    Font menuFont = GetFontDefault(); 

    // Instancias de los personajes
    Player player(playerStartX, playerStartY);
    EnemyAI enemy(enemyStartX, enemyStartY, Difficulty::HARD);

    GameState gameState = PLAYING; // Estado inicial

    while (!WindowShouldClose()) {
        
        // --- 2. LÓGICA SEGÚN EL ESTADO DEL JUEGO ---
        if (gameState == PLAYING) {
            // Actualización de movimientos
            player.Update(assetManager);
            enemy.Update(player.getPosition(), IsKeyDown(KEY_SPACE), screenWidth, screenHeight);

            // Detección de colisiones (Hitboxes vs Hurtbox)
            if (player.GetAttackHitbox().CheckCollision(enemy.GetHurtbox())) {
                enemy.TakeDamage(0.5f); 
            }

            if (enemy.GetAttackHitbox().CheckCollision(player.GetHurtbox())) {
                player.TakeDamage(2.0f); 
            }

            // Comprobar condiciones de fin de partida
            if (player.IsDead()) {
                gameState = GAME_OVER;
            } else if (enemy.IsDead()) {
                gameState = VICTORY;
            }
        }

        // --- 3. RENDERIZADO ---
        BeginDrawing();
        ClearBackground(GRAY);

        // Dibujar elementos estéticos del escenario de combate
        DrawText("SISTEMA DE COMBATE ACTIVO", 10, 10, 20, YELLOW);
        DrawText("WASD/Flechas: Mover Jugador | SPACE: Atacar (Punch) | U: Esquivar (Backward)", 10, 35, 16, LIGHTGRAY);

        // Dibujar al enemigo y HUD de vida
        enemy.Draw(assetManager, player.getPosition());

        // 1. Hurtboxes (Cuerpos) - Los dibujamos en Azul semitransparente
        DrawRectangleRec(player.GetHurtbox(), Fade(BLUE, 0.4f));
        DrawRectangleRec(enemy.GetHurtbox(), Fade(BLUE, 0.4f));

        // 2. Attack Hitboxes (Cajas de ataque) - Las dibujamos con su método original de la clase Hitbox 
        // (asumiendo que GetAttackHitbox() sí te devuelve el objeto Hitbox completo)
        player.GetAttackHitbox().Draw(RED);
        enemy.GetAttackHitbox().Draw(RED);

        HUD::Draw(
            screenWidth, 
            screenHeight, 
            player.GetHealth(), 
            player.GetMaxHealth(), 
            enemy.GetHealth(), 
            enemy.GetMaxHealth(), 
            menuFont
        );

        // --- 4. PANTALLAS DE FIN DE PARTIDA ---
        if (gameState == GAME_OVER) {
            // Capa semitransparente para oscurecer el fondo
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
            DrawText("¡GAME OVER!", screenWidth / 2.0f - MeasureText("¡GAME OVER!", 60) / 2.0f, screenHeight / 2.0f - 60, 60, RED);
            DrawText("Has sido derrotado. Presiona ESC para salir.", screenWidth / 2.0f - MeasureText("Has sido derrotado. Presiona ESC para salir.", 20) / 2.0f, screenHeight / 2.0f + 20, 20, LIGHTGRAY);
        } 
        else if (gameState == VICTORY) {
            // Capa semitransparente
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
            DrawText("¡VICTORIA!", screenWidth / 2.0f - MeasureText("¡VICTORIA!", 60) / 2.0f, screenHeight / 2.0f - 60, 60, GREEN);
            DrawText("¡Has vencido al enemigo! Presiona ESC para salir.", screenWidth / 2.0f - MeasureText("¡Has vencido al enemigo! Presiona ESC para salir.", 20) / 2.0f, screenHeight / 2.0f + 20, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}