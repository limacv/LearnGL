#include "camera3D.h"

    // Constructor with vectors
    Camera::Camera(const glm::vec3& position, const glm::vec3& worldUp) 
    :   Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        Zoom_deg(45.f),
        Position(position),
        WorldUp(worldUp),
        Yaw(-PI/2.f),
        Pitch(0.f)
    {
        synWithEuler();
    }
    // Constructor with scalar values
    Camera::Camera(const float& posX, const float& posY, const float& posZ, const float& upX, 
                    const float& upY, const float& upZ) 
    :   Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
        Zoom_deg(PI/4.f),
        Position(glm::vec3(posX, posY, posZ)),
        WorldUp(glm::vec3(upX, upY, upZ)),
        Yaw(-PI/2.f),
        Pitch(0.f)
    {
        synWithEuler();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }
    // Returns the Background View Matrix where position will never change
    glm::mat4 Camera::GetBackgroundViewMatrix() const
    {
        return glm::lookAt(glm::vec3(0.f, 0.f, 0.f), this->Front, this->Up);
    }

    float Camera::GetFOV() const
    {
        return glm::radians(this->Zoom_deg);
    }

    glm::vec3 Camera::GetPosition() const
    {
        return this->Position;
    }
    glm::vec3 Camera::GetFront() const
    {
        return this->Front;
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void Camera::MoveInEyeCood(const Camera_Movement& direction, const float& distance)
    {
        switch (direction)
        {
        case CAM_FORWARD:
            this->Position += Front * distance;
            break;
        case CAM_BACKWARD:
            this->Position -= Front * distance;
            break;
        case CAM_LEFT:
            this->Position -= Right * distance;
            break;
        case CAM_RIGHT:
            this->Position += Right * distance;
            break;  
        default:
            break;
        }
    }
    void Camera::MoveInWorldCood(const Camera_Movement& direction, const float& distance)
    {
        switch (direction)
        {
        case CAM_FORWARD:
            this->Position += glm::cross(WorldUp, Right) * distance;
            break;
        case CAM_BACKWARD:
            this->Position -= glm::cross(WorldUp, Right) * distance;
            break;
        case CAM_LEFT:
            this->Position -= Right * distance;
            break;
        case CAM_RIGHT:
            this->Position += Right * distance;
            break;  
        default:
            break;
        }
    }

    
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void Camera::mouseMovement(const float& xoffset, const float& yoffset, GLboolean constrainPitch)
    {
        this->Yaw   += xoffset;
        this->Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 3.14f/2.f)
                Pitch = 3.14f/2.f;
            if (Pitch < -3.14f/2.f)
                Pitch = -3.14f/2.f;
        }
        synWithEuler();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void Camera::changeVisionField(const float& yoffset)
    {
        if (Zoom_deg >= 1.0f && Zoom_deg <= 89.0f)
            Zoom_deg -= yoffset;

        if (Zoom_deg <= 1.0f)
            Zoom_deg = 1.0f;
        else if (Zoom_deg >= 89.0f)
            Zoom_deg = 89.0f;
    }

    //private
    //----------------------------------
    void Camera::synWithEuler()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(Yaw) * cos(Pitch);
        front.y = sin(Pitch);
        front.z = sin(Yaw) * cos(Pitch);
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look worldUp or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }

    void Camera::synWithCart()
    {
        this->Pitch = asin(this->Front.y);
        this->Yaw = acos(Front.x / cos(this->Pitch));
        if (Front.z < 0)
            this->Yaw = 2 * PI - this->Yaw;
    }