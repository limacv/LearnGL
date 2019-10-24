# pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <glad/glad.h>

#include "stb_image.h"
// assimp loader
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "glm/glm.hpp"
// class of my
#include "shaderClass.h"

struct Vertex
{
    // TODO: change glm::vec3 to aivector3D to reduce copy?
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex2DCood;
};
struct Texture
{
    GLuint id;
    std::string name; // name in shader
};

GLuint GetTextureFromFile(std::string path);

class myMesh
{
private:
    GLuint VAO;
    std::string nameInShader;
    std::vector<Vertex> verts;
    std::vector<GLuint> idxs;
    std::vector<Texture> texes;

    void setupMesh();
public:
    myMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indexes, const std::vector<Texture>& textures);
    /**
     * Args
     *  attrCount: the number of attributes
     *  attrIdx: the array pointer of an attributes array
     *  sizes_float: the array pointer of a number-of-float array
    */
    void appendInstanceAttrib(const GLuint& bufferObj, 
                    const GLuint& attrCount, GLuint const* const& attrIdx, 
                    GLuint const* const& sizes_float) const;
    void setNameInShader(const std::string& name);
    void draw(Shader shader, bool isInstance=false, const GLsizei& instanceNum=0) const;
};

class myModel
{
private:
    std::vector<myMesh> meshes;
    std::string directory;
    std::unordered_map<std::string, GLuint> texMap;
    
    void loadModel(const std::string& path);
    void processNode(aiNode *node, const aiScene * const& scene);
    myMesh processMesh(aiMesh *mesh, const aiScene * const& scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mate, aiTextureType type, 
                                             std::string typeName);

public:
    myModel();
    myModel(const std::string& path);
    
    /**
     * Args
     *  attrCount: the number of attributes
     *  attrIdx: the array pointer of an attributes array
     *  sizes_float: the array pointer of a number-of-float array
    */
    void appendInstanceAttrib(const GLuint& bufferObj, 
                    const GLuint& attrCount, GLuint const* const& attrIdx, 
                    GLuint const* const& sizes_float) const;
    void myModel::draw(Shader shader, bool isInstance=false, const GLsizei& instanceNum=0) const;
};