#include "FishingSpot.h"
#include "raymath.h"
#include <math.h>
#include <rlgl.h>

FishingSpot::FishingSpot(Vector3 pos) {
    position = pos;
    active = true;
    radius = 6.0f; // Un radio generoso para que sea fácil atinarle
    bubbleTimer = 0.0f;

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

    // --- 1. DIBUJAR BURBUJAS (Igual que antes) ---
    bubbleTimer += GetFrameTime();
    float yOffset = sin(bubbleTimer * 3.0f) * 0.2f;
    DrawCylinder((Vector3){position.x, 0.0f + yOffset, position.z}, 1.5f, 1.5f, 0.15f, 8, Fade(WHITE, 0.25f));
    DrawCylinderWires((Vector3){position.x, 0.0f, position.z}, radius, radius, 1.0f, 16, Fade(SKYBLUE, 0.3f));

    // --- 2. DIBUJAR PECES CON PROFUNDIDAD ---
    for (auto &fish : fishes) {
        // A. Actualizar posición (Igual que antes)
        fish.angle += fish.speed * GetFrameTime(); 
        float rad = fish.angle;
        float fishX = position.x + cos(rad) * fish.distance;
        float fishZ = position.z + sin(rad) * fish.distance;
        Vector3 fishPos = { fishX, fish.depth, fishZ };

        // B. CALCULAR DISTANCIA Y VISIBILIDAD (NUEVO)
        float distToPlayer = Vector3Distance(fishPos, playerPos);
        
        float visibility = Remap(distToPlayer, 15.0f, 40.0f, 0.8f, 0.0f);
        
        // Clamp (Asegurar que esté entre 0 y 1)
        if (visibility < 0.0f) visibility = 0.0f;
        if (visibility > 0.8f) visibility = 0.8f;

        // Si es invisible, no lo dibujamos (ahorra recursos)
        if (visibility <= 0.01f) continue;

        // C. Dibujar
        rlPushMatrix();
            rlTranslatef(fishX, fish.depth, fishZ);
            float rotationDegrees = -rad * RAD2DEG; 
            rlRotatef(rotationDegrees, 0, 1, 0);

            // Usamos Fade() para aplicar la transparencia calculada al color del pez
            Color finalColor = Fade(fish.color, visibility);

            DrawCube(Vector3{0,0,0}, 1.0f * fish.scale, 0.3f * fish.scale, 0.05f * fish.scale, finalColor);
            DrawCube(Vector3{-0.5f * fish.scale, 0, 0}, 0.3f * fish.scale, 0.3f * fish.scale, 0.05f * fish.scale, finalColor);
        rlPopMatrix();
    }
}