#include "GameRenderer.h"

// Necesitamos definir los estados aquí o en un header común.
// Para no romper nada, usaremos los valores literales del enum del main
// (0=NAVIGATING, 1=FISHING, 2=INVENTORY, 3=DOCKED, 4=STORAGE)
// Lo ideal es mover el enum a "GameConstants.h"

GameRenderer::GameRenderer() { }

void GameRenderer::Draw(int currentState, Boat& boat, World& world, GameCamera& camera, FishingMinigame& minigame) {
    BeginDrawing();
        ClearBackground(BLACK);
        world.DrawSky();

        // 1. DIBUJAR MUNDO 3D
        Draw3DScene(boat, world, camera);

        // 2. DIBUJAR UI GLOBAL
        DrawFPS(10, 10);
        boat.DrawUI(camera.getCamera()); // Feedback flotante

        // 3. DIBUJAR UI SEGÚN ESTADO
        switch (currentState) {
            case 0: DrawUI_Navigating(boat, world); break; // STATE_NAVIGATING
            case 1: DrawUI_Fishing(minigame); break;       // STATE_FISHING
            case 2: DrawUI_Inventory(boat); break;         // STATE_INVENTORY
            case 3: DrawUI_Docked(); break;                // STATE_DOCKED
            case 4: DrawUI_Storage(boat, world); break;    // STATE_STORAGE
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
    DrawCustomCursor(); // Agregamos cursor aquí también si queremos
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

    DrawText("CLICK IZQUIERDO para transferir objetos", 450, 800, 20, LIGHTGRAY);
    DrawText("TAB / ESC para Volver", 50, 50, 20, WHITE);

    DrawCustomCursor();
}

void GameRenderer::DrawCustomCursor() {
    Vector2 mousePos = GetMousePosition();
    DrawTriangle(mousePos, {mousePos.x, mousePos.y+20}, {mousePos.x+15, mousePos.y+15}, WHITE);
}