#include "objectClass.h"

GLuint LoadTextureFromFile(std::string path)
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

/**
 * implement for myMesh class
*/
myMesh::myMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indexes, const std::vector<Texture>& textures)
:   verts(vertices), idxs(indexes), texes(textures), nameInShader("object")
{
    setupMesh();
}

void myMesh::setNameInShader(const std::string& name)
{
    nameInShader = name;
}

void myMesh::setupMesh()
{
    GLuint VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), &verts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO)  ;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxs.size() * sizeof(GLuint), &idxs[0], GL_STATIC_DRAW);
    // verts.position
    glEnableVertexAttribArray(0);   
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // verts.normal
    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // verts.textureCoordinate
    glEnableVertexAttribArray(2);   
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex2DCood));

    glBindVertexArray(0);
}

/**
 * Args
 *  attrCount: the number of attributes
 *  attrIdx: the array pointer of an attributes array
 *  sizes_float: the array pointer of a number-of-float array
*/
void myMesh::appendInstanceAttrib(const GLuint& bufferObj, 
                            const GLuint& attrCount, GLuint const* const& attrIdx, 
                            GLuint const* const& sizes_float) const
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, bufferObj);
    
    GLsizei stride = 0;
    for (unsigned int i = 0; i < attrCount; ++i)
        stride += sizes_float[i];

    stride *= sizeof(float);
    GLsizei offset = 0;
    for (GLuint i = 0; i < attrCount; ++i)
    {
        glEnableVertexAttribArray(attrIdx[i]);
        glVertexAttribPointer(attrIdx[i], sizes_float[i], GL_FLOAT, GL_FALSE,
                                stride, (void*)offset);
        glVertexAttribDivisor(attrIdx[i], 1);

        offset += (sizes_float[i] * sizeof(float));
    }
    
    glBindVertexArray(VAO);
}

void myMesh::draw(Shader shader, bool isInstance, const GLsizei& instanceNum) const
{
    if(nameInShader == "")
        throw "mesh.nameInShader haven't specified!";
    // bind relevant texture UNIT
    for(int i = 0; i < texes.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        shader.setUniform(nameInShader + "." + texes[i].name, i);
        glBindTexture(GL_TEXTURE_2D, texes[i].id);
    }
    glActiveTexture(0);
    // draw
    shader.use();
    glBindVertexArray(VAO);
    if (isInstance)
        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)idxs.size(), GL_UNSIGNED_INT, 0, instanceNum);
    else
        glDrawElements(GL_TRIANGLES, (GLsizei)idxs.size(), GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}


/**
 * implement for myModel class
*/
myModel::myModel()
{

}
myModel::myModel(const std::string& path)
{
    loadModel(path);
}

void myModel::loadModel(const std::string& path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);    

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    
    processNode(scene->mRootNode, scene);
}

void myModel::processNode(aiNode *node, const aiScene * const& scene)
{
    for(GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for(GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}
myMesh myModel::processMesh(aiMesh *mesh, const aiScene * const& scene)
{
    // deal with myMesh.verts
    std::vector<Vertex> vertices;
    for(GLuint i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        aiVector3D *& temp = mesh->mTextureCoords[0];
        vertex.tex2DCood = temp ? glm::vec2(temp[i].x, temp[i].y) : glm::vec2(0.f, 0.f);
        
        vertices.push_back(vertex);
    }

    // deal with myMesh.idxs
    std::vector<GLuint> indices;
    for(GLuint i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace &face = mesh->mFaces[i];
        for(GLuint j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // deal with myMesh.material
    std::vector<Texture> textures;
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> tex = loadMaterialTextures(material, aiTextureType_DIFFUSE, "A_D");
        textures.insert(textures.end(), tex.begin(), tex.end());
        tex = loadMaterialTextures(material, aiTextureType_SPECULAR, "S");
        textures.insert(textures.end(), tex.begin(), tex.end());
    }
    return myMesh(vertices, indices, textures);
}
// TODO: make the std::vector<Texture> a & input
std::vector<Texture> myModel::loadMaterialTextures(aiMaterial *mate, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(GLuint i = 0; i < mate->GetTextureCount(type); ++i)
    {
        // get path to the specified texture
        aiString str2path;
        mate->GetTexture(type, i, &str2path);
        std::string strpath = (std::string)str2path.C_Str();
        // find if texture[i] has already loaded
        std::unordered_map<std::string, GLuint>::const_iterator it = texMap.find(strpath);
        if(it == texMap.end()) //not found, load
        {
            Texture texture;
            texture.id = LoadTextureFromFile(directory + '/' + strpath);
            texture.name = typeName;
            
            textures.push_back(texture);
            texMap.insert({strpath, texture.id});
        }
        else // found, copy value
        {
            // TODO: it seems that the texture.typeName should be different
            textures.push_back(Texture({it->second, typeName}));
        }
        
    }
    return textures;
}

// public
void myModel::appendInstanceAttrib(const GLuint& bufferObj, 
                            const GLuint& attrCount, GLuint const* const& attrIdx, 
                            GLuint const* const& sizes_float) const
{
    for (GLuint i = 0; i < meshes.size(); ++i)
    {
        meshes[i].appendInstanceAttrib(bufferObj, attrCount,attrIdx, sizes_float);
    }
    
}                            

void myModel::draw(Shader shader, bool isInstance, const GLsizei& instanceNum) const
{
    for (GLint i = 0; i < meshes.size(); ++i)
        meshes[i].draw(shader, isInstance, instanceNum);
}