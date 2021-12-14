#include "Camera.h"
#include <math.h>
#include <iostream>
#include <glut.h>

//Constructor and sets up initial values
Camera::Camera(Input* input)
{
	in = input;

	up.set(0, 1, 0);
	forward.set(0, 0, 0);
	position.set(0, 0, 0);

	pitch = 0;
	yaw = 0;
	roll = 0;

	xCentre = 0;
	yCentre = 0;

	glutSetCursor(GLUT_CURSOR_NONE);
}

//Creates an additional camera with input and a initial position of the XYZ values
void Camera::CreateCamera(Input* input, float xPos, float yPos, float zPos)
{
	this->in = input;
	position.set(xPos, yPos, zPos);
}

int Camera::GetMouseX()
{
	//Grabs the input of mouse X subtracting the distrance from the x centre
	return in->getMouseX() - xCentre;
}

int Camera::GetMouseY()
{
	//Grabs the input of mouse Y subtracting the distrance from the y centre
	return in->getMouseY() - yCentre;
}

void Camera::Update()
{
	float cosR, cosP, cosY;
	float sinR, sinP, sinY;

	cosY = cosf(yaw * 3.1415 / 180);
	cosP = cosf(pitch * 3.1415 / 180);
	cosR = cosf(roll * 3.1415 / 180);

	sinY = sinf(yaw * 3.1415 / 180);
	sinP = sinf(pitch * 3.1415 / 180);
	sinR = sinf(roll * 3.1415 / 180);

	forward.x = sinY * cosP; 
	forward.y = sinP;
	forward.z = cosP * -cosY;

	lookAt = forward + position;

	up.x = -cosY * sinR - sinY * sinP * cosR;
	up.y = cosP * cosR;
	up.z = -sinY * sinR - sinP * cosR * -cosY;

	std::cout << up.x << " " << up.y << " " << up.z << "\n";
	std::cout << forward.x << " " << forward.y << " " << forward.z << "\n";
	std::cout << position.x << " " << position.y << " " << position.z << "\n";

	right = forward.cross(up);

	glutWarpPointer(xCentre, yCentre);
}


void Camera::HandleInput(float dt)
{
	//If controls are not locked, allow WASD movement
	if (!lockedControls)
	{
		//Up movement
		if (in->isKeyDown('w'))
			MoveForward(dt);

		//Down movement
		if (in->isKeyDown('s'))
			MoveBackward(dt);

		//Left movement
		if (in->isKeyDown('a'))
			MoveLeft(dt);

		//Right movement
		if (in->isKeyDown('d'))
			MoveRight(dt);
	}

	//Mouse should not be locked

	//Left - Right Mouse movement
	if (GetMouseX() != 0)
	{
		yaw += (GetMouseX() * 2) / mouseSens;
		Update();
	}

	//Up - Down Mouse movement
	if (GetMouseY() != 0)
	{
		if (pitch > 89)
			pitch = 89;
		else if (pitch < -89)
			pitch = -89;

		pitch -= (GetMouseY() * 2) / mouseSens;
		Update();
	}
}

//Moves the camera forward
void Camera::MoveForward(float dt)
{
	position.add(forward, speed * dt);
	Update();
}

//Moves the camera backwards
void Camera::MoveBackward(float dt)
{
	position.subtract(forward, speed * dt);
	Update();
}

//Moves the camera to the right
void Camera::MoveRight(float dt)
{
	position.add(right, speed * dt);
	Update();
}

//Moves the camera to the left
void Camera::MoveLeft(float dt)
{
	position.subtract(right, speed * dt);
	Update();
}

//Getters
Vector3 Camera::GetForwardView()
{
	return forward;
}

Vector3 Camera::GetUpView()
{
	return up;
}

Vector3 Camera::GetPositionView()
{
	return position;
}

