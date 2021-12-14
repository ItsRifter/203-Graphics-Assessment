#include "Scene.h"
#include "Camera.h"
// Scene constructor, initilises OpenGL
// You should add further variables to need initilised.

//This is outside the methods so this can be edited in update
GLfloat Light_Diffuse2[] = { 0.7f, 0.0f, 0.0f, 1.0f };

Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;
	initialiseOpenGL();

	// Other OpenGL / render setting should be applied here.

	// Initialise scene variables

	//Light 0 - Red Point
	GLfloat light_Position[] = { 0.0f, 2.0f, 0.0f, 1.0f };
	GLfloat Light_Diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_Position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Light_Diffuse);
	glEnable(GL_LIGHT0);

	//Light 1 - Spot
	GLfloat light_Position2[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	GLfloat spot_direction2[] = { 0.0, -1.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_Position2);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Diffuse2);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 90.0f);
	glEnable(GL_LIGHT1);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	//Create a new main camera
	mainCam = new Camera(input);
	freeRoamCam.CreateCamera(input, 0, 0.5, 0);
	camSpot1.CreateCamera(input, 10, 4, -21);
	camSpot2.CreateCamera(input, 20, 0, 8);

	mainCam = &freeRoamCam;

	//Load each model with textures
	if (!baseModel.load("models/station_base.obj", "gfx/metal2.png"))
		printf("Model 'Base' failed to load");

	if (!skyboxModel.load("models/station_skybox.obj", "gfx/space.png"))
		printf("Model 'Skybox' failed to load");

	if (!pipesModel.load("models/station_pipes.obj", "gfx/pipe.png"))
		printf("Model 'Pipes' failed to load");

	if (doorsModel.load("models/station_doors.obj", "gfx/door.png"))
		printf("Model 'Doors' failed to load");

	if (toolModel.load("models/station_toolbox.obj", "gfx/metal1.png"))
		printf("Model 'Doors' failed to load");

	if (lightsModel.load("models/station_lights.obj", "gfx/lights.png"))
		printf("Model 'Lights' failed to load");

	if (stepLadderModel.load("models/station_stepladder.obj", "gfx/stepladder.png"))
		printf("Model 'Stepladder' failed to load");

	if (chairModel.load("models/station_chair.obj", "gfx/wood.png"))
		printf("Model 'Chair' failed to load");

	//Unbinds the texture once finished
	glBindTexture(GL_TEXTURE_2D, NULL);
}

void Scene::handleInput(float dt)
{
	// Handle user input
	mainCam->HandleInput(dt);

	//When the Q key is pressed, enable or disable wireframe mode
	if (input->isKeyDown(113))
	{
		isWireframeOn = !isWireframeOn;
		input->setKeyUp(113);
	}

	//When the F key is pressed, enable or disable fullbright mode
	if (input->isKeyDown(102))
	{
		isFullbrightOn = !isFullbrightOn;
		input->setKeyUp(102);
	}

	//After the F key is pressed, should lighting be enabled or disabled?
	if (isFullbrightOn)
		glDisable(GL_LIGHTING);
	else
		glEnable(GL_LIGHTING);

	
	//Gets input if we should change camera;
	ChangeCamera(dt);

}

void Scene::ChangeCamera(float dt)
{
	//Check if the camera is not a null pointer
	if (mainCam != nullptr)
	{
		//If the Number 1 is pressed, check if already set to, if not set, switch camera to freeroam
		if (input->isKeyDown(49) && mainCam != &freeRoamCam)
		{
			mainCam = &freeRoamCam;
			input->setKeyUp(49);
			mainCam->lockedControls = false;
		}
		//Same above but for Number 2
		else if (input->isKeyDown(50) && mainCam != &camSpot1)
		{
			mainCam = &camSpot1;
			input->setKeyUp(50);
			mainCam->lockedControls = true;
		}
		//Same above but for Number 3
		else if (input->isKeyDown(51) && mainCam != &camSpot2)
		{
			mainCam = &camSpot2;
			input->setKeyUp(50);
			mainCam->lockedControls = true;
		}

		//Set the X centre of the camera by the width divided by 2 and same process but for height
		mainCam->xCentre = width / 2;
		mainCam->yCentre = height / 2;

		//Handle the input
		mainCam->HandleInput(dt);
	}
}

