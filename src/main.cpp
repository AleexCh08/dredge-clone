#include "raylib.h"
#include "entities/Boat.h"
#include "core/GameCamera.h"
#include "core/FishingMinigame.h"
#include "core/World.h"
#include "core/GameRenderer.h"
#include "core/GameConstants.h"
#include <raymath.h>

bool gameRunning = true;
bool isDragging = false;
InventoryItem draggedItem;       
Inventory* sourceInventory = nullptr; 
int draggedItemOriginalIndex = -1;

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1440, 900, "Dredge Clone"); 
    SetExitKey(KEY_NULL);
    DisableCursor(); 
    ToggleFullscreen();
    SetTargetFPS(60);

    Boat playerBoat;
    GameCamera gameCamera;
    World gameWorld;
    gameWorld.Init();
    FishingMinigame minigame;
    GameRenderer renderer;
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
                Inventory* inv = playerBoat.GetInventory();

                if (!isDragging && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    int idx = inv->GetItemIndexUnderMouse(0, 0);
                    if (idx != -1) {
                        draggedItem = inv->GetItem(idx);
                        draggedItemOriginalIndex = idx;
                        sourceInventory = inv;
                        isDragging = true;
                        inv->RemoveItem(idx);
                    }
                }

                if (isDragging) {
                    if (IsKeyPressed(KEY_R)) {
                        int temp = draggedItem.width;
                        draggedItem.width = draggedItem.height;
                        draggedItem.height = temp;
                    }

                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                        Vector2 gridPos = inv->GetGridCellFromMouse(0, 0);                       
                        bool placed = false;
                        if (gridPos.x != -1) {
                            if (inv->AddItemAt(draggedItem, (int)gridPos.x, (int)gridPos.y)) {
                                placed = true; 
                            }
                        }
                        
                        if (!placed) {
                            if (gridPos.x == -1) {
                                playerBoat.ShowFeedback("¡TIRADO AL AGUA!", RED);
                                // Feedback opcional: Sonido de "Splash"
                            } else {
                                InventoryItem original = draggedItem;
                                if (!inv->AddItemAt(draggedItem, draggedItem.gridX, draggedItem.gridY)) {
                                    int temp = draggedItem.width; draggedItem.width = draggedItem.height; draggedItem.height = temp;
                                    inv->AddItemAt(draggedItem, draggedItem.gridX, draggedItem.gridY);
                                }
                            }
                        }
                        
                        isDragging = false;
                        sourceInventory = nullptr;
                    }
                }

                if (!isDragging && (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_ESCAPE))) {
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

                if (IsKeyPressed(KEY_R)) {
                    int idxBoat = boatInv->GetItemIndexUnderMouse(boatX, boatY);
                    if (idxBoat != -1) boatInv->TryRotateItem(idxBoat);

                    int idxPort = portInv->GetItemIndexUnderMouse(portX, portY);
                    if (idxPort != -1) portInv->TryRotateItem(idxPort);
                }

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    int itemIdx = boatInv->GetItemIndexUnderMouse(boatX, boatY);
                    if (itemIdx != -1) {
                        InventoryItem item = boatInv->GetItem(itemIdx);
                        if (portInv->AddItem(item.name.c_str(), item.width, item.height, item.color)) {
                            boatInv->RemoveItem(itemIdx);
                        } else {
                            playerBoat.ShowFeedback("¡ALMACEN LLENO!", RED);
                        }
                    }

                    int itemIdxPort = portInv->GetItemIndexUnderMouse(portX, portY);
                    if (itemIdxPort != -1) {
                        InventoryItem item = portInv->GetItem(itemIdxPort);
                        if (boatInv->AddItem(item.name.c_str(), item.width, item.height, item.color)) {
                            portInv->RemoveItem(itemIdxPort);
                        } else {
                            playerBoat.ShowFeedback("¡BARCO LLENO!", RED);
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

        // DIBUJADO
        renderer.Draw(currentState, playerBoat, gameWorld, gameCamera, minigame, isDragging, draggedItem);       
    }

    CloseWindow();
    return 0;
}