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
#include "textureLoad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


Shader objectShader;
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

myModel planet, rock;
const GLuint instanceNum = 10000;
glm::mat4 *modelMat = new glm::mat4[instanceNum];
inline void sceneInitial()
{
    objectShader = Shader("./src/LearnOpenGL/shader/instance.vert", "./src/LearnOpenGL/shader/instance.frag");
    
    // import model
    planet = myModel("./texture/planet/planet.obj");
    rock = myModel("./texture/rock/rock.obj");

    // model mat for each instance
    srand((unsigned int)glfwGetTime());
    float radius = 20.f;
    float offset = 4.f;
    for(unsigned int i = 0; i < instanceNum; i++)
    {
        glm::mat4 model = glm::mat4(1.f);
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)instanceNum * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 5) / 100.0f + 0.01f;
        model = glm::scale(model, glm::vec3(scale));

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotAngle = (float)(rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        modelMat[i] = model;
    }  

    // send modelMat into the array buffer
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, instanceNum * sizeof(glm::mat4), &modelMat[0], GL_STATIC_DRAW);

    const GLuint attrNum[4] = {3, 4, 5, 6};
    const GLuint attrSizes[4] = {4, 4, 4, 4};
    rock.appendInstanceAttrib(VBO, 4, attrNum, attrSizes);
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
    model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));	
    objectShader.setUniform("modelPlanet", model);
    glEnable(GL_CULL_FACE);

    planet.draw(objectShader);

    // model Matrixes & draw cubes
    rock.draw(objectShader, true, instanceNum);
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
    glEnable(GL_DEPTH_TEST);
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