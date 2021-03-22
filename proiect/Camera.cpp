#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        //TODO - Update the rest of camera parameters
		this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);;
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
		switch (direction) {
		case MOVE_FORWARD:
			cameraPosition += cameraFrontDirection * speed;
			break;

		case MOVE_BACKWARD:
			cameraPosition -= cameraFrontDirection * speed;
			break;

		case MOVE_RIGHT:
			cameraPosition += cameraRightDirection * speed;
			break;

		case MOVE_LEFT:
			cameraPosition -= cameraRightDirection * speed;
			break;
		}
		cameraTarget = cameraPosition + cameraFrontDirection;
    }


    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
		cameraFrontDirection = glm::vec3(
			cos(pitch) * sin(yaw),
			sin(pitch),
			cos(pitch) * cos(yaw)
		);
		cameraTarget = cameraPosition + cameraFrontDirection;
		cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
	}

	void Camera::setPosition(float x, float y, float z) {
		//TODO
		cameraPosition = glm::vec3(x, y, z);
		cameraTarget = cameraPosition + cameraFrontDirection;
	}
}