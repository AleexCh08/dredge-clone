#include "raylib.h"
#include "entities/Boat.h"
#include "core/GameCamera.h"
#include "core/FishingMinigame.h"
#include "core/World.h"
#include <raymath.h>

enum GameState {
    STATE_NAVIGATING,
    STATE_FISHING,
    STATE_INVENTORY,
    STATE_DOCKED,
    STATE_STORAGE
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
                playerBoat.ResolvePortCollision(gameWorld.GetPort());
                
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
                if (gameWorld.GetPort()->IsPlayerInsideDock(playerBoat.getPosition())) {  // Entrar al muelle            
                    if (IsKeyPressed(KEY_E)) {
                        currentState = STATE_DOCKED;
                        EnableCursor(); 
                        HideCursor(); 
                    }
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

            case STATE_DOCKED:
            {
                if (IsKeyPressed(KEY_ONE)) {
                    currentState = STATE_STORAGE;
                    if (!playerBoat.GetInventory()->IsOpen()) playerBoat.GetInventory()->Toggle();
                    gameWorld.GetPort()->GetStorage()->Toggle(); 
                }
                
                if (IsKeyPressed(KEY_TWO)) {
                    // Pescadero (Futura fase)
                }

                if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
                    currentState = STATE_NAVIGATING;
                    DisableCursor(); 
                }
            } break;

            case STATE_STORAGE:
            {
                Inventory* boatInv = playerBoat.GetInventory();
                Inventory* portInv = gameWorld.GetPort()->GetStorage();

                int boatX = 100; 
                int boatY = 200;
                int portX = 800; 
                int portY = 200;

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    int itemIdx = boatInv->GetItemIndexUnderMouse(boatX, boatY);
                    if (itemIdx != -1) {
                        InventoryItem item = boatInv->GetItem(itemIdx);
                        if (portInv->AddItem(item.name.c_str(), item.width, item.height, item.color)) {
                            boatInv->RemoveItem(itemIdx);
                        }
                    }

                    int itemIdxPort = portInv->GetItemIndexUnderMouse(portX, portY);
                    if (itemIdxPort != -1) {
                        InventoryItem item = portInv->GetItem(itemIdxPort);
                        if (boatInv->AddItem(item.name.c_str(), item.width, item.height, item.color)) {
                            portInv->RemoveItem(itemIdxPort);
                        }
                    }
                }

                if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_ESCAPE)) {
                    currentState = STATE_DOCKED;
                    if (boatInv->IsOpen()) boatInv->Toggle();
                    if (portInv->IsOpen()) portInv->Toggle(); 
                }
            } break;
        }
        
        if (currentState != STATE_INVENTORY && currentState != STATE_FISHING 
            && currentState != STATE_DOCKED && currentState != STATE_STORAGE) 
        {
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
                    DrawText("ESTAS EN EL PUERTO", 600, 200, 20, GREEN);
                }
                if (gameWorld.GetPort()->IsPlayerInsideDock(playerBoat.getPosition())) { // Muelle
                    DrawText("PRESIONA 'E' PARA ATRACAR", 500, 100, 20, WHITE);
                }
            }
            else if (currentState == STATE_FISHING) {
                minigame.Draw();
            }
            else if (currentState == STATE_INVENTORY) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
                playerBoat.GetInventory()->Draw(); 
            }
            else if (currentState == STATE_DOCKED) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
                DrawText("PUERTO - MUELLE PRINCIPAL", 450, 200, 40, WHITE);
                
                DrawText("1. ALMACEN (GUARDAR OBJETOS)", 500, 400, 20, WHITE);
                DrawText("2. PESCADERO (CERRADO)", 500, 450, 20, GRAY);
                DrawText("Q. SALPAR (SALIR)", 500, 600, 20, RED);

                Vector2 mousePos = GetMousePosition();
                DrawTriangle(mousePos, {mousePos.x, mousePos.y+20}, {mousePos.x+15, mousePos.y+15}, WHITE);
            }
            else if (currentState == STATE_STORAGE) {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));
                
                DrawText("CARGA DEL BARCO", 100, 150, 20, YELLOW);
                DrawText("ALMACEN DEL PUERTO", 800, 150, 20, GREEN);

                playerBoat.GetInventory()->Draw(100, 200); 
                gameWorld.GetPort()->GetStorage()->Draw(800, 200);

                DrawText("CLICK IZQUIERDO para transferir objetos", 450, 800, 20, LIGHTGRAY);
                DrawText("TAB / ESC para Volver", 50, 50, 20, WHITE);

                Vector2 mousePos = GetMousePosition();
                DrawTriangle(mousePos, {mousePos.x, mousePos.y+20}, {mousePos.x+15, mousePos.y+15}, WHITE);
            }
            
        EndDrawing();
    }

    CloseWindow();
    return 0;
}