#include "GameCamera.h"
#include <cmath>
#define MIN_HEIGHT 4.0f    // Muy bajito (3ra persona)
#define MAX_HEIGHT 16.0f   // Alto, pero no tanto para no ver bordes negros
#define MIN_DIST   12.0f   // Pegado al barco
#define MAX_DIST   15.0f   // Lejos

GameCamera::GameCamera() {
    camera = { 0 };
    
    // Configuración tipo "Dredge" (Vista isométrica)
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // El eje Y es "arriba"
    camera.fovy = 45.0f; // Campo de visión
    camera.projection = CAMERA_PERSPECTIVE;

    // Valores iniciales
    distance = 25.0f; // Distancia horizontal
    height = 25.0f;   // Altura fija
    angle = 3.14159f; // Ángulo inicial (detrás del barco)
    zoomLevel = 0.0f;
    targetZoom = 0.0f;

    offset = (Vector3){ 0.0f, 25.0f, 20.0f };
    
    // Posición inicial
    camera.position = offset;
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
}

void GameCamera::Update(Vector3 targetPosition) {
    // 1. INPUT ZOOM (SCROLL)
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        // Modificamos el OBJETIVO (targetZoom), no el zoom directo
        targetZoom -= wheel * 0.1f; // Un poco más de sensibilidad
        
        // Clamp del target
        if (targetZoom < 0.0f) targetZoom = 0.0f;
        if (targetZoom > 1.0f) targetZoom = 1.0f;
    }

    // 2. SUAVIZADO DEL ZOOM (NUEVO)
    zoomLevel = Lerp(zoomLevel, targetZoom, 5.0f * GetFrameTime());

    // 3. INPUT ROTACIÓN (MOUSE)
    float mouseDelta = GetMouseDelta().x;
    angle -= mouseDelta * 0.005f;
    if (angle > 2.0f * PI) angle -= 2.0f * PI;
    if (angle < 0.0f) angle += 2.0f * PI;

    // 4. CALCULAR POSICIÓN (Usamos zoomLevel, que ya está suavizado)
    float currentHeight = Lerp(MIN_HEIGHT, MAX_HEIGHT, zoomLevel);
    float currentDist   = Lerp(MIN_DIST, MAX_DIST, zoomLevel);

    // Calculamos coordenadas polares
    float offsetX = sin(angle) * currentDist;
    float offsetZ = cos(angle) * currentDist;

    // Asignamos posición
    camera.position.x = targetPosition.x + offsetX;
    camera.position.z = targetPosition.z + offsetZ;
    camera.position.y = targetPosition.y + currentHeight;

    // PUNTO DE MIRA (TARGET)
    float targetOffsetY = Lerp(2.0f, 0.0f, zoomLevel); 
    
    camera.target = targetPosition;
    camera.target.y += targetOffsetY; 
}

Camera3D GameCamera::getCamera() {
    return camera;
}