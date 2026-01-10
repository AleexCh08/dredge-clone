cmake_minimum_required(VERSION 3.11)

project(DredgeClone)

set(CMAKE_CXX_STANDARD 17)

# --- CARGA DE RAYLIB ---
include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(raylib)

# --- DEFINICIÓN DE FUENTES ---
# Ahora listamos los archivos con su ruta relativa nueva
file(GLOB_RECURSE SOURCES "src/*.cpp") 
# NOTA: GLOB_RECURSE busca automáticamente todos los .cpp dentro de src/ y subcarpetas.
# Es muy cómodo para desarrollo, aunque en producción a veces se listan a mano.

# --- EJECUTABLE ---
add_executable(${PROJECT_NAME} ${SOURCES})

# --- INCLUDES ---
# Esto permite que C++ encuentre los .h dentro de la carpeta src
target_include_directories(${PROJECT_NAME} PRIVATE src)

# --- LINKING ---
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(MSWIN)
    target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
endif()

# --- COPIAR ASSETS ---
# Copia la carpeta assets al directorio donde se crea el ejecutable
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})