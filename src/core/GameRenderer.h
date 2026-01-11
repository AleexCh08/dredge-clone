#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include "raylib.h"
#include "../entities/Boat.h"
#include "../core/World.h"
#include "../core/GameCamera.h"
#include "../core/FishingMinigame.h"
#include "GameConstants.h"

class GameRenderer {
public:
    GameRenderer();
    
    void Draw(
        GameState currentState, 
        Boat& boat, 
        World& world, 
        GameCamera& camera, 
        FishingMinigame& minigame
    );

private:
    void Draw3DScene(Boat& boat, World& world, GameCamera& camera);
    void DrawUI_Navigating(Boat& boat, World& world);
    void DrawUI_Fishing(FishingMinigame& minigame);
    void DrawUI_Inventory(Boat& boat);
    void DrawUI_Docked();
    void DrawUI_Storage(Boat& boat, World& world);
    
    void DrawCustomCursor();
};

#endif