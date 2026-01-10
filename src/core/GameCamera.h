#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include "raylib.h"
#include "raymath.h"

class GameCamera {
public:
    GameCamera();
    void Update(Vector3 targetPosition);
    Camera3D getCamera();

private:
    Camera3D camera;
    float distance; // Qué tan lejos estamos del barco
    float height;   // Qué tan alto
    float angle;    // Ángulo de rotación alrededor del barco
    float zoomLevel;
    float targetZoom;
    Vector3 offset; // Distancia fija respecto al objetivo
};

#endif