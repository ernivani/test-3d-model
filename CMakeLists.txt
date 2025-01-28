cmake_minimum_required(VERSION 3.10)
project(sdl_obj_viewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Configuration des paquets
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

# Configuration des include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    /opt/homebrew/include
)

# Fichiers sources
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

# Configuration de l'exécutable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Liaisons des bibliothèques
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    glm::glm
)

# Installation pour macOS
if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework OpenGL")
endif()