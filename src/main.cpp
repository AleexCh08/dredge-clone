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

bool gameRunning = true;

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

    while (!WindowShouldClose() && gameRunning) {
        float deltaTime = GetFrameTime();
        float time = GetTime();

        // --- LOGICA ---
        switch (currentState) {
            case STATE_NAVIGATING:
            {
                playerBoat.Update(true);
                Port* port = gameWorld.GetPort();
                    if (port->CheckCollision(playerBoat.getPosition(), 2.0f)) {
                        Vector3 dir = Vector3Normalize(Vector3Subtract(playerBoat.getPosition(), port->GetPosition()));
                        Vector3 newPos = playerBoat.getPosition();
                        newPos.x += dir.x * 0.5f; // Empujón
                        newPos.z += dir.z * 0.5f;
                        playerBoat.BounceBack(dir); 
                    }

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
                    HideCursor(); 
                }
                if (IsKeyPressed(KEY_ESCAPE)) { // Salir del juego
                    gameRunning = false; 
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
                        FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                        if (spot) {
                            bool success = playerBoat.CaptureFish(spot->GetFishType());
                            if (success) {
                                spot->Deactivate();
                            }
                        } 
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
        
        if (currentState != STATE_INVENTORY && currentState != STATE_FISHING) {
            gameCamera.Update(playerBoat.getPosition());
        }
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
            playerBoat.DrawUI(gameCamera.getCamera());
            
            if (currentState == STATE_NAVIGATING) {
                FishingSpot* spot = gameWorld.GetNearbySpot(playerBoat.getPosition());
                if (spot) DrawText("E - PESCAR", 600, 600, 20, WHITE);
                if (gameWorld.GetPort()->IsPlayerInside(playerBoat.getPosition())) { // Detectar puerto
                    DrawText("ESTAS EN EL PUERTO", 400, 100, 20, GREEN);
                }
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