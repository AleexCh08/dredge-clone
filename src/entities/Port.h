#ifndef PORT_H
#define PORT_H

#include "raylib.h"

class Port {
public:
    Port(Vector3 pos);
    
    void Draw();
    bool IsPlayerInside(Vector3 playerPos);
    Vector3 GetPosition() const { return position; }

private:
    Vector3 position;
    float dockRadius; // Qué tan cerca hay que estar para atracar
};

#endif