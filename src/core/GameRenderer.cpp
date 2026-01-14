#include "GameRenderer.h"

GameRenderer::GameRenderer() { }

void GameRenderer::Draw(GameState currentState, Boat& boat, World& world, GameCamera& camera, FishingMinigame& minigame, bool isDragging, InventoryItem draggedItem) {
    BeginDrawing();
        if (world.IsNight()) {
            ClearBackground((Color){ 10, 10, 30, 255 });
        } else {
            ClearBackground(SKYBLUE);
        }
        world.DrawSky();

        Draw3DScene(boat, world, camera);

        DrawFPS(10, 10);
        boat.DrawUI(camera.getCamera());

        world.DrawClock();

        switch (currentState) {
            case STATE_NAVIGATING: DrawUI_Navigating(boat, world); break;
            case STATE_FISHING:    DrawUI_Fishing(minigame); break;
            case STATE_INVENTORY:  DrawUI_Inventory(boat); break;
            case STATE_DOCKED:     DrawUI_Docked(); break;
            case STATE_STORAGE:    DrawUI_Storage(boat, world); break;
        }

        if (isDragging) {
            Vector2 mouse = GetMousePosition();
            DrawRectangle(mouse.x, mouse.y, draggedItem.width * CELL_SIZE, draggedItem.height * CELL_SIZE, Fade(draggedItem.color, 0.8f));
            DrawRectangleLines(mouse.x, mouse.y, draggedItem.width * CELL_SIZE, draggedItem.height * CELL_SIZE, WHITE);
        }

        if (currentState == STATE_GAMEOVER) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
            DrawText("EL MAR TE HA RECLAMADO", GetScreenWidth()/2 - 200, GetScreenHeight()/2 - 20, 40, RED);
            DrawText("Presiona ESC para salir", GetScreenWidth()/2 - 150, GetScreenHeight()/2 + 40, 20, GRAY);
        }

    EndDrawing();
}

void GameRenderer::Draw3DScene(Boat& boat, World& world, GameCamera& camera) {
    BeginMode3D(camera.getCamera());
        world.Draw(boat.getPosition());
        boat.Draw();                
    EndMode3D();
}

void GameRenderer::DrawUI_Navigating(Boat& boat, World& world) {
    FishingSpot* spot = world.GetNearbySpot(boat.getPosition());
    if (spot) DrawText("E - PESCAR", 600, 600, 20, WHITE);
    
    if (world.GetPort()->IsPlayerInside(boat.getPosition())) {
        DrawText("ESTAS EN EL PUERTO", 600, 200, 20, GREEN);
    }
    if (world.GetPort()->IsPlayerInsideDock(boat.getPosition())) {
        DrawText("PRESIONA 'E' PARA ATRACAR", 500, 100, 20, WHITE);
    }
}

void GameRenderer::DrawUI_Fishing(FishingMinigame& minigame) {
    minigame.Draw();
}

void GameRenderer::DrawUI_Inventory(Boat& boat) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    boat.GetInventory()->Draw();
    DrawTooltip(boat.GetInventory(), 0, 0);
    DrawCustomCursor(); 
}

void GameRenderer::DrawUI_Docked() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
    DrawText("PUERTO - MUELLE PRINCIPAL", 450, 200, 40, WHITE);
    
    DrawText("1. ALMACEN (GUARDAR OBJETOS)", 500, 400, 20, WHITE);
    DrawText("2. PESCADERO (CERRADO)", 500, 450, 20, GRAY);
    DrawText("Q. SALPAR (SALIR)", 500, 600, 20, RED);

    DrawCustomCursor();
}

void GameRenderer::DrawUI_Storage(Boat& boat, World& world) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.85f));
    
    DrawText("CARGA DEL BARCO", 100, 150, 20, YELLOW);
    DrawText("ALMACEN DEL PUERTO", 800, 150, 20, GREEN);

    boat.GetInventory()->Draw(100, 200); 
    world.GetPort()->GetStorage()->Draw(800, 200);

    DrawTooltip(boat.GetInventory(), 100, 200);
    DrawTooltip(world.GetPort()->GetStorage(), 800, 200);

    DrawText("CLICK IZQUIERDO para transferir objetos", 450, 800, 20, LIGHTGRAY);
    DrawText("TAB / ESC para Volver", 50, 50, 20, WHITE);

    DrawCustomCursor();
}

void GameRenderer::DrawCustomCursor() {
    Vector2 mousePos = GetMousePosition();
    DrawTriangle(mousePos, {mousePos.x, mousePos.y+20}, {mousePos.x+15, mousePos.y+15}, WHITE);
}

void GameRenderer::DrawTooltip(Inventory* inv, int offsetX, int offsetY) {
    int idx = inv->GetItemIndexUnderMouse(offsetX, offsetY);
    
    if (idx != -1) {
        InventoryItem item = inv->GetItem(idx);
        Vector2 mouse = GetMousePosition();
        
        const char* text = TextFormat("%s (%dx%d)", item.name.c_str(), item.width, item.height);
        int textWidth = MeasureText(text, 20);
        
        int boxX = mouse.x + 15;
        int boxY = mouse.y + 15;
        
        DrawRectangle(boxX, boxY, textWidth + 20, 30, DARKGRAY);
        DrawRectangleLines(boxX, boxY, textWidth + 20, 30, WHITE);
        DrawText(text, boxX + 10, boxY + 5, 20, WHITE);
    }
}