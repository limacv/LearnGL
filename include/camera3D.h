#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum Camera_Movement {
    CAM_FORWARD,
    CAM_BACKWARD,
    CAM_LEFT,
    CAM_RIGHT
};

const float PI = glm::pi<float>();
// Default camera values
// const float YAW         = -90.0f;
// const float PITCH       =  0.0f;
// const float SPEED       =  2.5f;
// const float SENSITIVITY =  0.1f;
// const float ZOOM        =  45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Constructor with vectors
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    // Constructor with scalar values
    Camera(const float& posX, const float& posY, const float& posZ, 
            const float& upX, const float& upY, const float& upZ);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetBackgroundViewMatrix() const;
    float GetFOV() const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetFront() const;

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void MoveInEyeCood(const Camera_Movement& direction, const float& distance);
    void MoveInWorldCood(const Camera_Movement& direction, const float& distance);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void mouseMovement(const float& xoffset, const float& yoffset, GLboolean constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void changeVisionField(const float& yoffset);

private:
    // Camera Attributes
    glm::vec3 WorldUp;
    
    glm::vec3 Position;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float Zoom_deg;

    // update Front, Up & Right with Yaw & Pitch
    void synWithEuler();
    // update Yaw & Pitch with Front, Up & Right
    void synWithCart();
};
