#pragma once
#include "Vector3.h";
#include "Input.h";

class Camera
{
public:
	Camera(Input* userInput);

	void Update();

	Vector3 GetUpView();
	Vector3 GetForwardView(); 
	Vector3 GetPositionView();

	void MoveForward(float dt);
	void MoveBackward(float dt);

	void MoveLeft(float dt);
	void MoveRight(float dt);

	void HandleInput(float time);

	int GetMouseX();
	int GetMouseY();

	float yaw;
	float pitch;
	float roll;

	float xCentre;
	float yCentre;

	Vector3 position;
	Vector3 forward;
	Vector3 up;
	Vector3 lookAt;
	Vector3 right;

	Input* in;

	float speed = 5.0f;
	float mouseSens = 25.0f;
};

