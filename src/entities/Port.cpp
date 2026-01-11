#include "Port.h"
#include "raymath.h"

Port::Port(Vector3 pos) : storage(8, 6) {
    position = pos;
    dockRadius = 25.0f;
    collisionRadius = 10.0f; 
}

bool Port::IsPlayerInside(Vector3 playerPos) {
    return Vector3Distance(playerPos, position) < dockRadius;
}

bool Port::CheckCollision(Vector3 playerPos, float playerRadius) {
    float dist = Vector2Distance((Vector2){position.x, position.z}, (Vector2){playerPos.x, playerPos.z});
    
    // Si la distancia es menor que la suma de radios, hay choque
    return dist < (collisionRadius + playerRadius);
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