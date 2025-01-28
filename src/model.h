#pragma once

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Model {
public:
    Model();
    ~Model();

    bool loadFromFile(const char* path);
    void draw(Shader& shader);
    void cleanup();

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint VAO, VBO, EBO;
    glm::vec3 position;

    void setupMesh();
}; 