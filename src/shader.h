#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();
    
    bool init(const char* vertexPath, const char* fragmentPath);
    void use();
    void cleanup();
    
    // Utility functions for setting uniforms
    void setMat4(const std::string &name, const glm::mat4 &mat);
    void setVec3(const std::string &name, const glm::vec3 &value);
    void setFloat(const std::string &name, float value);

private:
    GLuint programId;
    bool checkCompileErrors(GLuint shader, const std::string& type);
}; 