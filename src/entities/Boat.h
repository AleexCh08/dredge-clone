#ifndef BOAT_H
#define BOAT_H

#include "raylib.h"
#include <deque>

// Estructura pequeña para cada "pedazo" de espuma
struct WakeParticle {
    Vector3 position;
    float alpha; // Transparencia (se va desvaneciendo)
    float size;
};

class Boat {
public:
    Boat(); // Constructor
    void Update(bool inputEnabled); // Lógica (Input, Física)
    void Draw();   // Dibujado
    void AddFish() { fishCount++; }
    int GetFishCount() { return fishCount; }

    Vector3 getPosition(); // Para que la cámara nos pueda seguir

private:
    Vector3 position;
    float rotation; // En grados
    float speed;
    float rotationSpeed;
    float currentTurnSpeed;
    
    // Configuraciones del barco
    const float MAX_SPEED = 10.0f;
    const float ACCELERATION = 5.0f;
    const float MAX_TURN_SPEED = 35.0f;   // Bajamos de 60 a 35 (Gira más lento en general)
    const float TURN_ACCEL = 40.0f;       // Qué tan rápido empieza a girar
    const float TURN_DECAY = 0.90f;       // Qué tan rápido deja de girar al soltar (Fricción)
    const float FRICTION = 0.98f;

    std::deque<WakeParticle> wakeTrail; // Lista de partículas
    float wakeTimer; // Para no soltar partículas en cada frame

    float tiltAngle;       // Inclinación lateral actual (Roll)
    float targetTilt;      // A dónde queremos inclinarnos
    float pitchAngle;      // Inclinación frontal (Pitch)

    int fishCount;

    const float MAP_LIMIT = 90.0f; // El radio máximo donde puedes navegar
    void CheckMapBounds();
};

#endif