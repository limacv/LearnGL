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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Shader shaderProgram;
Camera camera(glm::vec3(0, 0, 3));
GLuint VAO, texture, texture1;
const float screenWid = 800.0f, screenHei = 600.0f;
const float mouseSensitivity = 0.0005f;
const float keyMoveSensitivity = 10.f;

//callback function WHEN window size changes
void frambuffer_size_change(GLFWwindow *window, GLint img_wid, GLint img_hei)
{
    glViewport(0, 0, img_wid, img_hei);
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

const glm::vec3 translateVecs[] = {
    glm::vec3(-1.3f,  1.0f, -1.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f)
};
inline void sceneInitial()
{
    // vertices data
    // ---------------------------------
    float verts1[] = {
        // positions          // colors     // texture coords
         1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        
        // the other pair
         1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,   0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,   0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
    };


    GLuint indices[] = {
        0, 1, 2,  3,  4,  5,  6,  7,
        8, 9, 10, 11, 12, 13, 14, 15
    };
    shaderProgram = Shader("./src/LearnOpenGL/shader/basicTextureShader.vert", "./src/LearnOpenGL/shader/basicTextureShader.frag");
    // create vertex buffer object
    // -------------------------------------
    glGenVertexArrays(1, &VAO);
    GLuint VBO, VEO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VEO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts1), verts1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // unbind VAO
    glBindVertexArray(0);

    // texture
    // --------------------------
    int img_wid, img_hei, img_channel;
    //// texture1
    //// -------
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    uint8_t* image = stbi_load("./texture/container.jpg", &img_wid, &img_hei, &img_channel, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_wid, img_hei, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    //// texture2
    //// --------
    glGenTextures(1, &texture1);
    // glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    image = stbi_load("./texture/awesomeface.png", &img_wid, &img_hei, &img_channel, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_wid, img_hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    //// set sampler uniform (that is: set texture in GLSL)
    //// --------
    shaderProgram.use();
    shaderProgram.setUniform("image", 0);
    shaderProgram.setUniform("image1", 1);
}

// write code here to change RENDER SCENE
inline void renderScene()
{

    shaderProgram.use();
    // calculate time-varying variable
    float pastTime = (float)glfwGetTime();
    float c = cos(pastTime), s = sin(pastTime);
    float v0_1 = cos(pastTime) / 2.0f + 0.5f;
    shaderProgram.setUniform("a", v0_1);

    // calculate transformation matrix
    // -----------------------------------
    //// view Matrix
    glm::mat4 view = camera.GetViewMatrix();
    shaderProgram.setUniform("view", view);

    //// project Matrix
    // glm::mat4 project = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);
    glm::mat4 project = glm::perspective(camera.GetFOV(), screenWid / screenHei, 0.1f, 100.0f);
    shaderProgram.setUniform("projection", project);

    // bind buffers & draw
    glBindVertexArray(VAO);
    for(int i = 0; i < 10; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        model = glm::translate(model, translateVecs[i]);
        // model = glm::rotate(model, pastTime * (i+1) / 4.f, glm::normalize(glm::vec3(c, s, 0)));  
        shaderProgram.setUniform("model", model);

        glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_INT, (void*)(8*sizeof(GLuint)));
    }
    glBindVertexArray(0);

}

inline void mytest()
{
    // int number;
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &number);
    // std::cout << "max attribute: " << number << std::endl;
}

int main(int argc, char **argv)
{
    // shader/basic initializing & initialize a window
    // -------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow((int)screenWid, (int)screenHei, "LearnOpenGL", NULL, NULL);
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
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
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