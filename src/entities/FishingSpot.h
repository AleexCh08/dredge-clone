#ifndef FISHINGSPOT_H
#define FISHINGSPOT_H

#include "raylib.h"
#include <vector>
#include <string>

struct BubbleParticle {
    Vector3 position;
    float speed;
    float wobbleOffset; // Para que no suban en línea recta perfecta
    float size;
};

struct VisualFish {
    float angle;      // En qué punto del círculo está
    float speed;      // Qué tan rápido nada
    float distance;   // Qué tan lejos del centro
    float depth;      // Profundidad (Y negativo)
    float scale;      // Tamaño del pez
    Color color;
};

struct FishType {
    std::string name;
    int width;
    int height;
    Color color;
    int price;
};

class FishingSpot {
public:
    // Constructor con posición
    FishingSpot(Vector3 pos);

    void Update(float deltaTime);
    
    void Draw(Vector3 playerPos);
    bool IsPlayerInside(Vector3 playerPos);
    
    bool IsActive() const { return active; }
    Vector3 GetPosition() const { return position; }
    void Deactivate();

    FishType GetFishType() const { return currentFish; }

private:
    Vector3 position;
    bool active;       // ¿Quedan peces?
    float radius;      // Radio de detección del barco
    float bubbleTimer; // Para animación simple

    float respawnTimer;
    const float RESPAWN_TIME = 15.0f;

    std::vector<VisualFish> fishes;
    std::vector<BubbleParticle> bubbles;
    FishType currentFish;
};

#endif