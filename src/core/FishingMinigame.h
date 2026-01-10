#ifndef FISHINGMINIGAME_H
#define FISHINGMINIGAME_H

#include "raylib.h"

class FishingMinigame {
public:
    FishingMinigame();
    
    // Inicia el juego (resetea ángulos)
    void Start();
    
    // Retorna true si el juego sigue activo, false si terminó (ganó o canceló)
    void Update();
    void Draw();

    // Getters para saber el resultado (luego los usaremos)
    bool IsActive() { return active; }
    bool DidWin() { return lastResult; }

private:
    bool active;
    bool lastResult;
    
    float rotationAngle; // Dónde está la aguja (0 a 360)
    float rotationSpeed; // Qué tan rápido gira
    
    // La Zona Verde (Target)
    float targetStartAngle;
    float targetSize; // Ancho del arco en grados
};

#endif