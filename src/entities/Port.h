#ifndef PORT_H
#define PORT_H

#include "raylib.h"
#include "../core/Inventory.h"

class Port {
public:
    Port(Vector3 pos);
    
    void Draw();
    
    bool IsPlayerInside(Vector3 playerPos);
    bool CheckCollision(Vector3 playerPos, float playerRadius);
    Vector3 GetPosition() const { return position; }
    Inventory* GetStorage() { return &storage; } // Inventario de almacen

private:
    Vector3 position;
    float dockRadius; // Qué tan cerca hay que estar para atracar
    float collisionRadius;
    Inventory storage;
};

#endif