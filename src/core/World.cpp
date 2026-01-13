#include "World.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath> 

World::World() : homePort((Vector3){-40.0f, 0.0f, -40.0f}) {
    waterModel = { 0 };
    waterShader = { 0 };
    timeLoc = 0;
    timeOfDay = 8.0f;
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
}

void World::Update(float deltaTime, float time, Vector3 playerPosition) {
    // Reloj global
    timeOfDay += deltaTime * TIME_SPEED;
    if (timeOfDay >= 24.0f) {
        timeOfDay = 0.0f; // Nuevo día
    }

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
    // Colores base
    Color dayTop = (Color){ 65, 105, 225, 255 };    // Azul Real
    Color dayBottom = (Color){ 135, 206, 250, 255 }; // Azul Cielo
    
    Color nightTop = (Color){ 10, 10, 30, 255 };     // Casi negro
    Color nightBottom = (Color){ 25, 25, 112, 255 }; // Azul Medianoche

    // Decidimos qué color usar
    Color topColor, bottomColor;

    if (IsNight()) {
        topColor = nightTop;
        bottomColor = nightBottom;
    } else {
        // Opcional: Podríamos hacer un degradado al atardecer, 
        // pero por ahora cambio brusco para probar.
        topColor = dayTop;
        bottomColor = dayBottom;
    }
    
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), topColor, bottomColor);
    
    // UI DEL RELOJ (TEMPORAL para debug)
    DrawText(TextFormat("%02.0f:00", floor(timeOfDay)), GetScreenWidth() - 100, 20, 30, WHITE);
}

FishingSpot* World::GetNearbySpot(Vector3 playerPos) {
    for (auto &spot : fishingSpots) {
        if (spot.IsActive() && spot.IsPlayerInside(playerPos)) {
            return &spot; // Retornamos la dirección de memoria del spot encontrado
        }
    }
    return nullptr; // No hay nada cerca
}

bool World::IsNight() const {
    // Consideramos noche desde las 19:00 (7 PM) hasta las 05:00 (6 AM)
    return (timeOfDay >= 19.0f || timeOfDay < 6.0f);
}

void World::SkipTime(float hours) {
    timeOfDay += hours;
    if (timeOfDay >= 24.0f) timeOfDay -= 24.0f;
}

void World::Unload() {
    if (waterShader.id > 0) UnloadShader(waterShader);
    if (waterModel.meshCount > 0) UnloadModel(waterModel);
}