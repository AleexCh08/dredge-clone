#include "raylib.h"
#include "entities/Boat.h"
#include "core/GameCamera.h"
#include "core/FishingMinigame.h"
#include "core/World.h"
#include <raymath.h>

enum GameState {
    STATE_NAVIGATING,
    STATE_FISHING
};

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1440, 900, "Dredge Clone - Phase 1 Refactor"); 
    SetExitKey(KEY_NULL);
    DisableCursor(); 
    ToggleFullscreen();
    SetTargetFPS(60);

    Boat playerBoat;
    GameCamera gameCamera;
    World gameWorld;
    gameWorld.Init();
    FishingMinigame minigame;
    GameState currentState = STATE_NAVIGATING;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float time = GetTime();

        // --- LOGICA ---
        switch (currentState) {
            case STATE_NAVIGATING:
            {
                playerBoat.Update(true);
                FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                if (spot != nullptr && IsKeyPressed(KEY_E)) {
                    currentState = STATE_FISHING;
                    minigame.Start(); 
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    CloseWindow(); 
                }
            } break;

            case STATE_FISHING:
            {
                playerBoat.Update(false);
                minigame.Update();
                if (!minigame.IsActive()) {
                    currentState = STATE_NAVIGATING;
                }
            } break;
        }
        
        gameCamera.Update(playerBoat.getPosition());
        gameWorld.Update(deltaTime, time, playerBoat.getPosition());

        // --- DIBUJADO ---
        BeginDrawing();
            ClearBackground(BLACK);
            gameWorld.DrawSky();

            BeginMode3D(gameCamera.getCamera());
                gameWorld.Draw();
                playerBoat.Draw();                

            EndMode3D();

            DrawFPS(10, 10);
            
            if (currentState == STATE_NAVIGATING) {
                 FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                 if (spot) DrawText("E - PESCAR", 600, 600, 20, WHITE);
            }
            else if (currentState == STATE_FISHING) {
                minigame.Draw();
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}