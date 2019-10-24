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

Shader objectShader, lighterShader;
Camera camera(glm::vec3(0, 0, 0));
GLuint VAO, lightVAO, texture, texture1;
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

const glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
const glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
const glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};
inline void sceneInitial()
{
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

    objectShader = Shader("./src/LearnOpenGL/shader/lightingShader.vert", "./src/LearnOpenGL/shader/lightingShader.frag");
    lighterShader = Shader("./src/LearnOpenGL/shader/unit.vert", "./src/LearnOpenGL/shader/unit.frag");
    // create vertex array object
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
    
    // unbind VAO
    glBindVertexArray(0);

    // creat light vertex array object
    // ------------------------------
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    
    // operate VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // texture
    // --------------------------
    texture = loadTexture("./texture/container2.png");
    texture1 = loadTexture("./texture/container2_specular.png");

    // set constant UNIFORMS
    objectShader.setUniform("object.A_D", 0);
    objectShader.setUniform("object.S", 1);
    objectShader.setUniform("object.shininess", 32.f);

    lighterShader.setUniform("lighterColor", 1.f, 1.f, 1.f, 1.f);
}

// write code here to change RENDER SCENE
inline void renderScene()
{
    // calculate time-varying variable
    // float pastTime = (float)glfwGetTime();
    // float c = cos(pastTime), s = sin(pastTime);
    // float v0_1 = cos(pastTime) / 2.0f + 0.5f;
    // objectShader.setUniform("a", v0_1);

    // tranfer necessary UNIFORMS
    // ----------------------------------
    objectShader.setUniform("viewPos", camera.GetPosition());
    //// dlight
    objectShader.setUniform("dlight.direction", -0.2f, -1.0f, -0.3f);
    objectShader.setUniform("dlight.A", 0.05f, 0.05f, 0.05f);
    objectShader.setUniform("dlight.D", 0.4f, 0.4f, 0.4f);
    objectShader.setUniform("dlight.S", 0.5f, 0.5f, 0.5f);
    //// plight[]
    objectShader.setUniform("plight[0].position", pointLightPositions[0]);
    objectShader.setUniform("plight[0].A", 0.05f, 0.05f, 0.05f);
    objectShader.setUniform("plight[0].D", 0.8f, 0.8f, 0.8f);
    objectShader.setUniform("plight[0].S", 1.f, 1.f, 1.f);
    objectShader.setUniform("plight[0].Kl", 0.06f);
    objectShader.setUniform("plight[0].Kq", 0.015f);

    objectShader.setUniform("plight[1].position", pointLightPositions[1]);
    objectShader.setUniform("plight[1].A", 0.05f, 0.05f, 0.05f);
    objectShader.setUniform("plight[1].D", 0.8f, 0.8f, 0.8f);
    objectShader.setUniform("plight[1].S", 1.f, 1.f, 1.f);
    objectShader.setUniform("plight[1].Kl", 0.06f);
    objectShader.setUniform("plight[1].Kq", 0.015f);

    objectShader.setUniform("plight[2].position", pointLightPositions[2]);
    objectShader.setUniform("plight[2].A", 0.05f, 0.05f, 0.05f);
    objectShader.setUniform("plight[2].D", 0.8f, 0.8f, 0.8f);
    objectShader.setUniform("plight[2].S", 1.f, 1.f, 1.f);
    objectShader.setUniform("plight[2].Kl", 0.06f);
    objectShader.setUniform("plight[2].Kq", 0.015f);

    objectShader.setUniform("plight[3].position", pointLightPositions[3]);
    objectShader.setUniform("plight[3].A", 0.05f, 0.05f, 0.05f);
    objectShader.setUniform("plight[3].D", 0.8f, 0.8f, 0.8f);
    objectShader.setUniform("plight[3].S", 1.f, 1.f, 1.f);
    objectShader.setUniform("plight[3].Kl", 0.06f);
    objectShader.setUniform("plight[3].Kq", 0.015f);
    //// slight
    objectShader.setUniform("slight.position", camera.GetPosition());
    objectShader.setUniform("slight.direction", camera.GetFront());
    objectShader.setUniform("slight.innerCosPhi", glm::cos(glm::radians(12.5f)));
    objectShader.setUniform("slight.outerCosPhi", glm::cos(glm::radians(15.f)));
    objectShader.setUniform("slight.A", 0.f, 0.f, 0.f);
    objectShader.setUniform("slight.D", 1.0f, 1.0f, 1.0f);
    objectShader.setUniform("slight.S", 1.f, 1.f, 1.f);
    objectShader.setUniform("slight.Kl", 0.06f);
    objectShader.setUniform("slight.Kq", 0.015f);
    // calculate transformation matrix
    // -----------------------------------
    //// view Matrix
    glm::mat4 view = camera.GetViewMatrix();
    objectShader.setUniform("view", view);
    lighterShader.setUniform("view", view);
    //// project Matrix
    // glm::mat4 project = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f);
    glm::mat4 project = glm::perspective(camera.GetFOV(), screenWid / screenHei, 0.1f, 100.0f);
    objectShader.setUniform("projection", project);
    lighterShader.setUniform("projection", project);

    //// bind Texture Map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
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

    // draw light
    lighterShader.use();
    glBindVertexArray(lightVAO);
    for(int i = 0; i < sizeof(pointLightPositions)/sizeof(pointLightPositions[0]); ++i)
    {
        glm::mat4 lighterModel = glm::mat4(1.f);
        lighterModel = glm::translate(lighterModel, pointLightPositions[i]);
        lighterModel = glm::scale(lighterModel, glm::vec3(0.1f, 0.1f, 0.1f));
        lighterShader.setUniform("model", lighterModel);

        glDrawArrays(GL_TRIANGLES, 0, 36);
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