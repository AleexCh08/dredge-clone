#ifndef INVENTORY_H
#define INVENTORY_H

#include "raylib.h"
#include <vector>
#include <string>

// Configuración de la Grilla (El "Tablero")
#define INV_COLS 6
#define INV_ROWS 4
#define CELL_SIZE 60 
#define PADDING 10

// Estructura de un Item (El "Bloque de Tetris")
struct InventoryItem {
    std::string name;
    int width;  // Cuántas celdas ocupa a lo ancho
    int height; // Cuántas celdas ocupa a lo alto
    Color color;
    int gridX;  // Posición X en la grilla (0 a INV_COLS-1)
    int gridY;  // Posición Y en la grilla (0 a INV_ROWS-1)
};

class Inventory {
public:
    Inventory();
    
    void Update();
    void Draw();
    void Toggle(); // Abrir/Cerrar con "I"
    
    bool IsOpen() const { return isOpen; }

    // EL ALGORITMO (Lo usaremos pronto)
    // Verifica si un item cabe en una posición específica (x, y)
    bool CanPlaceItem(int x, int y, int w, int h);
    
    // Intenta agregar un item automáticamente (busca hueco)
    bool AddItem(const char* name, int w, int h, Color color);

private:
    bool isOpen;
    
    // MATRIZ LÓGICA: true = ocupado, false = libre
    bool collisionGrid[INV_COLS][INV_ROWS];
    
    // LISTA VISUAL: Los objetos que tenemos
    std::vector<InventoryItem> items;
};

#endif