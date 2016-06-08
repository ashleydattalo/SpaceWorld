#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Texture.h"
#include "Object.h"

using namespace std;
using namespace Eigen;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = "./"; // Where the resources are loaded from
string RESOURCE_DIR2 = "./";
string name;


shared_ptr<Camera> camera;
shared_ptr<Program> prog;
shared_ptr<Texture> texture0;
shared_ptr<Texture> texture1;
shared_ptr<Texture> texture2;
shared_ptr<Shape> shape;
shared_ptr<Shape> sun;

shared_ptr<Shape> sky;
shared_ptr<Program> progSky;
shared_ptr<Texture> textureCube;

vector<shared_ptr<Object>> objects;


Matrix3f T;
Vector3f lightPosCam;

bool keyToggles[256] = {false}; // only for English keyboards!
bool startAnimation;

float angle = 0;



// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt   = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (xmouse > 0 && xmouse < width && ymouse > 0 && ymouse < height) {
		camera->mouseMoved(xmouse, ymouse);
	}
}

static void char_callback(GLFWwindow *window, unsigned int key)
{
	keyToggles[key] = !keyToggles[key];
	switch(key) {
		case 'x':
			lightPosCam(0) += 0.1;
			break;
		case 'X':
			lightPosCam(0) -= 0.1;
			break;
		case 'y':
			lightPosCam(1) += 0.1;
			break;
		case 'Y':
			lightPosCam(1) -= 0.1;
			break;
		case 'z':
			lightPosCam(2) += 0.1;
			break;
		case 'Z':
			lightPosCam(2) -= 0.1;
			break;
	}

	if (key == 'o') {
		T(0, 2) += .05;
		cout << "\n" << T << endl;
	}

	if (key == 'm') {
		startAnimation = 1;
	}

	if (key == '/') {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	camera->keyPressed(key);
}

// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

float randFloat(float l, float h)
{
	float r = rand() / (float)RAND_MAX;
	return (1.0f - r) * l + r * h;
}

// This function is called once to initialize the scene and OpenGL
static void init()
{
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	prog = make_shared<Program>();
	prog->setShaderNames(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	prog->setVerbose(false);
	prog->init();
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->addAttribute("aTex");
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addUniform("T");
	prog->addUniform("V");
	prog->addUniform("texture0");
	prog->addUniform("texture1");
	prog->addUniform("texture2");
	prog->addUniform("lightPosCam");
	//prog->addUniform("cubemap");

	camera = make_shared<Camera>();
	camera->setInitDistance(3.0f);
	
	texture0 = make_shared<Texture>();
	texture0->setFilename(RESOURCE_DIR + "earthKd.jpg");
	texture0->init();
	texture0->setUnit(0);
	texture0->setWrapModes(GL_REPEAT, GL_REPEAT);
	
	texture1 = make_shared<Texture>();
	texture1->setFilename(RESOURCE_DIR + "earthKs.jpg");
	texture1->init();
	texture1->setUnit(1);
	texture1->setWrapModes(GL_REPEAT, GL_REPEAT);
	
	texture2 = make_shared<Texture>();
	texture2->setFilename(RESOURCE_DIR + "earthClouds.jpg");
	texture2->init();
	texture2->setUnit(2);
	texture2->setWrapModes(GL_REPEAT, GL_REPEAT);
	
	T.setIdentity();

	lightPosCam << 1.0f, 1.0f, -3.0f;
	
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "sphere.obj");
	shape->fitToUnitBox();
	texture2->setUnit(10);
	shape->init();

	sun = make_shared<Shape>();
	sun->loadMesh(RESOURCE_DIR + "sphere.obj");
	sun->fitToUnitBox();
	sun->init();
	

	shared_ptr<Object> planet = make_shared<Object>();
	shared_ptr<Object> sun = make_shared<Object>();
	shared_ptr<Object> ship = make_shared<Object>();

	objects.push_back(planet);
	objects.push_back(sun);
	objects.push_back(ship);

	objects[0]->shape = make_shared<Shape>();
	objects[1]->shape = make_shared<Shape>();
	objects[2]->shape = make_shared<Shape>();

	objects[0]->shape->loadMesh(RESOURCE_DIR + "sphere.obj");
	objects[0]->shape->fitToUnitBox();
	objects[0]->shape->init();

	objects[0]->position = { -50, 50, 0 };
	objects[0]->scale = { 1.0f, 1.0f, 1.0f };
	objects[0]->xRot = randFloat(0, 360);
	objects[0]->ka = { randFloat(0, 0.2f), randFloat(0, 0.2f), randFloat(0, 0.2f) };
	objects[0]->ks = { 0.8f, 0.7f, 0.7f };
	objects[0]->kd = { 1.0f, 0.9f, 0.8f };
	objects[0]->velocity = { 1.0f, 0.9f, 0.8f };
	objects[0]->mass = 5.971f;
	objects[0]->c = 1.0f;

	objects[1]->shape->loadMesh(RESOURCE_DIR + "station.obj");
	objects[1]->shape->fitToUnitBox();
	objects[1]->shape->init();

	objects[1]->position = { 10, 0, 0};
	objects[1]->scale = { 1.0f, 1.0f, 1.0f };
	objects[1]->xRot = randFloat(0, 360);
	objects[1]->ka = { randFloat(0, 0.2f), randFloat(0, 0.2f), randFloat(0, 0.2f) };
	objects[1]->ks = { 0.8f, 0.7f, 0.7f };
	objects[1]->kd = { 1.0f, 0.9f, 0.8f };
	objects[1]->velocity = { 0, 0, 0 };
	objects[1]->mass = 10000.0f;
	objects[1]->c = 5.0f;

	objects[2]->shape->loadMesh(RESOURCE_DIR + "executor.obj");
	objects[2]->shape->fitToUnitBox();
	objects[2]->shape->init();

	objects[2]->position = { 10, 0, 0};
	objects[2]->scale = { 1.0f, 1.0f, 1.0f };
	objects[2]->xRot = randFloat(0, 360);
	objects[2]->ka = { randFloat(0, 0.2f), randFloat(0, 0.2f), randFloat(0, 0.2f) };
	objects[2]->ks = { 0.8f, 0.7f, 0.7f };
	objects[2]->kd = { 1.0f, 0.9f, 0.8f };
	objects[2]->velocity = { 0, 0, 0 };
	objects[2]->mass = 10000.0f;
	objects[2]->c = 5.0f;



	//cube map stuff
	sky = make_shared<Shape>();
	sky->loadMesh(RESOURCE_DIR + "sphere.obj");
	sky->init();

	progSky = make_shared<Program>();
	progSky->setShaderNames(RESOURCE_DIR + "sky_vert.glsl", RESOURCE_DIR + "sky_frag.glsl");
	progSky->setVerbose(false);
	progSky->init();
	progSky->addAttribute("aPos");
	progSky->addAttribute("aNor");
	progSky->addUniform("P");
	progSky->addUniform("MV");
	progSky->addUniform("cubemap");

	textureCube = make_shared<Texture>();
	textureCube->setFilenamesTexCube(RESOURCE_DIR + "negx.jpg",
							  RESOURCE_DIR + "posx.jpg",
							  RESOURCE_DIR + "posy.jpg",
							  RESOURCE_DIR + "negy.jpg",
							  RESOURCE_DIR + "negz.jpg",
							  RESOURCE_DIR + "posz.jpg");

	textureCube->setFilenamesTexCube(RESOURCE_DIR2 + name + "_lf.tga",
							  RESOURCE_DIR2 + name +  "_rt.tga",
							  RESOURCE_DIR2 + name +  "_dn.tga",
							  RESOURCE_DIR2 + name +  "_up.tga",
							  RESOURCE_DIR2 + name +  "_ft.tga",
							  RESOURCE_DIR2 + name +  "_bk.tga");
	textureCube->initTexCube();



	GLSL::checkError(GET_FILE_LINE);
}

// This function is called every frame to draw the scene.
static void render()
{
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'c']) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(keyToggles[(unsigned)'l']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	
	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();
	camera->applyViewMatrix(MV);

	if (startAnimation) {
		T(0, 2) += .0005;
		T(1, 2) += .001;

		lightPosCam(2) += 0.01;

	}

	// Draw sky sphere
	progSky->bind();
	textureCube->bindTexCube(progSky->getUniform("cubemap"));
	MV->pushMatrix();
	MV->scale(500.0f);
	glUniformMatrix4fv(progSky->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	glUniformMatrix4fv(progSky->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
	sky->draw(progSky);
	MV->popMatrix();
	textureCube->unbindTexCube();
	progSky->unbind();




	prog->bind();

	// glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, MV->topMatrix().data());


	texture0->bind(prog->getUniform("texture0"));
	texture1->bind(prog->getUniform("texture0"));
	texture2->bind(prog->getUniform("texture2"));


	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
	glUniformMatrix3fv(prog->getUniform("T"), 1, GL_FALSE, T.data());
	glUniform3f(prog->getUniform("lightPosCam"), lightPosCam[0], lightPosCam[1], lightPosCam[2]);

	// MV->pushMatrix();
	// MV->translate(lightPosCam);
	// glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	// glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
	// sun->draw(prog);
	// MV->popMatrix();


	MV->pushMatrix();
	MV->translate(objects[0]->position);
	MV->scale(10, 10, 10);
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
	objects[0]->step(.01f);
	objects[0]->shape->draw(prog);
	texture1->unbind();
	texture0->unbind();
	MV->popMatrix();

	MV->pushMatrix();
	MV->translate(objects[1]->position);
	MV->rotate(45, 0, 0, 1);
	MV->scale(20, 20, 20);
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
	objects[1]->step(.01f);
	objects[1]->shape->draw(prog);
	texture1->unbind();
	texture0->unbind();
	MV->popMatrix();

	MV->pushMatrix();
	MV->translate(objects[1]->position);
	MV->rotate(45, 0, 1, 0);
	MV->scale(20, 20, 20);
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P->topMatrix().data());
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, MV->topMatrix().data());
	objects[2]->step(.01f);
	objects[2]->shape->draw(prog);
	texture1->unbind();
	texture0->unbind();
	MV->popMatrix();

	prog->unbind();
	
	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv)
{
	if(argc < 1) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");
	RESOURCE_DIR2 = argv[2] + string("/");
	name = argv[3];

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Ashley Dattalo", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
