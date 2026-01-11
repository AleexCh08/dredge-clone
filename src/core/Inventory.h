#ifndef INVENTORY_H
#define INVENTORY_H

#include "raylib.h"
#include <vector>
#include <string>

// Configuración de la Grilla (El "Tablero")
#define MAX_INV_COLS 12
#define MAX_INV_ROWS 10
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
    Inventory(int cols = 6, int rows = 4);
    
    void Update();
    void Draw(int offsetX = 0, int offsetY = 0);
    void Toggle(); // Abrir/Cerrar con "I"
    
    bool IsOpen() const { return isOpen; }

    // EL ALGORITMO (Lo usaremos pronto)
    // Verifica si un item cabe en una posición específica (x, y)
    bool CanPlaceItem(int x, int y, int w, int h);
    
    // Intenta agregar un item automáticamente (busca hueco)
    bool AddItem(const char* name, int w, int h, Color color);

private:
    bool isOpen;
    int cols; 
    int rows;
    
    // MATRIZ LÓGICA: true = ocupado, false = libre
    bool collisionGrid[MAX_INV_COLS][MAX_INV_ROWS];
    
    // LISTA VISUAL: Los objetos que tenemos
    std::vector<InventoryItem> items;
};

#endif