#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	void mouseControlAerea();
	void mouseControlQuiosco();
	void keyControlAerea(bool* keys, GLfloat deltaTime);
	void keyControlAvatar(bool* keys, GLfloat deltaTime);
	void keyControlQuiosco(bool* keys, GLfloat deltaTime);
	glm::vec3 getCameraPosition();
	void setCameraPosition(glm::vec3 pos) { position = pos; };
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();
	

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	GLfloat max_x, min_x, max_y, min_y, max_z, min_z;
	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

