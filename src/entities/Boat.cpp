#include "Boat.h"
#include "raymath.h"
#include "core/World.h"
#include <math.h>   
#include <rlgl.h>

Boat::Boat() {
    position = { 0.0f, 0.0f, 0.0f };
    rotation = 0.0f;
    speed = 0.0f;
    wakeTimer = 0.0f;
    tiltAngle = 0.0f;
    targetTilt = 0.0f;
    pitchAngle = 0.0f;
    currentTurnSpeed = 0.0f;
}

void Boat::CheckMapBounds() {
    // Calculamos la distancia desde el centro (0,0)
    float distance = Vector2Length((Vector2){position.x, position.z});

    // Si nos pasamos del límite...
    if (distance > MAP_LIMIT) {
        // Empujamos el barco de regreso hacia el centro
        // Normalizamos el vector de posición para saber la dirección de retorno
        Vector3 directionToCenter = Vector3Normalize(Vector3Scale(position, -1.0f));
        
        // Empuje correctivo
        position.x += directionToCenter.x * 0.5f;
        position.z += directionToCenter.z * 0.5f;
        
        // Matamos la velocidad para que se sienta el "choque"
        speed *= 0.5f; 
    }
}

void Boat::Update() {
    // 1. INPUT DEL USUARIO
    // Rotación (A / D) CON INERCIA
    if (IsKeyDown(KEY_A)) {
        currentTurnSpeed += TURN_ACCEL * GetFrameTime();
    } else if (IsKeyDown(KEY_D)) {
        currentTurnSpeed -= TURN_ACCEL * GetFrameTime();
    } else {
        // Si no tocas nada, la velocidad de giro decae suavemente
        currentTurnSpeed *= TURN_DECAY;
    }

    // Limitar la velocidad de giro máxima (Clamp)
    if (currentTurnSpeed > MAX_TURN_SPEED) currentTurnSpeed = MAX_TURN_SPEED;
    if (currentTurnSpeed < -MAX_TURN_SPEED) currentTurnSpeed = -MAX_TURN_SPEED;

    // Aplicar el giro a la rotación real
    rotation += currentTurnSpeed * GetFrameTime();
    // Calculamos el Target Tilt
    targetTilt = 0.0f;
    if (IsKeyDown(KEY_A)) targetTilt = 15.0f;  // Girar izq -> Inclinarse der
    if (IsKeyDown(KEY_D)) targetTilt = -15.0f; // Girar der -> Inclinarse izq

    // Aceleración (W / S)
    if (IsKeyDown(KEY_W)) {
        speed += ACCELERATION * GetFrameTime();
    } else if (IsKeyDown(KEY_S)) {
        speed -= ACCELERATION * GetFrameTime();
    } else {
        // Fricción del agua (frena suavemente si no tocas nada)
        speed *= FRICTION;
    }

    // 1. Suavizar el balanceo lateral (Lerp)
    float tiltSpeed = 2.0f * GetFrameTime();
    tiltAngle += (targetTilt - tiltAngle) * tiltSpeed;
    float targetPitch = (speed / MAX_SPEED) * -5.0f;
    
    // Suavizamos el pitch también
    pitchAngle += (targetPitch - pitchAngle) * 2.0f * GetFrameTime();

    // Limites de velocidad
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    if (speed < -MAX_SPEED / 2) speed = -MAX_SPEED / 2; // Reversa más lenta
  
    // Calculamos la altura Y basándonos en nuestra posición X
    // Nota: Sumamos 0.5f extra para que el barco no se hunda a la mitad, sino que flote sobre la ola
    position.x += sin(rotation * DEG2RAD) * speed * GetFrameTime();
    position.z += cos(rotation * DEG2RAD) * speed * GetFrameTime();

    CheckMapBounds();

    // EFECTO DE OLEAJE (FLOTAR)
    // Usamos el tiempo y la posición X para que el barco suba y baje suavemente
    float time = GetTime();
    float targetY = sin(position.x * WaterConstants::frequency + time * WaterConstants::speed) * WaterConstants::amplitude + 0.2f;
    float damping = 5.0f;
    position.y += (targetY - position.y) * damping * GetFrameTime(); 

    if (fabs(speed) > 1.0f) {
        wakeTimer += GetFrameTime();
        if (wakeTimer > 0.1f) { // Soltar una partícula cada 0.1 segundos
            WakeParticle p;
            // Nace justo detrás del barco (un poco abajo para que esté en el agua)
            p.position = position;
            p.position.y = 0.1f; // Casi al nivel del mar
            p.alpha = 1.0f; // Totalmente visible
            p.size = 0.5f;
            
            wakeTrail.push_back(p);
            wakeTimer = 0.0f;
            
            // Limitar cantidad de partículas (Optimización)
            if (wakeTrail.size() > 50) wakeTrail.pop_front();
        }
    }

    // Actualizar partículas existentes (Desvanecer)
    for (auto &p : wakeTrail) {
        p.alpha -= 0.5f * GetFrameTime(); // Se desvanecen en 2 segundos aprox
        p.size += 0.5f * GetFrameTime();  // Se expanden un poco
    }

    // Limpiar partículas muertas (Alpha <= 0)
    if (!wakeTrail.empty() && wakeTrail.front().alpha <= 0) {
        wakeTrail.pop_front();
    }
}

void Boat::Draw() {
    // --- DIBUJAR ESTELA (CORREGIDO) ---
    // Traemos las constantes de la ola para calcular la altura
    float time = GetTime();
    float frequency = 2.0f; 
    float amplitude = 0.5f;
    float waveSpeed = 2.0f;

    for (const auto &p : wakeTrail) {
        // Calculamos la altura de la ola en la posición DE LA PARTÍCULA (p.position.x)
        float waveHeight = sin(p.position.x * frequency + time * waveSpeed) * amplitude;
        
        // La dibujamos justo encima de la ola (+0.1f para evitar parpadeo)
        Vector3 drawPos = p.position;
        drawPos.y = waveHeight + 0.1f;

        DrawCube(drawPos, p.size, 0.05f, p.size, Fade(WHITE, p.alpha));
    }

    // Dibujar el cuerpo del barco
    // PushMatrix guarda el estado actual para que rotar el barco no rote todo el mundo
    rlPushMatrix();
        rlTranslatef(position.x, position.y, position.z);
        rlRotatef(rotation, 0, 1, 0); // Rotar sobre el eje Y
        rlRotatef(pitchAngle, 1, 0, 0);
        rlRotatef(tiltAngle, 0, 0, 1);

        // El barco (Un cubo rojo alargado por ahora)
        DrawCube((Vector3){0, 0.5f, 0}, 1.0f, 1.0f, 2.0f, RED);
        DrawCubeWires((Vector3){0, 0.5f, 0}, 1.0f, 1.0f, 2.0f, MAROON);
        DrawCube((Vector3){0, 1.0f, 0.5f}, 0.8f, 0.8f, 0.8f, DARKGRAY);

    rlPopMatrix();
}

Vector3 Boat::getPosition() {
    return position;
}