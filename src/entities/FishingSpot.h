#ifndef FISHINGSPOT_H
#define FISHINGSPOT_H

#include "raylib.h"
#include <vector>

struct VisualFish {
    float angle;      // En qué punto del círculo está
    float speed;      // Qué tan rápido nada
    float distance;   // Qué tan lejos del centro
    float depth;      // Profundidad (Y negativo)
    float scale;      // Tamaño del pez
    Color color;
};

class FishingSpot {
public:
    // Constructor con posición
    FishingSpot(Vector3 pos);
    
    void Draw(Vector3 playerPos);
    bool IsPlayerInside(Vector3 playerPos);
    
    bool IsActive() const { return active; }
    Vector3 GetPosition() const { return position; }
    void Deactivate() { active = false; }

private:
    Vector3 position;
    bool active;       // ¿Quedan peces?
    float radius;      // Radio de detección del barco
    float bubbleTimer; // Para animación simple

    std::vector<VisualFish> fishes;
};

#endif