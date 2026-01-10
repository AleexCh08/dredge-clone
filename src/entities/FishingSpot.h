#ifndef FISHINGSPOT_H
#define FISHINGSPOT_H

#include "raylib.h"

class FishingSpot {
public:
    // Constructor con posición
    FishingSpot(Vector3 pos);
    
    void Draw();
    
    // Retorna true si el barco está dentro del radio de pesca
    bool IsPlayerInside(Vector3 playerPos);
    
    // Getters
    bool IsActive() const { return active; }
    Vector3 GetPosition() const { return position; }
    
    // Desactivar el punto (cuando pescamos todo)
    void Deactivate() { active = false; }

private:
    Vector3 position;
    bool active;       // ¿Quedan peces?
    float radius;      // Radio de detección del barco
    float bubbleTimer; // Para animación simple
};

#endif