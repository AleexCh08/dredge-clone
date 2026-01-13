#ifndef BOAT_H
#define BOAT_H

#include "raylib.h"
#include "core/Inventory.h"
#include "FishingSpot.h"
#include "Port.h"
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
    void Draw(); 

    bool CaptureFish(const FishType& fish);
    void FailFishing();

    int GetFishCount() { return fishCount; }

    void StartFishing(Vector3 targetPos); //  Activa el sedal
    void StopFishing();
    void ResolvePortCollision(Port* port);

    void ShowFeedback(const char* text, Color color);

    void DrawUI(Camera3D camera);
    Vector3 getPosition(); // Para que la cámara nos pueda seguir

    Inventory* GetInventory() { return &inventory; } // Acceder al inventario

    void ToggleLight() { isLightOn = !isLightOn; } // Luz/linterna
    bool IsLightOn() const { return isLightOn; }

private:
    Vector3 position;
    float rotation; // En grados
    float speed;
    float rotationSpeed;
    float currentTurnSpeed;
    bool isLightOn;
    
    // Configuraciones del barco
    const float MAX_SPEED = 10.0f;
    const float ACCELERATION = 5.0f;
    const float MAX_TURN_SPEED = 35.0f;   // Gira más lento 
    const float TURN_ACCEL = 40.0f;       // Qué tan rápido empieza a girar
    const float TURN_DECAY = 0.90f;       // Qué tan rápido deja de girar al soltar (Fricción)
    const float FRICTION = 0.98f;

    std::deque<WakeParticle> wakeTrail; // Lista de partículas
    float wakeTimer; // Para no soltar partículas en cada frame

    float tiltAngle;       // Inclinación lateral actual (Roll)
    float targetTilt;      // A dónde queremos inclinarnos
    float pitchAngle;      // Inclinación frontal (Pitch)

    bool isFishingLineActive; // Sedal
    Vector3 fishingLineTarget;
    
    int fishCount;
    bool showFeedback;
    float feedbackTimer;
    const char* feedbackText;
    Vector3 feedbackPos;
    Color feedbackColor;

    Inventory inventory; // Nuevo objeto inventario

    const float MAP_LIMIT = 90.0f; // El radio máximo donde puedes navegar
    void CheckMapBounds();
    void BounceBack(Vector3 direction);
};

#endif