#include "Scene.h"

// Scene constructor, initilises OpenGL
// You should add further variables to need initilised.
Scene::Scene(Input *in)
{
	// Store pointer for input class
	input = in;
	initialiseOpenGL();

	// Other OpenGL / render setting should be applied here.

	// Initialise scene variables
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	cam = new Camera(input);

	if (!baseModel.load("models/station_base.obj", "gfx/crate.png"))
		printf("Model 'Base' failed to load");

	if (!skyboxModel.load("models/station_skybox.obj", "gfx/space.png"))
		printf("Model 'Skybox' failed to load");

	if (!pipesModel.load("models/station_pipes.obj", "gfx/pipe.png"))
		printf("Model 'Pipes' failed to load");

	glBindTexture(GL_TEXTURE_2D, NULL);
}

void Scene::handleInput(float dt)
{
	// Handle user input
	cam->HandleInput(dt);

	//If the Q key is pressed, enable or disable wireframe mode
	if (input->isKeyDown(113))
	{
		isWireframeOn = !isWireframeOn;
		input->setKeyUp(113);
	}


	//If the F key is pressed, enable or disable fullbright mode
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
}

void Scene::update(float dt)
{
	// update scene related variables.

	// Calculate FPS for output
	calculateFPS();
}

void Scene::render() {

	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(cam->GetPositionView().x, cam->GetPositionView().y, cam->GetPositionView().z, cam->lookAt.x, cam->lookAt.y, cam->lookAt.z, cam->GetUpView().x, cam->GetUpView().y, cam->GetUpView().z);
	
	// Render geometry/scene here -------------------------------------	

	//Renders models either with or without wireframe on
	baseModel.render(isWireframeOn);
	skyboxModel.render(isWireframeOn);
	pipesModel.render(isWireframeOn);

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

	cam->xCentre = width / 2;
	cam->yCentre = height / 2;

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

	displayText(-1.f, 0.96f, 1.f, 0.f, 0.f, mouseText);
	displayText(-1.f, 0.90f, 1.f, 0.f, 0.f, fps);
	displayText(-1.f, 0.84f, 1.f, 0.f, 0.f, wireframeText);
	displayText(-1.f, 0.78f, 1.f, 0.f, 0.f, fullbrightText);
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
