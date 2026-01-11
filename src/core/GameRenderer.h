#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include "raylib.h"
#include "../entities/Boat.h"
#include "../core/World.h"
#include "../core/GameCamera.h"
#include "../core/FishingMinigame.h"

// Enum duplicado o incluido de un header común (GameStates.h sería ideal, 
// pero por ahora lo pasamos como int o incluimos main.cpp indirectamente. 
// Mejor: pasamos el estado como int para no complicar includes circulares).

class GameRenderer {
public:
    GameRenderer();
    
    // Método maestro que orquesta todo el dibujo
    void Draw(
        int currentState, 
        Boat& boat, 
        World& world, 
        GameCamera& camera, 
        FishingMinigame& minigame
    );

private:
    // Métodos auxiliares para limpiar el Draw principal
    void Draw3DScene(Boat& boat, World& world, GameCamera& camera);
    void DrawUI_Navigating(Boat& boat, World& world);
    void DrawUI_Fishing(FishingMinigame& minigame);
    void DrawUI_Inventory(Boat& boat);
    void DrawUI_Docked();
    void DrawUI_Storage(Boat& boat, World& world);
    
    void DrawCustomCursor();
};

#endif