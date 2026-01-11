#include "World.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath> 

World::World() : homePort((Vector3){-40.0f, 0.0f, -40.0f}) {
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

    // Generacion aleatoria de puntos de pesca
    fishingSpots.clear(); 
    
    int numberOfSpots = 5; // ¿Cuántas zonas de pesca quieres?
    float minDist = 30.0f;  // Mínimo 30 metros (lejos del puerto/inicio)
    float maxDist = 80.0f;  // Máximo 80 metros (antes de chocar con montañas)
    int spotsCreated = 0;
    int maxAttempts = 100; // Evitar bucle infinito
    int attempts = 0;

    while (spotsCreated < numberOfSpots && attempts < maxAttempts) {
        attempts++;
        
        float angle = (float)GetRandomValue(0, 360);
        float dist = (float)GetRandomValue((int)minDist, (int)maxDist);
        float x = sin(angle * DEG2RAD) * dist;
        float z = cos(angle * DEG2RAD) * dist;
        Vector3 candidatePos = { x, 0.0f, z };

        // VERIFICACIÓN DE SEGURIDAD:
        // Calculamos distancia al puerto
        float distToPort = Vector3Distance(candidatePos, homePort.GetPosition());

        // Si está muy cerca del puerto lo descartamos
        if (distToPort < 20.0f) {
            continue; // Intentar de nuevo
        }

        fishingSpots.push_back(FishingSpot(candidatePos));
        spotsCreated++;
    }

    /*
    for (int i = 0; i < numberOfSpots; i++) {
        float angle = (float)GetRandomValue(0, 360);
        
        float dist = (float)GetRandomValue((int)minDist, (int)maxDist);

        float x = sin(angle * DEG2RAD) * dist;
        float z = cos(angle * DEG2RAD) * dist;

        // Crear el spot
        fishingSpots.push_back(FishingSpot({ x, 0.0f, z }));
    }*/
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

    // ACTUALIZAR CADA SPOT
    for (auto &spot : fishingSpots) {
        spot.Update(deltaTime); 
    }
}

void World::Draw(Vector3 playerPos) {
    // Dibujar puerto
    homePort.Draw();

    // Dibujar Montañas (Límites)
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

    // Dibujar Agua
    BeginBlendMode(BLEND_ALPHA);
    rlDisableDepthMask();  
        DrawModel(waterModel, (Vector3){0, 0, 0}, 1.0f, (Color){ 0, 100, 200, 210 });
    rlEnableDepthMask();
    EndBlendMode();

    // Dibujar puntos de pesca
    BeginBlendMode(BLEND_ALPHA);
        for (auto &spot : fishingSpots) {
            spot.Draw(playerPos);
        }
    EndBlendMode();
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