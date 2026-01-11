#include "Inventory.h"

Inventory::Inventory(int c, int r) {
    isOpen = false;
    cols = c;
    rows = r;

    if (cols > MAX_INV_COLS) cols = MAX_INV_COLS;
    if (rows > MAX_INV_ROWS) rows = MAX_INV_ROWS;

    // Inicializar la matriz vacía (todo false)
    for (int x = 0; x < MAX_INV_COLS; x++) {
        for (int y = 0; y < MAX_INV_ROWS; y++) {
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
    // Verificar límites de la matriz (Out of Bounds)
    if (startX < 0 || startY < 0) return false;
    if (startX + w > cols || startY + h > rows) return false;

    // Verificar colisión con otros items
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            if (collisionGrid[startX + i][startY + j]) {
                return false;
            }
        }
    }
    return true; 
}

bool Inventory::AddItem(const char* name, int w, int h, Color color) {
    // Algoritmo simple: Busca el primer hueco donde quepa
    // Recorremos la matriz celda por celda
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (CanPlaceItem(x, y, w, h)) {
                
                InventoryItem newItem;
                newItem.name = name; newItem.width = w; newItem.height = h; newItem.color = color;
                newItem.gridX = x; newItem.gridY = y;
                items.push_back(newItem);

                for (int i = 0; i < w; i++) {
                    for (int j = 0; j < h; j++) {
                        collisionGrid[x + i][y + j] = true;
                    }
                }
                return true; 
            }
        }
    }
    return false; // Inventario lleno
}

void Inventory::Draw(int offsetX, int offsetY) {
    if (!isOpen) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    int totalW = (cols * CELL_SIZE) + (PADDING * 2);
    int totalH = (rows * CELL_SIZE) + (PADDING * 2);
    
    // Si no hay offset manual, centramos en pantalla
    int startX = (offsetX == 0) ? (screenW - totalW) / 2 : offsetX;
    int startY = (offsetY == 0) ? (screenH - totalH) / 2 : offsetY;

    // Fondo
    DrawRectangle(startX, startY, totalW, totalH, Fade(DARKBLUE, 0.9f));
    DrawRectangleLines(startX, startY, totalW, totalH, WHITE);
    
    // Grilla
    int gridStartX = startX + PADDING;
    int gridStartY = startY + PADDING;

    for (int x = 0; x < cols; x++) { // Usar cols
        for (int y = 0; y < rows; y++) { // Usar rows
            DrawRectangleLines(
                gridStartX + (x * CELL_SIZE), 
                gridStartY + (y * CELL_SIZE), 
                CELL_SIZE, CELL_SIZE, 
                Fade(WHITE, 0.3f)
            );
        }
    }

    // Items
    for (const auto& item : items) {
        int itemX = gridStartX + (item.gridX * CELL_SIZE);
        int itemY = gridStartY + (item.gridY * CELL_SIZE);
        int itemW = item.width * CELL_SIZE;
        int itemH = item.height * CELL_SIZE;

        DrawRectangle(itemX + 2, itemY + 2, itemW - 4, itemH - 4, item.color);
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

// Detectar si el mouse está sobre un ítem
int Inventory::GetItemIndexUnderMouse(int offsetX, int offsetY) {
    if (!isOpen) return -1;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int totalW = (cols * CELL_SIZE) + (PADDING * 2);
    int totalH = (rows * CELL_SIZE) + (PADDING * 2);
    
    int startX = (offsetX == 0) ? (screenW - totalW) / 2 : offsetX;
    int startY = (offsetY == 0) ? (screenH - totalH) / 2 : offsetY;
    int gridStartX = startX + PADDING;
    int gridStartY = startY + PADDING;

    Vector2 mouse = GetMousePosition();

    // Revisar cada ítem para ver si el mouse está encima
    for (int i = 0; i < items.size(); i++) {
        InventoryItem& item = items[i];
        
        int itemX = gridStartX + (item.gridX * CELL_SIZE);
        int itemY = gridStartY + (item.gridY * CELL_SIZE);
        int itemW = item.width * CELL_SIZE;
        int itemH = item.height * CELL_SIZE;

        Rectangle rect = { (float)itemX, (float)itemY, (float)itemW, (float)itemH };

        if (CheckCollisionPointRec(mouse, rect)) {
            return i; // ¡Encontramos el ítem número i!
        }
    }
    return -1; 
}

InventoryItem Inventory::GetItem(int index) {
    return items[index];
}

void Inventory::RemoveItem(int index) {
    if (index < 0 || index >= items.size()) return;

    InventoryItem item = items[index];

    for (int i = 0; i < item.width; i++) {
        for (int j = 0; j < item.height; j++) {
            collisionGrid[item.gridX + i][item.gridY + j] = false;
        }
    }

    items.erase(items.begin() + index);
}