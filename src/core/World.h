#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <vector>
#include "../entities/FishingSpot.h"
#include "../entities/Port.h"

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
    
    Port* GetPort() { return &homePort; }
    FishingSpot* GetNearbySpot(Vector3 playerPos);

    float GetTimeOfDay() const { return timeOfDay; }
    bool IsNight() const;
    void SkipTime(float hours);

private:
    Model waterModel;
    Shader waterShader;
    Port homePort;

    int timeLoc; // Ubicación de la variable 'time' en el shader
    int freqLoc;
    int ampLoc;
    int speedLoc;
    std::vector<FishingSpot> fishingSpots; // Lista de puntos de pesca 
    
    float timeOfDay; // 0.0f a 24.0f
    const float TIME_SPEED = 0.5f; // 1 segundo real = 0.5 horas juego (Ajusta a tu gusto)
};

#endif