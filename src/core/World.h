#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <vector>
#include "../entities/FishingSpot.h"

struct WaterConstants {
    static constexpr float frequency = 2.0f;
    static constexpr float amplitude = 0.5f;
    static constexpr float speed = 2.0f;
};

class World {
public:
    World();  // Constructor
    ~World(); // Destructor 

    void Init(); // Cargar modelos y shaders
    void Update(float deltaTime, float time, Vector3 playerPosition);
    void Draw(Vector3 playerPos); 
    void DrawSky();
    void Unload(); // Limpieza manual si se requiere

    FishingSpot* GetNearbySpot(Vector3 playerPos);

private:
    Model waterModel;
    Shader waterShader;
    int timeLoc; // Ubicación de la variable 'time' en el shader
    int freqLoc;
    int ampLoc;
    int speedLoc;
    std::vector<FishingSpot> fishingSpots; // Lista de puntos de pesca
};

#endif