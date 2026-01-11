#include "raylib.h"
#include "entities/Boat.h"
#include "core/GameCamera.h"
#include "core/FishingMinigame.h"
#include "core/World.h"
#include <raymath.h>

enum GameState {
    STATE_NAVIGATING,
    STATE_FISHING,
    STATE_INVENTORY
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

    playerBoat.GetInventory()->AddItem("Motor V8", 2, 2, GRAY);
    playerBoat.GetInventory()->AddItem("Bacalao", 1, 2, ORANGE);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        float time = GetTime();

        // --- LOGICA ---
        switch (currentState) {
            case STATE_NAVIGATING:
            {
                playerBoat.Update(true);
                FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                if (spot != nullptr && IsKeyPressed(KEY_E)) { // Pescar
                    currentState = STATE_FISHING;
                    minigame.Start();
                    playerBoat.StartFishing(spot->GetPosition()); 
                }
                if (IsKeyPressed(KEY_I)) { // Inventario
                    currentState = STATE_INVENTORY;
                    playerBoat.GetInventory()->Toggle();
                    EnableCursor(); 
                }
                if (IsKeyPressed(KEY_ESCAPE)) { // Salir del juego
                    CloseWindow(); 
                }
            } break;

            case STATE_FISHING:
            {
                playerBoat.Update(false);
                minigame.Update();

                if (!minigame.IsActive()) {
                    currentState = STATE_NAVIGATING;
                    playerBoat.StopFishing();
                    if (minigame.DidWin()) {
                        playerBoat.AddFish();
                        FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                        if (spot) spot->Deactivate(); 
                    } else {
                        playerBoat.FailFishing(); 
                    }
                }
            } break;

            case STATE_INVENTORY: 
            {              
                playerBoat.GetInventory()->Update();

                if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_ESCAPE)) {
                    currentState = STATE_NAVIGATING;
                    playerBoat.GetInventory()->Toggle(); 
                    DisableCursor();
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
                gameWorld.Draw(playerBoat.getPosition());
                playerBoat.Draw();                
            EndMode3D();

            DrawFPS(10, 10);
            DrawText(TextFormat("PECES: %d", playerBoat.GetFishCount()), 100, 10, 30, GOLD);
            playerBoat.DrawUI(gameCamera.getCamera());
            
            if (currentState == STATE_NAVIGATING) {
                 FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                 if (spot) DrawText("E - PESCAR", 600, 600, 20, WHITE);
            }
            else if (currentState == STATE_FISHING) {
                minigame.Draw();
            }
            else if (currentState == STATE_INVENTORY) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
                playerBoat.GetInventory()->Draw(); 
            }
            
        EndDrawing();
    }

    CloseWindow();
    return 0;
}