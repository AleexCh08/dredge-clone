#ifndef WORLD_H
#define WORLD_H

#include "raylib.h"
#include <vector>
#include "../entities/FishingSpot.h"
#include "../entities/Port.h"

struct Obstacle {
    Vector3 position;
    float radius;
    bool isVisible; // Para las alucinaciones (aparecen/desaparecen)

    bool isHallucination; // ¿Es real o fake?
    float lifetime;
};

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
    void Update(float deltaTime, float time, Vector3 playerPosition, bool boatLightOn);
    void Draw(Vector3 playerPos); 
    void DrawSky();
    void DrawClock();
    void Unload(); // Limpieza manual si se requiere
    bool CheckCollisions(class Boat& boat); // Retorna true si hubo choque
    
    Port* GetPort() { return &homePort; }
    FishingSpot* GetNearbySpot(Vector3 playerPos);

    float GetTimeOfDay() const { return timeOfDay; }
    bool IsNight() const;
    void SkipTime(float hours);

    void UpdateHallucinations(float deltaTime, float panicLevel, Vector3 playerPos, Vector3 playerForward); // Mostrar alucinaciones
    void CleanupObstacles();

private:
    Model waterModel;
    Shader waterShader;
    Shader worldShader;
    Port homePort;

    // Shaders
    int worldViewPosLoc, worldNightFactorLoc, worldLightRadiusLoc;

    int timeLoc; int freqLoc; int ampLoc;
    
    int speedLoc;
    int viewPosLoc;     // Posición del barco
    int nightFactorLoc; // Cuánto oscurecer
    int lightRadiusLoc; // Radio de la linterna

    float lightRadius;

    std::vector<FishingSpot> fishingSpots; // Lista de puntos de pesca 
    std::vector<Obstacle> obstacles; // Lista de obstaculos
    
    float timeOfDay; // 0.0f a 24.0f
    float currentNightFactor;
    const float TIME_SPEED = 1.0f / 12.0f; // 1 hora de juego = 12 segundos reales (Ajusta a tu gusto)
};

#endif