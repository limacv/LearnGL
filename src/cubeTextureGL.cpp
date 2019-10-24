#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "textureLoad.h"
#include "shaderClass.h"
#include "objectClass.h"
#include "camera3D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader objectShader, skyboxShader;
Camera camera(glm::vec3(0, 0, 0));
GLuint VAO, skyVAO, texture, texture1, texSky;
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

const glm::vec3 cubePositions[] = {
    glm::vec3( 10.0f,  10.0f,  5.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
};
const glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
};
myModel model;
inline void sceneInitial()
{
    model = myModel("./texture/nanosuit/nanosuit.obj");
    // vertices data
    // ---------------------------------
    float verts1[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    // create vertex array object
    // -------------------------------------
    glGenVertexArrays(1, &VAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // unbind VAO
    glBindVertexArray(0);

    // VAO for skybox
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(skyVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind VAO
    glBindVertexArray(0);

    // texture
    // --------------------------
    std::string texSkyPaths[] = 
    {
        "./texture/skybox/right.jpg",
        "./texture/skybox/left.jpg",
        "./texture/skybox/top.jpg",
        "./texture/skybox/bottom.jpg",
        "./texture/skybox/front.jpg",
        "./texture/skybox/back.jpg"
    };
    // std::string texSkyPaths[] = 
    // {
    //     "./texture/skybox1/front.tga",
    //     "./texture/skybox1/back.tga",
    //     "./texture/skybox1/up.tga",
    //     "./texture/skybox1/down.tga",
    //     "./texture/skybox1/right.tga",
    //     "./texture/skybox1/left.tga",
    // };
    texture = loadTexture("./texture/container2.png");
    texture1 = loadTexture("./texture/container2_specular.png");
    texSky = loadCubeMap(texSkyPaths);

    // load shader
    objectShader = Shader("./src/LearnOpenGL/shader/reflect_refract.vert", "./src/LearnOpenGL/shader/reflect_refract.frag");
    skyboxShader = Shader("./src/LearnOpenGL/shader/skyboxShader.vert", "./src/LearnOpenGL/shader/skyboxShader.frag");
    // set constant UNIFORMS
    objectShader.setUniform("object.A_D", 0);
    objectShader.setUniform("object.S", 1);
    objectShader.setUniform("sky", 2);
    skyboxShader.setUniform("skyBox", 0);
    glEnable(GL_DEPTH_TEST);
}

// write code here to change RENDER SCENE
inline void renderScene()
{
    // glm::mat4 project = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);
    glm::mat4 project = glm::perspective(camera.GetFOV(), screenWid / screenHei, 0.1f, 100.0f);

    // render other things
    // ----------------------------------
    objectShader.setUniform("camPos", camera.GetPosition());
    // transformation matrix
    objectShader.setUniform("view", camera.GetViewMatrix());
    objectShader.setUniform("projection", project);

    //// bind Texture Map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texSky);
    // model Matrixes & draw cubes
    objectShader.use();
    glBindVertexArray(VAO);
    for(int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); ++i)
    {
        //// model Matrix for object
        glm::mat4 objectModel = glm::mat4(1.0f);
        objectModel = glm::translate(objectModel, cubePositions[i]);
        objectModel = glm::rotate(objectModel, glm::radians(20.0f * float(i)), glm::vec3(1.0f, 0.3f, 0.5f));
        objectModel = glm::scale(objectModel, glm::vec3(1.f, 1.f, 1.f));
        objectShader.setUniform("model", objectModel);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    glm::mat4 objectModel = glm::mat4(1.0f);
    objectModel = glm::translate(objectModel, glm::vec3(0.f, 0.f, -5.f));
    objectShader.setUniform("model", objectModel);
    model.draw(objectShader);

    // render skybox
    // ---------------------------------------------------
    // set uniforms
    skyboxShader.setUniform("view", camera.GetViewMatrix());
    skyboxShader.setUniform("projection", project);

    // render
    glDepthFunc(GL_LEQUAL); // it's necessary because the z-buffer is 1-filled, and skybox is also 1-filled
    // glDepthMask(GL_FALSE);
    skyboxShader.use();
    glBindVertexArray(skyVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texSky);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    // glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

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
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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