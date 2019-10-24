# pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader();
    Shader(std::string vFile, std::string fFile);
    Shader(std::string vFile, std::string fFile, std::string gFile);
    // activate shader program
    void use();

    // uniform tool kits
    //// single vars
    void setUniform(const std::string &varName, const int& value) const;
    void setUniform(const std::string &varName, const float& value) const;
    void setUniform(const std::string &varName, const int& value0, const int& value1) const;
    void setUniform(const std::string &varName, const float& value0, const float& value1) const;
    void setUniform(const std::string &varName, const int& value0, const int& value1, const int& value2) const;
    void setUniform(const std::string &varName, const float& value0, const float& value1, const float& value2) const;
    void setUniform(const std::string &varName, const int& value0, const int& value1, const int& value2, const int& value3) const;
    void setUniform(const std::string &varName, const float& value0, const float& value1, const float& value2, const float& value3) const;
    //// vectors
    void setUniform(const std::string &varName, const glm::vec2& vector) const;
    void setUniform(const std::string &varName, const glm::vec3& vector) const;
    void setUniform(const std::string &varName, const glm::vec4& vector) const;
    //// matrix
    void setUniform(const std::string &varName, const glm::mat4& mat) const;
    void setUniform(const std::string &varName, const glm::mat3& mat) const;
    void setUniform(const std::string &varName, const glm::mat2& mat) const;

    // duplicated
    void setUniform(const std::string &varName, float * const& matPtr, const int& matDim) const;

private:
    unsigned int shaderID;
};