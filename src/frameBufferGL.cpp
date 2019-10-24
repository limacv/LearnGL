#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderClass.h"
#include "camera3D.h"
#include "objectClass.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


Shader objectShader, unitShader;
Camera camera(glm::vec3(0, 0, 0));
const float screenWid = 1280.0f, screenHei = 960.0f, screenRatio = screenWid/screenHei;
const float mouseSensitivity = 0.0005f;
const float keyMoveSensitivity = 10.f;

//callback function WHEN window size changes
void frambuffer_size_change(GLFWwindow *window, GLint img_wid, GLint img_hei)
{
    if (float(img_wid)/float(img_hei) > screenRatio)
        glViewport(0, 0, int(screenRatio * img_hei), img_hei);
    else
        glViewport(0, 0, img_wid, int(img_wid/screenRatio));
}

//callback function WHEN mouse moves
void mouse_move(GLFWwindow *window, double xpos, double ypos)
{
    static float xlastPos = (float)xpos, ylastPos = (float)ypos;

    float xoffset = ((float)xpos - xlastPos)*mouseSensitivity;
    float yoffset = (ylastPos - (float)ypos)*mouseSensitivity;
    xlastPos = (float)xpos;
    ylastPos = (float)ypos;

    camera.mouseMovement(xoffset, yoffset);
}

void scroll_change(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.changeVisionField((float)yoffset);
}
//function process WHEN keys are pressed
void process_key(GLFWwindow *window)
{
    // get delta Time
    static float lastFrameTime;
    float curTime = (float)glfwGetTime();
    float deltaTime = curTime - lastFrameTime;
    lastFrameTime = curTime;

    // calculate camera move distance
    float distanceperFrame = keyMoveSensitivity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // move camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.MoveInWorldCood(CAM_FORWARD, distanceperFrame);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.MoveInWorldCood(CAM_BACKWARD, distanceperFrame);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.MoveInWorldCood(CAM_LEFT, distanceperFrame);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.MoveInWorldCood(CAM_RIGHT, distanceperFrame);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.MoveInEyeCood(CAM_FORWARD, distanceperFrame);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.MoveInEyeCood(CAM_BACKWARD, distanceperFrame);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.MoveInEyeCood(CAM_LEFT, distanceperFrame);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.MoveInEyeCood(CAM_RIGHT, distanceperFrame);
    
}

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

// const glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
// const glm::vec3 pointLightPositions[] = {
//     glm::vec3( 0.7f,  0.2f,  2.0f),
//     glm::vec3( 2.3f, -3.3f, -4.0f),
//     glm::vec3(-4.0f,  2.0f, -12.0f),
//     glm::vec3( 0.0f,  0.0f, -3.0f)
// };
myModel object;
GLuint FBO, VAO, texAttach;
inline void sceneInitial()
{
    /*******************************************************************************
     * *****************************************************************************
     * see below
     * ******************************************************************************
     * ******************************************************************************
    */
    // Frame Buffer*******************
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    // color texture attachment
    glGenTextures(1, &texAttach);
    glBindTexture(GL_TEXTURE_2D, texAttach);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)screenWid*2, (GLsizei)screenHei, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texAttach, 0);
    

    // depth/stencil renderbuffer attachment
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (GLsizei)screenWid, (GLsizei)screenHei);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw "ERROR::FRAMEBUFFER:: FrameBuffer is not complete!";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    objectShader = Shader("./src/LearnOpenGL/shader/modelShader.vert", "./src/LearnOpenGL/shader/modelShader.frag");
    unitShader = Shader("./src/LearnOpenGL/shader/unit.vert", "./src/LearnOpenGL/shader/unit.frag");
    // import model
    object = myModel("./texture/nanosuit/nanosuit.obj");

    // a simple rectangular
    float vertices[] = {
        //pos               texcood
        -1.f, -1.f, -5.f,   0.f, 0.f,
        1.f,  -1.f, -5.f,   1.f, 0.f,
        -1.f, 1.f,  -5.f,   0.f, 1.f,
        1.f,  1.f,  -5.f,   1.f, 1.f
    };
    GLuint VBO;
    
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    
    unitShader.setUniform("tex", 0);
    unitShader.setUniform("color", 0.5f, 0.21f, 0.1f, 1.f);
}

// write code here to change RENDER SCENE
inline void renderScene()
{
    // -----------------------------------
    //// view Matrix
    glm::mat4 view = camera.GetViewMatrix();
    objectShader.setUniform("view", view);
    //// project Matrix
    // glm::mat4 project = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);
    glm::mat4 project = glm::perspective(camera.GetFOV(), screenWid / screenHei, 0.1f, 100.0f);
    objectShader.setUniform("projection", project);

    // model Matrixes & draw cubes
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); 
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
    objectShader.setUniform("model", model);

    // draw scene in a costum frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    object.draw(objectShader);

    // draw the frame buffer in the rectangler
    //// view Matrix
    view = camera.GetViewMatrix();
    unitShader.setUniform("view", view);
    //// project Matrix
    project = glm::perspective(camera.GetFOV(), screenWid / screenHei, 0.1f, 100.0f);
    unitShader.setUniform("projection", project);

    // model Matrixes & draw cubes
    model = glm::mat4(1.f);
    model = glm::translate(model, glm::vec3(0.0f, 0.f, 0.0f)); 
    unitShader.setUniform("model", model);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    unitShader.use();
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texAttach);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
/*******************************************************************************
     * *****************************************************************************
     * see above
     * ******************************************************************************
     * ******************************************************************************
    */

inline void mytest()
{
    // int number;
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &number);
    // std::cout << "max attribute: " << number << std::endl;
}

int main(int argc, char **argv)
{
    // basic initializing & initialize a window
    // -------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow((int)screenWid, (int)screenHei, "basic Lighting", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Other Initializing
    // ------------------------------------------------
    glViewport(0, 0, (int)screenWid, (int)screenHei);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // register window size change callback func
    glfwSetWindowSizeCallback(window, frambuffer_size_change);
    // hide cursor and capture it
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // register mouse move callback func
    glfwSetCursorPosCallback(window, mouse_move);
    glfwSetScrollCallback(window, scroll_change);
    sceneInitial();

    // Render Loop
    // ------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        //clear screen
        // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderScene();
        //process keyboard pressed
        mytest();
        process_key(window);
        //check triggle event, renew window status
        glfwPollEvents();
 
        
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}