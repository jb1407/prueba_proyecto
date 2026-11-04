#include "raylib.h"
#include "Player.hpp"
#include "enemyAI.hpp"

// Definimos los estados posibles del juego
enum class GameState
{
    PLAYING,
    GAME_OVER
};

int main()
{
    // 1. Inicializar la ventana y la tasa de refresco
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Arena Fighter 2D - Test Integración");
    SetTargetFPS(60);

    // 2. Instanciar al jugador y al bot
    Player player(150.0f, 300.0f);
    EnemyAI enemy(600.0f, 300.0f, Difficulty::MEDIUM);

    // Estado inicial del juego
    GameState currentGameState = GameState::PLAYING;

    // 3. Bucle principal del juego
    while (!WindowShouldClose())
    {

        // --- MÁQUINA DE ESTADOS (LÓGICA) ---
        if (currentGameState == GameState::PLAYING)
        {

            // Actualizar movimiento del jugador
            player.Update();

            // Detectar si el jugador está atacando (ej. presionando ESPACIO)
            bool isPlayerAttacking = IsKeyDown(KEY_SPACE);

            // Actualizar la IA del bot
            enemy.Update(player.getPosition(), isPlayerAttacking, screenWidth, screenHeight);

            // Si el jugador ataca y colisiona con el bot, le bajamos vida
            if (isPlayerAttacking)
            {
                Rectangle playerAttackBox = {player.getX() + 30.0f, player.getY(), 40.0f, 40.0f};
                if (CheckCollisionRecs(playerAttackBox, enemy.GetRectangle()))
                {
                    enemy.TakeDamage(1.0f); // Daño por frame o golpe
                }
            }

            // Si el bot está ejecutando un ataque ligero o pesado
            if (enemy.GetCurrentAction() == AIAction::ATTACK_LIGHT)
            {
                // Rectángulo de golpe rápido y corto, daño menor
                Rectangle lightAttackBox = {enemy.GetRectangle().x - 25.0f, enemy.GetRectangle().y, 30.0f, 30.0f};
                
                // ¡Aquí usamos player.GetRectangle() en lugar de getPosition()!
                if (CheckCollisionRecs(lightAttackBox, player.GetRectangle()))
                {
                    player.TakeDamage(5.0f); // Daño ligero
                }
            }
            else if (enemy.GetCurrentAction() == AIAction::ATTACK_HEAVY)
            {
                // Rectángulo de golpe más amplio y peligroso, daño mayor
                Rectangle heavyAttackBox = {enemy.GetRectangle().x - 50.0f, enemy.GetRectangle().y - 10.0f, 60.0f, 50.0f};
                
                // ¡Aquí también usamos player.GetRectangle()!
                if (CheckCollisionRecs(heavyAttackBox, player.GetRectangle()))
                {
                    player.TakeDamage(15.0f); // Daño pesado
                }
            }

            // Comprobar si alguien se quedó sin vida para pasar a Game Over
            if (player.IsDead() || enemy.IsDead())
            {
                currentGameState = GameState::GAME_OVER;
            }
        }
        else if (currentGameState == GameState::GAME_OVER)
        {
            // Reiniciar partida al presionar la tecla 'R'
            if (IsKeyPressed(KEY_R))
            {
                player = Player(150.0f, 300.0f);
                enemy = EnemyAI(600.0f, 300.0f, Difficulty::MEDIUM);

                currentGameState = GameState::PLAYING; // Regresamos al juego
            }
        }

        // --- RENDERIZADO (DIBUJO) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentGameState == GameState::PLAYING)
        {
            // Dibujar al jugador (Azul) usando su nuevo rectángulo unificado
            DrawRectangleRec(player.GetRectangle(), BLUE);

            // Dibujar al Bot (Rojo) usando su rectángulo
            DrawRectangleRec(enemy.GetRectangle(), RED);

            // --- HUD SUPERIOR ---
            // 1. Barra del Jugador (Esquina superior izquierda)
            float playerHealthPercent = player.GetHealth() / player.GetMaxHealth();
            DrawRectangle(50, 20, 200, 20, RED);                        // Fondo de daño (rojo)
            DrawRectangle(50, 20, (int)(200 * playerHealthPercent), 20, GREEN); // Vida actual (verde)
            DrawRectangleLines(50, 20, 200, 20, BLACK);                 // Borde negro

            // 2. Barra del Bot (Esquina superior derecha)
            float enemyHealthPercent = enemy.GetHealth() / enemy.GetMaxHealth();
            DrawRectangle(550, 20, 200, 20, RED);                       // Fondo de daño
            DrawRectangle(550, 20, (int)(200 * enemyHealthPercent), 20, GREEN); // Vida actual
            DrawRectangleLines(550, 20, 200, 20, BLACK);                // Borde negro

            // Textos informativos de controles
            DrawText("WASD / Flechas: Mover | ESPACIO: Atacar", 20, 50, 16, DARKGRAY);
            DrawText("Jugador = Azul | Bot = Rojo", 20, 75, 14, GRAY);
        }
        else if (currentGameState == GameState::GAME_OVER)
        {
            // Pantalla de Game Over
            DrawText("¡FIN DE LA PARTIDA!", 240, 160, 30, RED);

            if (player.IsDead())
            {
                DrawText("¡Has sido derrotado por el bot!", 260, 210, 18, DARKGRAY);
            }
            else
            {
                DrawText("¡Has derrotado al bot con éxito!", 265, 210, 18, DARKGREEN);
            }

            DrawText("Presiona [R] para reiniciar la partida", 255, 270, 18, DARKBLUE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}