#include <glad/glad.h>
#include <string>
#include <iostream>
#include <stb_image.h>

GLuint loadTexture(const std::string& path);
GLuint loadCubeMap(std::string const* const& paths);