#include "raylib.h"
#include "entities/Boat.h"
#include "core/GameCamera.h"
#include "core/World.h"
#include <raymath.h>

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1440, 900, "Dredge Clone - Phase 1 Refactor"); 
    DisableCursor(); 
    ToggleFullscreen();
    SetTargetFPS(60);

    Boat playerBoat;
    GameCamera gameCamera;
    World gameWorld;
    gameWorld.Init();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float time = GetTime();

        // --- LOGICA ---
        playerBoat.Update();
        gameCamera.Update(playerBoat.getPosition());
        gameWorld.Update(deltaTime, time, playerBoat.getPosition());

        FishingSpot* currentSpot = gameWorld.GetNearbySpot(playerBoat.getPosition());

        // --- DIBUJADO ---
        BeginDrawing();
            ClearBackground(BLACK);
            gameWorld.DrawSky();

            BeginMode3D(gameCamera.getCamera());
                gameWorld.Draw();
                playerBoat.Draw();                

            EndMode3D();

            DrawFPS(10, 10);
            
            if (currentSpot != nullptr) {
                DrawText("PRESIONA [E] PARA PESCAR", 
                         GetScreenWidth()/2 - 150, // Centrado horizontal
                         GetScreenHeight() - 100,  // Abajo
                         20, WHITE);
                         
                DrawText("Zona de Pesca Detectada", 10, 60, 20, GREEN);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}