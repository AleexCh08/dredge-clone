#include "FishingMinigame.h"
#include "raymath.h"

FishingMinigame::FishingMinigame() {
    active = false;
    rotationAngle = 0.0f;
    rotationSpeed = 150.0f; // Grados por segundo
}

void FishingMinigame::Start() {
    active = true;
    rotationAngle = 0.0f;
    
    // Configuramos una zona verde aleatoria
    // Ejemplo: Entre el grado 90 y el 270
    targetStartAngle = (float)GetRandomValue(90, 270); 
    targetSize = 40.0f; // Tamaño de la zona de éxito
}

void FishingMinigame::Update() {
    if (!active) return;

    // Girar la aguja
    rotationAngle += rotationSpeed * GetFrameTime();
    
    // Si da la vuelta completa (360), reseteamos a 0
    if (rotationAngle >= 360.0f) {
        rotationAngle -= 360.0f;
    }

    // Input: Salir con Escape (Cancelar pesca)
    if (IsKeyPressed(KEY_ESCAPE)) {
        active = false;
    }
}

void FishingMinigame::Draw() {
    if (!active) return;

    Vector2 center = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    float radius = 100.0f;

    // 1. Fondo Oscuro (Para que resalte)
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

    // 2. El Anillo Base (Gris oscuro)
    // startAngle, endAngle, segments
    DrawRing(center, radius - 10, radius, 0, 360, 0, Fade(LIGHTGRAY, 0.5f));

    // 3. La Zona de Éxito (Verde)
    // DrawRing usa ángulos en sentido antihorario a veces, ajustamos coordenadas
    DrawRing(center, radius - 15, radius + 5, targetStartAngle, targetStartAngle + targetSize, 0, GREEN);

    // 4. La Aguja (Línea Roja que gira)
    // Convertimos ángulo a radianes para calcular la posición X,Y
    float rad = rotationAngle * DEG2RAD;
    Vector2 needleEnd;
    
    // Ajuste matemático: Raylib dibuja 0 grados a la derecha (Eje X), 
    // pero queremos que gire en sentido horario o antihorario consistente.
    // Usamos sin/cos básicos.
    needleEnd.x = center.x + cos(rad) * (radius - 5);
    needleEnd.y = center.y + sin(rad) * (radius - 5);

    DrawLineEx(center, needleEnd, 3.0f, RED);
    DrawCircleV(center, 5.0f, RED); // Punto central

    // Texto de ayuda
    DrawText("ESPACIO para pescar", center.x - 60, center.y + 120, 20, WHITE);
}