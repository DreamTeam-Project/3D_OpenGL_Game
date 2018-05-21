#include "Camera.h"

Camera::Camera( vec3 up, GLfloat yaw, GLfloat pitch) 
		: Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
	this->WorldUp = up;
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) 
		: Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM) {
	//this->Position = glm::vec3(posX, posY, posZ);
	this->WorldUp = glm::vec3(upX, upY, upZ);
	this->Yaw = yaw;
	this->Pitch = pitch;
	this->updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(
		Position->get_pos(), Position->get_pos() + Front, Up);
		//this->Position, this->Position + this->Front, this->Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
	double velocity = this->MovementSpeed * deltaTime;
	added.setX(0);
	added.setY(0);
	added.setZ(0);
	if (direction == FORWARD) {
		/*printf("forw\n");
		printf("%f %f %f\n", Front.x, Front.y, Front.z);
		printf("%f %f %f\n", Position->body->getLinearVelocity().getX(), Position->body->getLinearVelocity().getY(), Position->body->getLinearVelocity().getZ());*/
		Position->set_velosity(
			Position->body->getLinearVelocity() + btVector3(Front.x* velocity, Front.y*velocity, Front.z*velocity));
		added += btVector3(Front.x* velocity, Front.y*velocity, Front.z*velocity);
		//printf("%f %f %f\n", Position->body->getLinearVelocity().getX(), Position->body->getLinearVelocity().getY(), Position->body->getLinearVelocity().getZ());
		////this->Position += this->Front * velocity;
	}
	if (direction == BACKWARD) {
		//this->Position -= this->Front * velocity;
		/*printf("Bac\n");*/
		Position->set_velosity(
			Position->body->getLinearVelocity() - btVector3(Front.x* velocity, Front.y*velocity, Front.z*velocity));
		added -= btVector3(Front.x* velocity, Front.y*velocity, Front.z*velocity);
	}
	if (direction == LEFT) {
		/*printf("left\n");*/
		Position->set_velosity(	Position->body->getLinearVelocity() - btVector3(Right.x* velocity, Right.y*velocity, Right.z*velocity));
		//this->Position -= this->Right * velocity;
		added -= btVector3(Right.x* velocity, Right.y*velocity, Right.z*velocity);
	}
	if (direction == RIGHT) {
		/*printf("right\n");*/
		Position->set_velosity(Position->body->getLinearVelocity() + btVector3(Right.x* velocity, Right.y*velocity, Right.z*velocity));
		//this->Position += this->Right * velocity;
		added += btVector3(Right.x* velocity, Right.y*velocity, Right.z*velocity);
	}
	/*printf("in dir\n");*/
	Position->body->setActivationState(ACTIVE_TAG);
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch) {
	xoffset *= this->MouseSensitivity;
	yoffset *= this->MouseSensitivity;


	this->Yaw += xoffset;
	this->Pitch += yoffset;

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