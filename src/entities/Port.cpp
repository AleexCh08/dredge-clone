#include "Port.h"
#include "raymath.h"
#include <cmath>

Port::Port(Vector3 pos) : storage(8, 6) {
    position = pos;
    islandRadius = 10.0f;      
    interactionRadius = 8.0f;
    dockRadius = 20.0f;
    collisionRadius = 10.0f;
    dockPosition = { position.x, 0.0f, position.z + 10.0f };
}

bool Port::IsPlayerInsideDock(Vector3 playerPos) {
    Vector3 ringPos = { position.x, 0.0f, position.z + 15.0f };
    
    float dist = Vector2Distance((Vector2){ringPos.x, ringPos.z}, (Vector2){playerPos.x, playerPos.z});
    return dist < interactionRadius;
}

bool Port::IsPlayerInside(Vector3 playerPos) {
    return Vector3Distance(playerPos, position) < dockRadius;
}

bool Port::CheckCollision(Vector3 playerPos, float playerRadius) {
    // Colisión con la Isla (Círculo) 
    float distToIsland = Vector2Distance((Vector2){position.x, position.z}, (Vector2){playerPos.x, playerPos.z});
    if (distToIsland < (collisionRadius + playerRadius)) return true;

    // Colisión con el Muelle 
    float dockWidth = 1.0f;
    float dockLength = 8.0f;

    float dx = std::abs(playerPos.x - dockPosition.x);
    float dz = std::abs(playerPos.z - dockPosition.z);

    // Verificamos si estamos dentro del rectángulo
    bool hitX = dx < (dockWidth / 2.0f + playerRadius);
    bool hitZ = dz < (dockLength / 2.0f + playerRadius);

    if (hitX && hitZ) return true; // Chocamos con la madera

    return false;
}

void Port::Draw() {
    // 1. La Isla (Cilindro Verde bajo)
    DrawCylinder(position, 10.0f, 12.0f, 2.0f, 8, GREEN);
    
    // 2. El Muelle (Cubo Marrón alargado hacia el agua)
    Vector3 dockPos = { position.x, 1.0f, position.z + 10.0f };
    DrawCube(dockPos, 4.0f, 1.0f, 10.0f, BROWN);
    
    // 3. El Edificio / Tienda (Cubo con techo)
    Vector3 housePos = { position.x, 3.0f, position.z };
    DrawCube(housePos, 4.0f, 4.0f, 4.0f, BEIGE);
    // Techo rojo
    DrawCylinder((Vector3){housePos.x, 6.0f, housePos.z}, 0.0f, 3.5f, 2.0f, 4, RED);

    // 4. Zona de Atraque (Anillo visual en el agua)
    DrawCylinderWires((Vector3){position.x, 0.0f, position.z + 15.0f}, 6.0f, 6.0f, 1.0f, 16, Fade(YELLOW, 0.5f));
}