#include "Inventory.h"

Inventory::Inventory() {
    isOpen = false;

    // Inicializar la matriz vacía (todo false)
    for (int x = 0; x < INV_COLS; x++) {
        for (int y = 0; y < INV_ROWS; y++) {
            collisionGrid[x][y] = false;
        }
    }
}

void Inventory::Toggle() {
    isOpen = !isOpen;
}

void Inventory::Update() {
    // Aquí luego manejaremos el Drag & Drop del mouse
}

// EL ALGORITMO CORE (Fase CS)
bool Inventory::CanPlaceItem(int startX, int startY, int w, int h) {
    // 1. Verificar límites de la matriz (Out of Bounds)
    if (startX < 0 || startY < 0) return false;
    if (startX + w > INV_COLS || startY + h > INV_ROWS) return false;

    // 2. Verificar colisión con otros items
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            // Si la celda [x+i][y+j] ya es true, chocamos
            if (collisionGrid[startX + i][startY + j]) {
                return false;
            }
        }
    }
    return true; // Todo limpio
}

bool Inventory::AddItem(const char* name, int w, int h, Color color) {
    // Algoritmo simple: Busca el primer hueco donde quepa
    // Recorremos la matriz celda por celda
    for (int y = 0; y < INV_ROWS; y++) {
        for (int x = 0; x < INV_COLS; x++) {
            
            if (CanPlaceItem(x, y, w, h)) {
                // ¡Encontramos hueco!
                
                // 1. Crear el item
                InventoryItem newItem;
                newItem.name = name;
                newItem.width = w;
                newItem.height = h;
                newItem.color = color;
                newItem.gridX = x;
                newItem.gridY = y;
                
                items.push_back(newItem);

                // 2. Marcar la matriz como ocupada
                for (int i = 0; i < w; i++) {
                    for (int j = 0; j < h; j++) {
                        collisionGrid[x + i][y + j] = true;
                    }
                }
                
                return true; // Éxito
            }
        }
    }
    return false; // Inventario lleno
}

void Inventory::Draw() {
    if (!isOpen) return;

    // Calcular posición para centrar en pantalla
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    int totalW = (INV_COLS * CELL_SIZE) + (PADDING * 2);
    int totalH = (INV_ROWS * CELL_SIZE) + (PADDING * 2);
    
    int startX = (screenW - totalW) / 2;
    int startY = (screenH - totalH) / 2;

    // 1. Fondo del Panel
    DrawRectangle(startX, startY, totalW, totalH, Fade(DARKBLUE, 0.9f));
    DrawRectangleLines(startX, startY, totalW, totalH, WHITE);
    
    // Título
    DrawText("INVENTARIO (CARGO)", startX + 10, startY - 30, 20, WHITE);

    // 2. Dibujar la Grilla (Líneas)
    int gridStartX = startX + PADDING;
    int gridStartY = startY + PADDING;

    for (int x = 0; x < INV_COLS; x++) {
        for (int y = 0; y < INV_ROWS; y++) {
            DrawRectangleLines(
                gridStartX + (x * CELL_SIZE), 
                gridStartY + (y * CELL_SIZE), 
                CELL_SIZE, CELL_SIZE, 
                Fade(WHITE, 0.3f)
            );
        }
    }

    // 3. Dibujar los Items
    for (const auto& item : items) {
        int itemX = gridStartX + (item.gridX * CELL_SIZE);
        int itemY = gridStartY + (item.gridY * CELL_SIZE);
        int itemW = item.width * CELL_SIZE;
        int itemH = item.height * CELL_SIZE;

        // Cuerpo del item
        DrawRectangle(itemX + 2, itemY + 2, itemW - 4, itemH - 4, item.color);
        
        // Nombre (recortado si es largo)
        DrawText(item.name.c_str(), itemX + 5, itemY + 5, 10, WHITE);
    }

    // Dibujamos un cursor
    Vector2 mousePos = GetMousePosition();   
    Vector2 v1 = mousePos; 
    Vector2 v2 = { mousePos.x, mousePos.y + 20 };
    Vector2 v3 = { mousePos.x + 15, mousePos.y + 15 };
    DrawTriangle(v1, v2, v3, WHITE);       
    DrawTriangleLines(v1, v2, v3, BLACK);
}