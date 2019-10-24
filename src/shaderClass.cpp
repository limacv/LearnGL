#include "shaderClass.h"

Shader::Shader(): shaderID(NULL)
{
    
}

Shader::Shader(std::string vFile, std::string fFile)
{
    std::string vstrCode = "";
    std::string fstrCode = "";
    // read from vfile & fFile
    try
    {
        std::ifstream shaderFile(vFile);
        vstrCode = std::string(
            (std::istreambuf_iterator<char>(shaderFile)),
             std::istreambuf_iterator<char>()
            );
        shaderFile.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR while opening: " << vFile << std::endl;
    }
    const char* vchCode = vstrCode.c_str();

    try
    {
        std::ifstream shaderFile(fFile);
        fstrCode = std::string(
            (std::istreambuf_iterator<char>(shaderFile)),
             std::istreambuf_iterator<char>()
            );
        shaderFile.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR while opening: " << fFile << std::endl;
    }
    const char* fchCode = fstrCode.c_str();

    // compile
    unsigned int vshaderID, fshaderID;
    int success;
    char status[512];

    //// vertex shader
    vshaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshaderID, 1, &vchCode, NULL);
    glCompileShader(vshaderID);
    ////// check compile status
    glGetShaderiv(vshaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vshaderID, 512, NULL, status);
        std::cout << "ERROR when compiling: " << vFile << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS compilation: " << vFile << std::endl;
    }

    //// fragment shader
    fshaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshaderID, 1, &fchCode, NULL);
    glCompileShader(fshaderID);
    ////// check compile status
    glGetShaderiv(fshaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fshaderID, 512, NULL, status);
        std::cout << "ERROR when compiling: " << fFile << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS compilation: " << fFile << std::endl;
    }

    // create program & link
    this->shaderID = glCreateProgram();
    auto& id = this->shaderID;
    glAttachShader(id, vshaderID);
    glAttachShader(id, fshaderID);
    glLinkProgram(id);
    
    //// check link status
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(id, 512, NULL, status);
        std::cout << "ERROR when linking: " << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS link" << std::endl;
    }
        glDeleteShader(vshaderID);
        glDeleteShader(fshaderID);
}

Shader::Shader(std::string vFile, std::string fFile, std::string gFile)
{
    std::string vstrCode = "";
    std::string fstrCode = "";
    std::string gstrCode = "";
    // read from vfile & fFile & gFile
    try
    {
        std::ifstream shaderFile(vFile);
        vstrCode = std::string(
            (std::istreambuf_iterator<char>(shaderFile)),
             std::istreambuf_iterator<char>()
            );
        shaderFile.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR while opening: " << vFile << std::endl;
    }
    const char* vchCode = vstrCode.c_str();

    try
    {
        std::ifstream shaderFile(fFile);
        fstrCode = std::string(
            (std::istreambuf_iterator<char>(shaderFile)),
             std::istreambuf_iterator<char>()
            );
        shaderFile.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR while opening: " << fFile << std::endl;
    }
    const char* fchCode = fstrCode.c_str();

    try
    {
        std::ifstream shaderFile(gFile);
        gstrCode = std::string(
            (std::istreambuf_iterator<char>(shaderFile)),
             std::istreambuf_iterator<char>()
            );
        shaderFile.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR while opening: " << gFile << std::endl;
    }
    const char* gchCode = gstrCode.c_str();

    // compile
    unsigned int vshaderID, fshaderID, gshaderID;
    int success;
    char status[512];

    //// vertex shader
    vshaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshaderID, 1, &vchCode, NULL);
    glCompileShader(vshaderID);
    ////// check compile status
    glGetShaderiv(vshaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vshaderID, 512, NULL, status);
        std::cout << "ERROR when compiling: " << vFile << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS compilation: " << vFile << std::endl;
    }

    //// fragment shader
    fshaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshaderID, 1, &fchCode, NULL);
    glCompileShader(fshaderID);
    ////// check compile status
    glGetShaderiv(fshaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fshaderID, 512, NULL, status);
        std::cout << "ERROR when compiling: " << fFile << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS compilation: " << fFile << std::endl;
    }

    //// geometry shader
    gshaderID = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gshaderID, 1, &gchCode, NULL);
    glCompileShader(gshaderID);
    ////// check compile status
    glGetShaderiv(gshaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(gshaderID, 512, NULL, status);
        std::cout << "ERROR when compiling: " << gFile << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS compilation: " << gFile << std::endl;
    }

    // create program & link
    this->shaderID = glCreateProgram();
    auto& id = this->shaderID;
    glAttachShader(id, vshaderID);
    glAttachShader(id, fshaderID);
    glAttachShader(id, gshaderID);
    glLinkProgram(id);
    
    //// check link status
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(id, 512, NULL, status);
        std::cout << "ERROR when linking: " << std::endl;
        std::cout << status << std::endl;
    }
    else
    {
        std::cout << "SUCCESS link" << std::endl;
    }
        glDeleteShader(vshaderID);
        glDeleteShader(fshaderID);
}

