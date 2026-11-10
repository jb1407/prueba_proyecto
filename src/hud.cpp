#include "hud.hpp"

void HUD::Draw(int screenWidth, int screenHeight, float playerHealth, float playerMaxHealth, float enemyHealth, float enemyMaxHealth, Font menuFont) {
    // Proporciones dinámicas basadas en el tamaño de la pantalla
    float boxWidth = screenWidth * 0.30f; // 30% del ancho de la pantalla por cada caja de HUD
    float boxHeight = 65.0f;             // Altura proporcional y cómoda
    float padding = 35.0f;               // Margen de separación desde los bordes de la ventana
    int numSegments = 8;                 // Número de bloques estilo arcade

    // =========================================================================
    // 1. HUD DEL JUGADOR (Esquina Superior Izquierda)
    // =========================================================================
    Rectangle playerBox = { padding, padding, boxWidth, boxHeight };
    
    // Fondo translúcido y marco exterior
    DrawRectangleRec(playerBox, Fade(BLACK, 0.75f));
    DrawRectangleLinesEx(playerBox, 2.5f, WHITE);

    // Etiqueta "PLAYER" ubicada de forma fija a la izquierda sin tocar la barra
    Vector2 playerTextPos = { playerBox.x + 20.0f, playerBox.y + (boxHeight - 22.0f) / 2.0f };
    DrawTextEx(menuFont, "PLAYER", playerTextPos, 22.0f, 2.0f, WHITE);

    // Barra de vida del jugador (calculada dinámicamente al lado del texto)
    float playerBarX = playerBox.x + 130.0f;
    float playerBarY = playerBox.y + (boxHeight - 24.0f) / 2.0f;
    float playerBarWidth = boxWidth - 150.0f;
    float playerBarHeight = 24.0f;
    
    DrawRectangle(playerBarX, playerBarY, playerBarWidth, playerBarHeight, DARKGRAY);
    DrawRectangleLines(playerBarX, playerBarY, playerBarWidth, playerBarHeight, WHITE);

    // Porcentaje y relleno de vida
    float playerHpPercent = playerHealth / playerMaxHealth;
    if (playerHpPercent < 0.0f) playerHpPercent = 0.0f;
    if (playerHpPercent > 1.0f) playerHpPercent = 1.0f;
    
    float playerFilledWidth = playerBarWidth * playerHpPercent;
    DrawRectangle(playerBarX, playerBarY, playerFilledWidth, playerBarHeight, GREEN);

    // Segmentos divisorios retro
    float playerSegWidth = playerBarWidth / numSegments;
    for (int i = 1; i < numSegments; i++) {
        DrawLine(playerBarX + (i * playerSegWidth), playerBarY, playerBarX + (i * playerSegWidth), playerBarY + playerBarHeight, BLACK);
    }

    // =========================================================================
    // 2. HUD DEL ENEMIGO (Esquina Superior Derecha - Simétrico)
    // =========================================================================
    Rectangle enemyBox = { screenWidth - boxWidth - padding, padding, boxWidth, boxHeight };
    
    DrawRectangleRec(enemyBox, Fade(BLACK, 0.75f));
    DrawRectangleLinesEx(enemyBox, 2.5f, WHITE);

    // Etiqueta "ENEMY" ubicada de forma fija al lado derecho de su caja
    Vector2 enemyTextPos = { enemyBox.x + boxWidth - 115.0f, enemyBox.y + (boxHeight - 22.0f) / 2.0f };
    DrawTextEx(menuFont, "ENEMY", enemyTextPos, 22.0f, 2.0f, WHITE);

    // Barra de vida del enemigo ubicada a la izquierda dentro de su propia caja
    float enemyBarX = enemyBox.x + 20.0f;
    float enemyBarY = enemyBox.y + (boxHeight - 24.0f) / 2.0f;
    float enemyBarWidth = boxWidth - 150.0f;
    float enemyBarHeight = 24.0f;
    
    DrawRectangle(enemyBarX, enemyBarY, enemyBarWidth, enemyBarHeight, DARKGRAY);
    DrawRectangleLines(enemyBarX, enemyBarY, enemyBarWidth, enemyBarHeight, WHITE);

    // Porcentaje y relleno de vida del enemigo
    float enemyHpPercent = enemyHealth / enemyMaxHealth;
    if (enemyHpPercent < 0.0f) enemyHpPercent = 0.0f;
    if (enemyHpPercent > 1.0f) enemyHpPercent = 1.0f;

    float enemyFilledWidth = enemyBarWidth * enemyHpPercent;
    DrawRectangle(enemyBarX, enemyBarY, enemyFilledWidth, enemyBarHeight, RED);

    // Segmentos divisorios de la barra del enemigo
    float enemySegWidth = enemyBarWidth / numSegments;
    for (int i = 1; i < numSegments; i++) {
        DrawLine(enemyBarX + (i * enemySegWidth), enemyBarY, enemyBarX + (i * enemySegWidth), enemyBarY + enemyBarHeight, BLACK);
    }
}