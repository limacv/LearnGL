#include "textureLoad.h"

GLuint loadTexture(const std::string& path)
{
    GLuint texture;
    glGenTextures(1, &texture);

    int wid, hei, chan;
    uint8_t* image = stbi_load(path.c_str(), &wid, &hei, &chan, 0);
    if(image)
    {
        GLenum format;
        switch (chan)
        {
        case 1:
            format = GL_RED; break;
        case 3:
            format = GL_RGB; break;
        case 4:
            format = GL_RGBA; break;
        default:
            std::cout << "Image Format not Recognized: " << std::endl;
            std::cout << path << std::endl;
            format = GL_RGB;
            texture = NULL;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wid, hei, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        std::cout << "Failed to Load Image File:" << std::endl;
        std::cout << path << std::endl;
        texture = NULL;
    }
    stbi_image_free(image);
    return texture;
}

GLuint loadCubeMap(std::string const* const& paths)
{
    GLuint cubeMap;
    glGenTextures(1, &cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

    int wid, hei, chan;
    for(int i = 0; i < 6; ++i)
    {
        uint8_t* img = stbi_load(paths[i].c_str(), &wid, &hei, &chan, 0);
        if(img)
        {
            GLenum format;
            switch (chan)
            {
            case 1:
                format = GL_RED; break;
            case 3:
                format = GL_RGB; break;
            case 4:
                format = GL_RGBA; break;
            default:
                std::cout << "Image Format not Recognized: " << std::endl;
                std::cout << paths[i] << std::endl;
                format = GL_RGB;
                cubeMap = NULL;
            }
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                        0, GL_RGB, wid, hei, 0, format, GL_UNSIGNED_BYTE, img);
            stbi_image_free(img);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
        else
        {
            std::cout << "Failed to Load Image File:" << std::endl;
            std::cout << paths[i] << std::endl;
            cubeMap = NULL;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return cubeMap;
}
