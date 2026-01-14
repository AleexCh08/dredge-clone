#include "World.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>
#include "entities/Boat.h"

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

    worldShader = LoadShader("assets/shaders/world.vs", "assets/shaders/world.fs");

    worldViewPosLoc = GetShaderLocation(worldShader, "viewPos");
    worldNightFactorLoc = GetShaderLocation(worldShader, "nightFactor");
    worldLightRadiusLoc = GetShaderLocation(worldShader, "lightRadius");

    waterShader = LoadShader("assets/shaders/water.vs", "assets/shaders/water.fs");
    waterModel.materials[0].shader = waterShader;
    waterModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){ 0, 100, 200, 255 };
    waterShader.locs[SHADER_LOC_COLOR_DIFFUSE] = GetShaderLocation(waterShader, "colDiffuse");
  
    timeLoc = GetShaderLocation(waterShader, "time");
    freqLoc = GetShaderLocation(waterShader, "frequency");
    ampLoc = GetShaderLocation(waterShader, "amplitude");
    speedLoc = GetShaderLocation(waterShader, "speed");

    viewPosLoc = GetShaderLocation(waterShader, "viewPos");
    nightFactorLoc = GetShaderLocation(waterShader, "nightFactor");
    lightRadiusLoc = GetShaderLocation(waterShader, "lightRadius");

    lightRadius = 20.0f;

    // Generacion de montañas
    obstacles.clear();
    int numMountains = 30;
    float mapRadius = 95.0f; // El radio donde están las montañas

    for (int i = 0; i < numMountains; i++) {
        float angle = (360.0f / numMountains) * i * DEG2RAD;
        float x = sin(angle) * mapRadius;
        float z = cos(angle) * mapRadius;
        
        // Guardamos el obstáculo
        Obstacle obs;
        obs.position = { x, 0.0f, z };
        obs.radius = 8.0f; // Radio aproximado de la base de la montaña
        obs.isVisible = true;
        obs.isHallucination = false;
        obs.lifetime = -1.0f;
        
        obstacles.push_back(obs);
    }

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

        float distToPort = Vector3Distance(candidatePos, homePort.GetPosition());

        // Si está muy cerca del puerto lo descartamos
        if (distToPort < 20.0f) {
            continue; // Intentar de nuevo
        }

        fishingSpots.push_back(FishingSpot(candidatePos));
        spotsCreated++;
    }
}

void World::Update(float deltaTime, float time, Vector3 playerPosition, bool boatLightOn) {
    // Reloj global
    timeOfDay += deltaTime * TIME_SPEED;
    if (timeOfDay >= 24.0f) {
        timeOfDay = 0.0f; // Nuevo día
    }

    currentNightFactor = 0.0f; 
    
    // Atardecer (17:00 a 20:00)
    if (timeOfDay >= 17.0f && timeOfDay < 20.0f) {
        currentNightFactor = (timeOfDay - 17.0f) / 3.0f; 
    }
    // Noche cerrada (20:00 a 05:00)
    else if (IsNight()) {
        currentNightFactor = 1.0f; 
    }
    // Amanecer (05:00 a 08:00)
    else if (timeOfDay >= 5.0f && timeOfDay < 8.0f) {
        currentNightFactor = 1.0f - ((timeOfDay - 5.0f) / 3.0f);
    }

    // Actualizar Shader con el tiempo global
    SetShaderValue(waterShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);

    float currentRadius = -20.0f;
    if (currentNightFactor < 0.1f) {
        currentRadius = 1000.0f;
    } 
    else if (boatLightOn) {
        currentRadius = 8.0f; // Luz encendida
    } 

    SetShaderValue(waterShader, viewPosLoc, &playerPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue(waterShader, nightFactorLoc, &currentNightFactor, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, lightRadiusLoc, &currentRadius, SHADER_UNIFORM_FLOAT);

    float freq = WaterConstants::frequency;
    float amp = WaterConstants::amplitude;
    float spd = WaterConstants::speed;

    SetShaderValue(waterShader, freqLoc, &freq, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, ampLoc, &amp, SHADER_UNIFORM_FLOAT);
    SetShaderValue(waterShader, speedLoc, &spd, SHADER_UNIFORM_FLOAT);

    SetShaderValue(worldShader, worldViewPosLoc, &playerPosition, SHADER_UNIFORM_VEC3);
    SetShaderValue(worldShader, worldNightFactorLoc, &currentNightFactor, SHADER_UNIFORM_FLOAT);
    SetShaderValue(worldShader, worldLightRadiusLoc, &currentRadius, SHADER_UNIFORM_FLOAT);

    // Mover el agua para que siga al jugador (Océano Infinito)
    waterModel.transform = MatrixTranslate(playerPosition.x, 0.0f, playerPosition.z);

    // ACTUALIZAR CADA SPOT
    for (auto &spot : fishingSpots) {
        spot.Update(deltaTime); 
    }
}

void World::Draw(Vector3 playerPos) {
    Color worldTint = WHITE;
    if (IsNight()) {
        worldTint = (Color){ 50, 50, 80, 255 }; // Oscuridad azulada
    } else if (timeOfDay >= 18.0f && timeOfDay < 19.0f) {
        // Transición simple
        worldTint = (Color){ 150, 100, 100, 255 }; // Atardecer rojizo
    }

    // Dibujar Agua
    BeginBlendMode(BLEND_ALPHA);
    rlDisableDepthMask();  
        DrawModel(waterModel, (Vector3){0, 0, 0}, 1.0f, WHITE);
    rlEnableDepthMask();
    EndBlendMode();

    BeginShaderMode(worldShader);
        // Dibujar puerto
        homePort.Draw(worldTint);

        // Dibujar Montañas (Límites)
        Color mountainColor = IsNight() ? (Color){ 20, 20, 20, 255 } : DARKGRAY;
        Color snowColor = IsNight() ? (Color){ 80, 80, 100, 255 } : WHITE;
                    
        for (const auto& obs : obstacles) {
            if (!obs.isVisible) continue;
 
            DrawCylinder(obs.position, 0.0f, obs.radius, 15.0f, 6, mountainColor);  
            DrawCylinder((Vector3){obs.position.x, 7.7f, obs.position.z}, 0.0f, obs.radius * 0.5f, 7.5f, 6, snowColor);
        }
    EndShaderMode();

    // Dibujar puntos de pesca
    BeginBlendMode(BLEND_ALPHA);
        for (auto &spot : fishingSpots) {
            spot.Draw(playerPos);
        }
    EndBlendMode();
}

void World::DrawSky() {
    // 1. Definir Paleta de Colores
    Color dayTop = (Color){ 100, 150, 255, 255 };    // Azul claro
    Color dayBot = (Color){ 200, 230, 255, 255 };    // Blanco azulado
    
    Color duskTop = (Color){ 60, 40, 100, 255 };     // Morado oscuro
    Color duskBot = (Color){ 255, 100, 50, 255 };    // Naranja atardecer
    
    Color nightTop = (Color){ 10, 10, 30, 255 };     // Negro azulado
    Color nightBot = (Color){ 20, 30, 60, 255 };     // Azul oscuro

    Color finalTop, finalBot;
 
    if (currentNightFactor < 0.5f) {
        float t = currentNightFactor * 2.0f; 
        
        finalTop = (Color){
            (unsigned char)Lerp(dayTop.r, duskTop.r, t),
            (unsigned char)Lerp(dayTop.g, duskTop.g, t),
            (unsigned char)Lerp(dayTop.b, duskTop.b, t), 255
        };
        finalBot = (Color){
            (unsigned char)Lerp(dayBot.r, duskBot.r, t),
            (unsigned char)Lerp(dayBot.g, duskBot.g, t),
            (unsigned char)Lerp(dayBot.b, duskBot.b, t), 255
        };
    } else {
        float t = (currentNightFactor - 0.5f) * 2.0f;

        finalTop = (Color){
            (unsigned char)Lerp(duskTop.r, nightTop.r, t),
            (unsigned char)Lerp(duskTop.g, nightTop.g, t),
            (unsigned char)Lerp(duskTop.b, nightTop.b, t), 255
        };
        finalBot = (Color){
            (unsigned char)Lerp(duskBot.r, nightBot.r, t),
            (unsigned char)Lerp(duskBot.g, nightBot.g, t),
            (unsigned char)Lerp(duskBot.b, nightBot.b, t), 255
        };
    }

    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), finalTop, finalBot);
    
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

