<div align="center">
  <img src="https://img.shields.io/badge/Estado-En_Proceso-orange?style=for-the-badge&logo=gear" alt="En Proceso" />
</div>

# Dredge Clone Project

Un juego de pesca y supervivencia con un fuerte componente de horror psicológico y gestión de recursos, desarrollado en C++ utilizando la biblioteca gráfica Raylib. El juego simula físicas marinas, iluminación dinámica y un sistema de cordura que altera el mundo que rodea al jugador.

---

## ⚙️ Tecnologías Utilizadas

*   **Lenguaje:** C++
*   **Motor Gráfico:** Raylib
*   **Shaders:** GLSL (Vertex y Fragment shaders personalizados)
*   **Construcción:** CMake

---

## 🚀 Características y Mecánicas Implementadas

*   **Navegación y Físicas:** Control del barco con inercia, fricción, balanceo dinámico sincronizado con el oleaje y sistema de partículas para la estela en el agua.
*   **Ciclo Día/Noche:** Transición fluida del cielo y la iluminación (Amanecer, Día, Atardecer, Noche Cerrada) atada a un reloj global interno.
*   **Iluminación Avanzada (Shaders):** Sistema de luz direccional (faro en forma de cono) y luces ambientales calculadas directamente en la GPU.
*   **Niebla Dinámica:** Niebla volumétrica que oculta el horizonte y reacciona a la linterna del jugador, disipándose dentro del cono de luz.
*   **Gestión de Inventario:** Interfaz de cuadrícula (grid) que permite arrastrar, soltar y rotar objetos (peces, materiales). Soporta la transferencia de ítems entre el barco y el almacén del puerto.
*   **Pesca:** Zonas de actividad generadas procedimentalmente y un minijuego interactivo para la captura de peces.
*   **Sistema de Cordura (Pánico):** Nivel de estrés mental que aumenta durante la noche y fuera de zonas seguras. Representado visualmente por la interfaz dinámica de "El Ojo".
*   **Alucinaciones:** Generación dinámica de obstáculos fantasma frente al jugador cuando el pánico alcanza niveles críticos.
*   **Sistema de Daño:** Casco del barco con 3 puntos de vida, penalización de velocidad al chocar y retroceso (knockback).
*   **Economía y Puerto Seguro:** Zonas de atraque donde el pánico se detiene. Permite dormir para reducir el estrés (acelerando el tiempo temporalmente), acceder al almacén y gastar dinero en reparar el casco.

---

## 🎮 Controles del Jugador

*   **W / S:** Acelerar / Frenar.
*   **A / D:** Girar a babor / estribor.
*   **F:** Encender / Apagar el faro direccional.
*   **E:** Interactuar (Iniciar pesca, atracar en el puerto).
*   **I / TAB:** Abrir / Cerrar inventario.
*   **Click Izquierdo (Mantenido):** Arrastrar objetos en el inventario.
*   **Click Derecho:** Transferencia rápida de objetos (Barco <-> Almacén).
*   **R:** Rotar objeto (en inventario) / Reparar casco (en puerto).
*   **Z (Mantener):** Dormir y acelerar el tiempo (solo en el puerto).
*   **1:** Abrir almacén del puerto.
*   **ESC:** Salir de los menús o cerrar el juego.

---

## 👨‍💻 Desarrollado por

**AleexCh** 
Desarrollador Full Stack y Estudiante de Licenciatura en Computación en la Universidad Central de Venezuela (UCV).
