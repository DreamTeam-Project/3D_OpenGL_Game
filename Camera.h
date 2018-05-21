#ifndef CAMERA_H
#define CAMERA_H

#define GLEW_STATIC

#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Physics.h"


enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 10.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

using glm::vec3;
class Character;

class Camera {
public:
	//vec3 Position;
	Character* Position;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp;


	btVector3 added;

	GLfloat Yaw;
	GLfloat Pitch;

	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	Camera(vec3 up = vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH);
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch);
	glm::mat4 GetViewMatrix() const;
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(GLfloat yoffset);

private:
	void updateCameraVectors();
};

#endif