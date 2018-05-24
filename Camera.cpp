#include "Camera.h"

Camera::Camera(vec3 position, vec3 up, GLfloat yaw, GLfloat pitch)
	: Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
	this->Position = position;
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
	: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
	this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(
		position->get_pos(), position->get_pos() + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
	
	float velosity =  this->MovementSpeed * deltaTime;
	btScalar len = position->body->getLinearVelocity().length();
	if (len < 10) {
		if (direction == FORWARD) {
			position->set_velosity(
				position->body->getLinearVelocity() +
				btVector3(Front.x* velosity, Front.y*velosity,
					Front.z*velosity));
		}
		if (direction == BACKWARD) {
			position->set_velosity(
				position->body->getLinearVelocity() -
				btVector3(Front.x* velosity, Front.y*velosity,
					Front.z*velosity));
		}
		if (direction == LEFT) {
			position->set_velosity(
				position->body->getLinearVelocity()
				- btVector3(Right.x* velosity, Right.y*velosity,
					Right.z*velosity));
		}
		if (direction == RIGHT) {
			position->set_velosity(
				position->body->getLinearVelocity()
				+ btVector3(Right.x* velosity, Right.y*velosity,
					Right.z*velosity));
		}
	}
	
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch) {
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;

	this->Yaw += xoffset;
	this->Pitch += yoffset;
	/*position->body->setAngularVelocity(btVector3(0, 0, (double)xoffset));
	this->Yaw = ()*/
	if (constrainPitch) {
		if (this->Pitch > 89.0f)
			this->Pitch = 89.0f;
		if (this->Pitch < -89.0f)
			this->Pitch = -89.0f;
	}

	this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(GLfloat yoffset) {
	if (this->Zoom >= 1.0f && this->Zoom <= 45.0f) {
		this->Zoom -= yoffset;
	}
	if (this->Zoom <= 1.0f) {
		this->Zoom = 1.0f;
	}
	if (this->Zoom >= 45.0f) {
		this->Zoom = 45.0f;
	}
}

void Camera::updateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	front.y = sin(glm::radians(this->Pitch));
	front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
	this->Front = glm::normalize(front);
	this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
	this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}