void Scene::update(float dt)
{
	// update scene related variables.

	//If the first lights diffuse is less than revert the fade effect, else if more than invert the fading
	if (Light_Diffuse2[0] < 0.05f)
		invertLightFade = false;
	else if (Light_Diffuse2[0] > 0.75f)
		invertLightFade = true;

	if (invertLightFade == true)
	{
		//Decreases the light multiplied by the fade speed
		Light_Diffuse2[0] -= dt * light1FadeSpeed;
		//Light_Diffuse2[1] -= dt * light1FadeSpeed;
		//Light_Diffuse2[2] -= dt * light1FadeSpeed;
	}
	else {
		//Increases the light multiplied by the fade speed
		Light_Diffuse2[0] += dt * light1FadeSpeed;
		//Light_Diffuse2[1] += dt * light1FadeSpeed;
		//Light_Diffuse2[2] += dt * light1FadeSpeed;
	}

	//Update lighting
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Light_Diffuse2);

	// Calculate FPS for output
	calculateFPS();
}

void Scene::render() {

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(mainCam->GetPositionView().x, mainCam->GetPositionView().y, mainCam->GetPositionView().z, mainCam->lookAt.x, mainCam->lookAt.y, mainCam->lookAt.z, mainCam->GetUpView().x, mainCam->GetUpView().y, mainCam->GetUpView().z);
	
	// Render geometry/scene here -------------------------------------	

	//Renders models either with or without wireframe on
	baseModel.render(isWireframeOn);
	skyboxModel.render(isWireframeOn);
	pipesModel.render(isWireframeOn);
	doorsModel.render(isWireframeOn);
	toolModel.render(isWireframeOn);
	lightsModel.render(isWireframeOn);
	chairModel.render(isWireframeOn);
	stepLadderModel.render(isWireframeOn);

	// End render geometry --------------------------------------

	// Render text, should be last object rendered.
	renderTextOutput();
	
	// Swap buffers, after all objects are rendered.
	glutSwapBuffers();
}

void Scene::initialiseOpenGL()
{
	//OpenGL settings
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);			// Cornflour Blue Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// Clear stencil buffer
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// Blending function
}

// Handles the resize of the window. If the window changes size the perspective matrix requires re-calculation to match new window size.
void Scene::resize(int w, int h) 
{
	width = w;
	height = h;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = (float)w / (float)h;
	fov = 45.0f;
	nearPlane = 0.1f;
	farPlane = 100.0f;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(fov, ratio, nearPlane, farPlane);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

// Calculates FPS
void Scene::calculateFPS()
{
	frame++;
	time = glutGet(GLUT_ELAPSED_TIME);

	if (time - timebase > 1000) {
		sprintf_s(fps, "FPS: %4.2f", frame*1000.0 / (time - timebase));
		timebase = time;
		frame = 0;
	}
}

// Compiles standard output text including FPS and current mouse position.
void Scene::renderTextOutput()
{
	// Render current mouse position and frames per second.
	sprintf_s(mouseText, "Mouse: %i, %i", input->getMouseX(), input->getMouseY());
	
	if (isWireframeOn)
		sprintf_s(wireframeText, "Wireframe: True");
	else
		sprintf_s(wireframeText, "Wireframe: False");

	if (isFullbrightOn)
		sprintf_s(fullbrightText, "Fullbright: True");
	else
		sprintf_s(fullbrightText, "Fullbright: False");

	sprintf_s(positionText, "Pos: %i %i %i", int(mainCam->position.x), int(mainCam->position.y), int(mainCam->position.z));

	displayText(-1.f, 0.96f, 1.f, 0.f, 0.f, mouseText);
	displayText(-1.f, 0.90f, 1.f, 0.f, 0.f, fps);
	displayText(-1.f, 0.84f, 1.f, 0.f, 0.f, wireframeText);
	displayText(-1.f, 0.78f, 1.f, 0.f, 0.f, fullbrightText);

	displayText(-1.f, 0.72f, 1.f, 0.f, 0.f, positionText);
}

// Renders text to screen. Must be called last in render function (before swap buffers)
void Scene::displayText(float x, float y, float r, float g, float b, char* string) {
	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, ((float)width/(float)height), nearPlane, farPlane);
	glMatrixMode(GL_MODELVIEW);
}