void World::DrawClock() {
    int hour = (int)timeOfDay;
    int minute = (int)((timeOfDay - (float)hour) * 60.0f);
    
    DrawText(TextFormat("%02d:%02d", hour, minute), GetScreenWidth() - 110, 20, 30, WHITE);
}

bool World::CheckCollisions(Boat& boat) {
    Vector3 boatPos = boat.getPosition();
    float boatSpeed = fabs(boat.GetSpeed()); // Velocidad absoluta

    for (auto& obs : obstacles) {
        if (!obs.isVisible) continue;

        float dist = Vector2Distance({boatPos.x, boatPos.z}, {obs.position.x, obs.position.z});
        
        if (dist < (obs.radius + 2.0f)) {
            if (obs.isHallucination) {
                obs.lifetime = 0.0f; 
            }
            
            if (boatSpeed > 5.0f) {
                boat.TakeDamage(1); 
            } else {
                boat.TakeDamage(0); 
            }
            return true;
        }
    }
    return false;
}

void World::UpdateHallucinations(float deltaTime, float panicLevel, Vector3 playerPos, Vector3 playerForward) {
    CleanupObstacles();

    if (panicLevel < 50.0f) return;

    float chance = (panicLevel - 50.0f) * 0.001f; 
    
    if (GetRandomValue(0, 1000) < (chance * 100.0f)) {   
        Obstacle rock; // GENERAR ROCA FANTASMA
        
        // Aparece frente al jugador a cierta distancia (entre 15 y 30 metros)
        float spawnDist = (float)GetRandomValue(15, 30);
        float offsetSide = (float)GetRandomValue(-5, 5);
        
        Vector3 right = { playerForward.z, 0.0f, -playerForward.x };
        
        rock.position = Vector3Add(playerPos, Vector3Scale(playerForward, spawnDist));
        rock.position = Vector3Add(rock.position, Vector3Scale(right, offsetSide));

        float distToPort = Vector3Distance(rock.position, homePort.GetPosition());
        if (distToPort < 50.0f) { 
            return; 
        }
        
        rock.radius = (float)GetRandomValue(2, 4); // Rocas más pequeñas
        rock.isVisible = true;
        rock.isHallucination = true;
        rock.lifetime = 10.0f; // Duran 10 segundos y desaparecen
        
        obstacles.push_back(rock);
    }
}

void World::CleanupObstacles() {
    // Recorremos y borramos las que se vencieron
    for (auto it = obstacles.begin(); it != obstacles.end(); ) {
        if (it->isHallucination) {
            it->lifetime -= GetFrameTime(); // Restamos tiempo
            if (it->lifetime <= 0.0f) {
                it = obstacles.erase(it); // Borrar y avanzar iterador
                continue;
            }
        }
        ++it;
    }
}

void World::Unload() {
    if (waterShader.id > 0) UnloadShader(waterShader);
    if (waterModel.meshCount > 0) UnloadModel(waterModel);
}