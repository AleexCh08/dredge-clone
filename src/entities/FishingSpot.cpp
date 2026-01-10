#include "FishingSpot.h"
#include "raymath.h"
#include <math.h>

FishingSpot::FishingSpot(Vector3 pos) {
    position = pos;
    active = true;
    radius = 6.0f; // Un radio generoso para que sea fácil atinarle
    bubbleTimer = 0.0f;
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

void FishingSpot::Draw() {
    if (!active) return;

    // Animación de "burbujas"
    bubbleTimer += GetFrameTime();
    
    // Hacemos que flote un poco
    float yOffset = sin(bubbleTimer * 3.0f) * 0.2f;
    
    // Dibujamos el área de pesca
    // Cilindro blanco semitransparente
    DrawCylinder((Vector3){position.x, 0.0f + yOffset, position.z}, 
                 1.5f, 1.5f, 0.5f, 8, Fade(WHITE, 0.5f));
                 
    // Anillo externo para indicar el radio de detección
    DrawCylinderWires((Vector3){position.x, 0.0f, position.z}, 
                      radius, radius, 1.0f, 16, Fade(SKYBLUE, 0.3f));
}