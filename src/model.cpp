#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

Model::Model() : VAO(0), VBO(0), EBO(0), position(0.0f, 0.0f, 0.0f) {
    printf("Model constructor\n");

    printf("Model position: %f, %f, %f\n", position.x, position.y, position.z);
}

Model::~Model() {
    cleanup();
}

bool Model::loadFromFile(const char* path) {
    std::cout << "Attempting to load model from: " << path << std::endl;
    // Add file extension check
    std::string filename(path);
    if (filename.substr(filename.find_last_of(".") + 1) != "obj") {
        std::cerr << "ERROR: Only Wavefront .obj files are supported\n";
        return false;
    }
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR: Failed to open file: " << path << " (Current working directory: " << std::filesystem::current_path() << ")" << std::endl;
        return false;
    }

    // Add counters for debug
    int vertexCount = 0;
    int faceCount = 0;
    int lineCount = 0;

    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texcoords;

    std::string line;
    while (std::getline(file, line)) {
        lineCount++;
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            vertexCount++;
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            temp_normals.push_back(normal);
        }
        else if (type == "vt") {
            glm::vec2 tex;
            iss >> tex.x >> tex.y;
            temp_texcoords.push_back(tex);
        }
        else if (type == "f") {
            faceCount++;
            std::vector<std::string> faceVertices;
            std::string vertex;
            while (iss >> vertex) {
                faceVertices.push_back(vertex);
            }

            // Triangulate faces (convert quads to triangles)
            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                // Process 3 vertices per triangle
                for (int j = 0; j < 3; j++) {
                    size_t idx = (j == 0) ? 0 : (i + j - 1);
                    std::string& v = faceVertices[idx];
                    
                    std::replace(v.begin(), v.end(), '/', ' ');
                    std::istringstream viss(v);
                    
                    unsigned int pos_idx = 0, tex_idx = 0, norm_idx = 0;
                    viss >> pos_idx;
                    if (!viss.eof()) viss >> tex_idx;
                    if (!viss.eof()) viss >> norm_idx;

                    // Handle indices with validation
                    Vertex vert;
                    if (pos_idx > 0 && pos_idx <= temp_positions.size()) {
                        vert.position = temp_positions[pos_idx - 1];
                    } else {
                        std::cerr << "Invalid position index: " << pos_idx << std::endl;
                        continue;
                    }

                    if (norm_idx > 0 && norm_idx <= temp_normals.size()) {
                        vert.normal = temp_normals[norm_idx - 1];
                    } else {
                        vert.normal = glm::vec3(0.0f);
                    }

                    if (tex_idx > 0 && tex_idx <= temp_texcoords.size()) {
                        vert.texCoords = temp_texcoords[tex_idx - 1];
                    } else {
                        vert.texCoords = glm::vec2(0.0f);
                    }

                    vertices.push_back(vert);
                    indices.push_back(static_cast<unsigned int>(indices.size()));
                }
            }
        }
    }

    std::cout << "Parsing results:\n"
              << "Lines processed: " << lineCount << "\n"
              << "Vertices found: " << vertexCount << "\n"
              << "Faces found: " << faceCount << "\n"
              << "Vertices loaded: " << vertices.size() << "\n"
              << "Indices loaded: " << indices.size() << std::endl;

    setupMesh();
    return true;
}

void Model::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Model::draw(Shader& shader) {
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Model::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
} 