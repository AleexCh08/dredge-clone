#include "World.h"
#include "raymath.h"
#include <cmath> 

World::World() {
    waterModel = { 0 };
    waterShader = { 0 };
    timeLoc = 0;
}

World::~World() {
    Unload();
}

void World::Init() {
    Mesh waterMesh = GenMeshPlane(300.0f, 300.0f, 50, 50);
    waterModel = LoadModelFromMesh(waterMesh);

    waterShader = LoadShader("assets/shaders/water.vs", 0);
    waterModel.materials[0].shader = waterShader;
    
    timeLoc = GetShaderLocation(waterShader, "time");
    freqLoc = GetShaderLocation(waterShader, "frequency");
    ampLoc = GetShaderLocation(waterShader, "amplitude");
    speedLoc = GetShaderLocation(waterShader, "speed");

    // Puntos de pesca de prueba
    fishingSpots.push_back(FishingSpot((Vector3){ 20.0f, 0.0f, 20.0f }));
    fishingSpots.push_back(FishingSpot((Vector3){ -30.0f, 0.0f, -10.0f }));
    fishingSpots.push_back(FishingSpot((Vector3){ 15.0f, 0.0f, -40.0f }));
}

void World::Update(float deltaTime, float time, Vector3 playerPosition) {
    // Actualizar Shader con el tiempo global
    SetShaderValue(waterShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

    float freq = WaterConstants::frequency;
    float amp = WaterConstants::amplitude;
    float spd = WaterConstants::speed;

    SetShaderValue(waterShader, freqLoc, &freq, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, ampLoc, &amp, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, speedLoc, &spd, SHADER_UNIFORM_FLOAT);

    // Mover el agua para que siga al jugador (Océano Infinito)
    waterModel.transform = MatrixTranslate(playerPosition.x, 0.0f, playerPosition.z);
}

void World::Draw() {
    // 1. Dibujar Agua
    DrawModel(waterModel, (Vector3){0, 0, 0}, 1.0f, (Color){ 0, 121, 241, 255 });

    // 2. Dibujar Montañas (Límites)
    int numMountains = 30;
    float mapRadius = 95.0f;
                
    for (int i = 0; i < numMountains; i++) {
        float angle = (360.0f / numMountains) * i * DEG2RAD;
        float x = sin(angle) * mapRadius;
        float z = cos(angle) * mapRadius;
        
        // Montaña 
        DrawCylinder((Vector3){x, 0, z}, 0.0f, 8.0f, 15.0f, 4, DARKGRAY);  
        DrawCylinder((Vector3){x, 7.5f, z}, 0.0f, 4.05f, 7.5f, 4, WHITE);
    }

    // Dibujar puntos de pesca
    for (auto &spot : fishingSpots) {
        spot.Draw();
    }
}

void World::DrawSky() {
    Color topColor = (Color){ 65, 105, 225, 255 }; 
    Color bottomColor = (Color){ 135, 206, 250, 255 };
    
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), topColor, bottomColor);
}

FishingSpot* World::GetNearbySpot(Vector3 playerPos) {
    for (auto &spot : fishingSpots) {
        if (spot.IsActive() && spot.IsPlayerInside(playerPos)) {
            return &spot; // Retornamos la dirección de memoria del spot encontrado
        }
    }
    return nullptr; // No hay nada cerca
}

void World::Unload() {
    if (waterShader.id > 0) UnloadShader(waterShader);
    if (waterModel.meshCount > 0) UnloadModel(waterModel);
}