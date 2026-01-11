#include "FishingSpot.h"
#include "raymath.h"
#include <math.h>
#include <rlgl.h>

FishingSpot::FishingSpot(Vector3 pos) {
    position = pos;
    active = true;
    radius = 2.5f; // Un radio generoso para que sea fácil atinarle
    bubbleTimer = 0.0f;
    respawnTimer = 0.0f;

    // --- INICIALIZAR BURBUJAS ---
    for(int i=0; i<20; i++) {
        BubbleParticle b;
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float dist = (GetRandomValue(0, 10) / 10.0f) * 0.5f; 
        
        b.position = { 
            pos.x + cos(angle)*dist, 
            -GetRandomValue(0, 20)/10.0f, // Altura aleatoria inicial (-2.0 a 0.0)
            pos.z + sin(angle)*dist 
        };
        
        b.speed = 0.5f + (GetRandomValue(0, 10)/10.0f); // Velocidad variable
        b.wobbleOffset = GetRandomValue(0, 100) / 10.0f;
        b.size = 0.05f + (GetRandomValue(0, 5)/100.0f); // Tamaño pequeñito
        bubbles.push_back(b);
    }

    // Generar peces visuales
    int numFish = GetRandomValue(3, 6); // Entre 3 y 6 peces por zona
    for(int i = 0; i < numFish; i++) {
        VisualFish f;
        f.angle = (float)GetRandomValue(0, 360); // Ángulo inicial aleatorio
        f.distance = (float)GetRandomValue(10, 30) / 10.0f; // Radio entre 1.0 y 3.0
        f.speed = (float)GetRandomValue(50, 150) / 100.0f; // Velocidad variable
        f.depth = -1.5f - ((float)GetRandomValue(0, 10) / 10.0f); // Entre -1.5 y -2.5 (Debajo del agua)
        f.scale = (float)GetRandomValue(8, 12) / 10.0f; // Variación de tamaño
        
        // Color aleatorio entre naranja y amarillo (tipo pez dorado)
        if (GetRandomValue(0, 1) == 0) f.color = ORANGE;
        else f.color = GOLD;

        fishes.push_back(f);
    }

    // Peces de prueba
    currentFish.name = "Bacalao";
    currentFish.width = 2;
    currentFish.height = 1; 
    currentFish.color = ORANGE;
    currentFish.price = 15;
}

void FishingSpot::Update(float deltaTime) {
    // LÓGICA DE REGENERACIÓN
    if (!active) {
        respawnTimer -= deltaTime;
        if (respawnTimer <= 0) {
            active = true; // ¡El spot vuelve a nacer!
            // Opcional: Podrías cambiar el tipo de pez aquí aleatoriamente
        }
        return; 
    }

    // ANIMACIÓN DE BURBUJAS
    float time = GetTime();
    for (auto &b : bubbles) {
        b.position.y += b.speed * deltaTime;
        b.position.x += sin(time * 5.0f + b.wobbleOffset) * 0.005f;
        
        if (b.position.y > 0.0f) {
            b.position.y = -2.0f; 
            float angle = GetRandomValue(0, 360) * DEG2RAD;
            float dist = (GetRandomValue(0, 10) / 10.0f) * 0.5f;
            b.position.x = position.x + cos(angle)*dist;
            b.position.z = position.z + sin(angle)*dist;
        }
    }
}

bool FishingSpot::IsPlayerInside(Vector3 playerPos) {
    if (!active) return false;

    // Colisión circular simple en 2D (ignoramos la altura Y)
    float dist = Vector2Distance(
        (Vector2){position.x, position.z}, 
        (Vector2){playerPos.x, playerPos.z}
    );
    
    return dist < radius;
}

void FishingSpot::Draw(Vector3 playerPos) {
    if (!active) return;

    // DIBUJAR BURBUJAS  ---
    rlDisableDepthTest();
        for (const auto &b : bubbles) {
            DrawCube(b.position, b.size, b.size, b.size, Fade(WHITE, 0.6f));
        }
    rlEnableDepthTest();

    // DIBUJAR PECES CON PROFUNDIDAD ---
    for (auto &fish : fishes) {
        // Actualizar posición 
        fish.angle += fish.speed * GetFrameTime(); 
        float rad = fish.angle;
        float fishX = position.x + cos(rad) * fish.distance;
        float fishZ = position.z + sin(rad) * fish.distance;
        Vector3 fishPos = { fishX, fish.depth, fishZ };

        float distToPlayer = Vector3Distance(fishPos, playerPos);
        
        float visibility = Remap(distToPlayer, 15.0f, 40.0f, 0.8f, 0.0f);
        
        if (visibility < 0.0f) visibility = 0.0f;
        if (visibility > 0.8f) visibility = 0.8f;

        // Si es invisible, no lo dibujamos
        if (visibility <= 0.01f) continue;

        // Dibujar
        rlPushMatrix();
            rlTranslatef(fishX, fish.depth, fishZ);
            rlRotatef(-rad * RAD2DEG, 0, 1, 0);
            Color finalColor = Fade(fish.color, visibility);
            DrawCube(Vector3{0,0,0}, 1.0f * fish.scale, 0.3f * fish.scale, 0.05f * fish.scale, finalColor);
            DrawCube(Vector3{-0.5f * fish.scale, 0, 0}, 0.3f * fish.scale, 0.3f * fish.scale, 0.05f * fish.scale, finalColor);
        rlPopMatrix();
    }
}

void FishingSpot::Deactivate() {
    active = false;
    respawnTimer = RESPAWN_TIME; // Iniciar contador
}