# SDL OBJ Viewer 🖼️

Un visualiseur 3D simple utilisant SDL2 et OpenGL pour afficher des modèles OBJ

## Fonctionnalités

- Chargement de modèles OBJ avec normales et coordonnées de texture
- Déplacement de caméra libre (WASD + souris)
- Rotation automatique du modèle
- Éclairage dynamique (diffus, ambiant, spéculaire)
- Support des shaders GLSL

## Dépendances

- SDL2
- GLEW
- OpenGL
- GLM

### Installation sur macOS

```bash
brew install sdl2 glew glm
```

## Construction

```bash
git clone https://github.com/ernivani/sdl-obj-viewer.git
cd sdl-obj-viewer
make
```

## Utilisation

```bash
# Lancer avec un modèle OBJ
./build/bin/sdl_obj_viewer ./build/coolguy.obj

# Contrôles :
- WASD : Déplacement avant/arrière/gauche/droite
- Espace/LShift : Monter/descendre
- Clic gauche + souris : Regarder autour
- Molette : Zoom avant/arrière
```

## Structure du projet

```
.
├── CMakeLists.txt    # Configuration CMake
├── Makefile          # Script de build
├── src/              # Code source
│   ├── main.cpp      # Point d'entrée
│   ├── model.[h/cpp] # Gestion des modèles 3D
│   └── shader.[h/cpp]# Gestion des shaders OpenGL
└── assets/           # Modèles et textures (à créer)
```
