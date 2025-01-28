#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "model.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Camera parameters
struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f;
    float movementSpeed = 2.5f;
    float mouseSensitivity = 0.1f;
    float scrollSensitivity = 1.0f;
};

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    // Visualize normals (temporary debug)
    FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
    return;

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
)";

bool init_gl_shaders(Shader& shader) {
    // Create temporary files for shaders
    FILE* vertexFile = fopen("temp_vertex.glsl", "w");
    FILE* fragmentFile = fopen("temp_fragment.glsl", "w");
    if (!vertexFile || !fragmentFile) {
        std::cerr << "Failed to create temporary shader files" << std::endl;
        return false;
    }
    
    fprintf(vertexFile, "%s", vertexShaderSource);
    fprintf(fragmentFile, "%s", fragmentShaderSource);
    
    fclose(vertexFile);
    fclose(fragmentFile);

    // Initialize shader
    bool success = shader.init("temp_vertex.glsl", "temp_fragment.glsl");
    
    // Clean up temporary files
    remove("temp_vertex.glsl");
    remove("temp_fragment.glsl");
    
    return success;
}

void processInput(Camera& camera, float deltaTime, const Uint8* keystate) {
    float velocity = camera.movementSpeed * deltaTime;
    
    if (keystate[SDL_SCANCODE_W])
        camera.position += camera.front * velocity;
    if (keystate[SDL_SCANCODE_S])
        camera.position -= camera.front * velocity;
    if (keystate[SDL_SCANCODE_A])
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    if (keystate[SDL_SCANCODE_D])
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    if (keystate[SDL_SCANCODE_SPACE])
        camera.position += camera.up * velocity;
    if (keystate[SDL_SCANCODE_LSHIFT])
        camera.position -= camera.up * velocity;

    
}

void processMouseMovement(Camera& camera, float xoffset, float yoffset, bool constrainPitch = true) {
    xoffset *= camera.mouseSensitivity;
    yoffset *= camera.mouseSensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    if (constrainPitch) {
        if (camera.pitch > 89.0f)
            camera.pitch = 89.0f;
        if (camera.pitch < -89.0f)
            camera.pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    front.y = sin(glm::radians(camera.pitch));
    front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    camera.front = glm::normalize(front);
}

void processMouseScroll(Camera& camera, float yoffset) {
    camera.zoom -= yoffset * camera.scrollSensitivity;
    if (camera.zoom < 1.0f)
        camera.zoom = 1.0f;
    if (camera.zoom > 45.0f)
        camera.zoom = 45.0f;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_obj_file>" << std::endl;
        return 1;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window
    SDL_Window* window = SDL_CreateWindow("SDL OBJ Viewer",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return 1;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Create and initialize shader
    Shader shader;
    if (!init_gl_shaders(shader)) {
        std::cerr << "Failed to initialize shaders" << std::endl;
        return 1;
    }

    // Load model
    Model model;
    if (!model.loadFromFile(argv[1])) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }

    // Initialize camera
    Camera camera;
    camera.position = glm::vec3(0.0f, 0.0f, 5.0f);  // Start further back
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 lightPos(0.0f, 3.0f, 3.0f);  // Move light higher
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));  // White light
    shader.setVec3("objectColor", glm::vec3(0.7f, 0.5f, 0.3f));  // More distinct color

    // Mouse state
    bool firstMouse = true;
    bool mousePressed = false;
    float lastX = WINDOW_WIDTH / 2.0f;
    float lastY = WINDOW_HEIGHT / 2.0f;

    // Timing
    Uint32 lastFrame = SDL_GetTicks();

    bool quit = false;
    SDL_Event e;

    // Capture mouse
    SDL_SetRelativeMouseMode(SDL_FALSE);

    static float rotation = 0.0f;

    while (!quit) {
        // Calculate delta time
        Uint32 currentFrame = SDL_GetTicks();
        float deltaTime = (currentFrame - lastFrame) / 1000.0f;
        lastFrame = currentFrame;

        rotation += deltaTime * 50.0f;  // 50 degrees per second

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = true;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = false;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                }
            }
            else if (e.type == SDL_MOUSEMOTION && mousePressed) {
                processMouseMovement(camera, e.motion.xrel, -e.motion.yrel);
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                processMouseScroll(camera, e.wheel.y);
            }
        }

        // Process keyboard input
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        processInput(camera, deltaTime, keystate);

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        // Set up view and projection matrices
        glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

        // Set uniforms
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
        shader.setMat4("model", modelMatrix);

        shader.setVec3("viewPos", camera.position);

        // Draw model
        model.draw(shader);

        // Swap buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    model.cleanup();
    shader.cleanup();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
} 