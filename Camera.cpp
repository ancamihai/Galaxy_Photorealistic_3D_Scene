#include "Camera.hpp"

#include <iostream>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUp));
        this->cameraUpDirection = cameraUp;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
        //turn glm::mat4();
    }

    //update the camera internal parameters following a camera move event
    glm::vec3 Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO

        glm::vec3 translateLength;

        switch (direction) {
        case MOVE_FORWARD:
            translateLength = cameraFrontDirection * speed;
            cameraPosition += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            translateLength = -cameraFrontDirection * speed;
            cameraPosition -= cameraFrontDirection * speed;
            break;
        case MOVE_RIGHT:
            translateLength = cameraRightDirection * speed;
            cameraPosition += cameraRightDirection * speed;
            break;
        case MOVE_LEFT:
            translateLength = -cameraRightDirection * speed;
            cameraPosition -= cameraRightDirection * speed;
            break;
        }

        cameraPosition.y = 2.0f;

        return translateLength;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(front);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    glm::vec3 Camera::getCameraTarget()
    {
        return cameraTarget;
    }
}