// use current shader program
void Shader::use()
{
    glUseProgram(this->shaderID);
}

// set uniforms
void Shader::setUniform(const std::string &varName, const int& value) const
{
    glUseProgram(this->shaderID);
    glUniform1i(glGetUniformLocation(this->shaderID, varName.c_str()), value);
}
void Shader::setUniform(const std::string &varName, const float& value) const
{
    glUseProgram(this->shaderID);
    glUniform1f(glGetUniformLocation(this->shaderID, varName.c_str()), value);
}
void Shader::setUniform(const std::string &varName, const int& value0, const int& value1) const
{
    glUseProgram(this->shaderID);
    glUniform2i(glGetUniformLocation(this->shaderID, varName.c_str()), value0, value1);
}
void Shader::setUniform(const std::string &varName, const float& value0, const float& value1) const
{
    glUseProgram(this->shaderID);
    glUniform2f(glGetUniformLocation(this->shaderID, varName.c_str()), value0, value1);
}
void Shader::setUniform(const std::string &varName, const int& value0, const int& value1, const int& value2) const
{
    glUseProgram(this->shaderID);
    glUniform3i(glGetUniformLocation(this->shaderID, varName.c_str()), value0, value1, value2);
}
void Shader::setUniform(const std::string &varName, const float& value0, const float& value1, const float& value2) const
{
    glUseProgram(this->shaderID);
    glUniform3f(glGetUniformLocation(this->shaderID, varName.c_str()), value0, value1, value2);
}
void Shader::setUniform(const std::string &varName, const int& value0, const int& value1, const int& value2, const int& value3) const
{
    glUseProgram(this->shaderID);
    glUniform4i(glGetUniformLocation(this->shaderID, varName.c_str()), value0, value1, value2, value3);
}
void Shader::setUniform(const std::string &varName, const float& value0, const float& value1, const float& value2, const float& value3) const
{
    glUseProgram(this->shaderID);
    glUniform4f(glGetUniformLocation(this->shaderID, varName.c_str()), value0, value1, value2, value3);
}


void Shader::setUniform(const std::string &varName, const glm::vec2& vector) const
{
    glUseProgram(this->shaderID);
    glUniform2fv(glGetUniformLocation(this->shaderID, varName.c_str()), 1, &vector[0]);
}
void Shader::setUniform(const std::string &varName, const glm::vec3& vector) const
{
    glUseProgram(this->shaderID);
    glUniform3fv(glGetUniformLocation(this->shaderID, varName.c_str()), 1, &vector[0]);
}
void Shader::setUniform(const std::string &varName, const glm::vec4& vector) const
{
    glUseProgram(this->shaderID);
    glUniform4fv(glGetUniformLocation(this->shaderID, varName.c_str()), 1, &vector[0]);
}




void Shader::setUniform(const std::string &varName, const glm::mat4& mat) const
{
    glUseProgram(this->shaderID);
    glUniformMatrix4fv(glGetUniformLocation(this->shaderID, varName.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setUniform(const std::string &varName, const glm::mat3& mat) const
{
    glUseProgram(this->shaderID);
    glUniformMatrix3fv(glGetUniformLocation(this->shaderID, varName.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setUniform(const std::string &varName, const glm::mat2& mat) const
{
    glUseProgram(this->shaderID);
    glUniformMatrix2fv(glGetUniformLocation(this->shaderID, varName.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// duplicated
void Shader::setUniform(const std::string &varName, float * const& matPtr, const int& matDim) const
{
    glUseProgram(this->shaderID);
    switch (matDim)
    {
    case 3:
        glUniformMatrix3fv(glGetUniformLocation(this->shaderID, varName.c_str()),
                                1, GL_FALSE, matPtr);
        break;
    case 4:
        glUniformMatrix4fv(glGetUniformLocation(this->shaderID, varName.c_str()),
                                1, GL_FALSE, matPtr);
        break;
    default:
        break;
    }
}
