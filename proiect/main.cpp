#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 model2;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightDir2;
glm::vec3 lightColor;
glm::vec3 lightColor2;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightDirLoc2;
GLuint lightColorLoc;
GLuint lightColorLoc2;

// camera
gps::Camera myCamera(
	glm::vec3(-90.0f, 30.0f, -90.0f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 1.0f;

GLboolean pressedKeys[1024];

// models
gps::Model3D carusel;
gps::Model3D rails;
gps::Model3D train;
gps::Model3D teren;
gps::Model3D roata;
gps::Model3D bazaroata;
gps::Model3D cart;
gps::Model3D sun;
gps::Model3D sun2;
gps::Model3D text;
GLfloat angle;
GLfloat rotatieCarusel;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
std::vector<const GLchar*> faces;
// shaders

gps::Shader myBasicShader;


glm::mat4 lightRotation;
glm::mat4 lightRotation2;
GLfloat lightAngle;
GLfloat lightAngle2;

int glWindowWidth = 1900;
int glWindowHeight = 950;
float mouseSpeed;
float horizontalAngle;
float verticalAngle;
float inc = 0.0f;
float inc2 = 0.0f;
int shadeMode;
int a = 0;
bool moving = false;
bool crashed = false;
bool inair = false;
bool hasmoved = false;
float nrinc = 0;
float xpoz, ypoz;

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error = "STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error = "STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}



void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	mouseSpeed = 0.0005f;
	horizontalAngle += mouseSpeed * float(glWindowWidth / 2 - xpos);
	verticalAngle += mouseSpeed * float(glWindowHeight / 2 - ypos);

	myCamera.rotate(verticalAngle, horizontalAngle);
	xpoz = xpos;
	ypoz = ypos;
	glfwSetCursorPos(window, glWindowWidth / 2, glWindowHeight / 2);
	
	view = myCamera.getViewMatrix();
	myBasicShader.useShaderProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void resetTrain() {
	crashed = false;
	hasmoved = false;
	inc2 = 0;
	inc = 0;
	inair = false;
	nrinc = 0;
}
bool animatie = false;
bool anim1 = false;
bool anim2 = false;
bool anim3 = false;
float x = 41;
float z = 160;
float y = 67;
float density = 0.0f;
GLuint fogDensityLoc;

void moreFog() {

	myBasicShader.useShaderProgram();
	density += 0.00005;
	fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, density);

	skyboxShader.useShaderProgram();
	fogDensityLoc = glGetUniformLocation(skyboxShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, density);
	myBasicShader.useShaderProgram();
}

void lessFog() {

	myBasicShader.useShaderProgram();
	if (density > 0)
		density -= 0.00005;
	fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, density);
	skyboxShader.useShaderProgram();
	fogDensityLoc = glGetUniformLocation(skyboxShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, density);
	myBasicShader.useShaderProgram();
}

void processMovement() {

	if (pressedKeys[GLFW_KEY_X]) {
		if (pressedKeys[GLFW_KEY_1])
			shadeMode = 0;
		if (pressedKeys[GLFW_KEY_2])
			shadeMode = 1;
		if (pressedKeys[GLFW_KEY_3])
			shadeMode = 2;
	}


	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		//update view matrix
		view = myCamera.getViewMatrix();
		myBasicShader.useShaderProgram();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		if (!crashed)
			angle -= (nrinc / 30.0f + 0.5f);
		nrinc += 0.1;
		moving = true;
	}
	else
		moving = false;

	if (pressedKeys[GLFW_KEY_E]) {
		a = 1;
	}
	if (pressedKeys[GLFW_KEY_H]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_K]) {
		lightAngle2 -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle2 += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_R]) {
		resetTrain();
	}

	if (pressedKeys[GLFW_KEY_N]) {
		lessFog();
	}

	if (pressedKeys[GLFW_KEY_M]) {
		moreFog();
	}

	if (pressedKeys[GLFW_KEY_V]) {
		if (pressedKeys[GLFW_KEY_1]) {
			x = 41;
			z = 160;
			y = 67;
			animatie = true;
			anim1 = true;
			glfwSetCursorPosCallback(myWindow.getWindow(), NULL);
		}
		if (pressedKeys[GLFW_KEY_2]) {
			animatie = false;
			anim1 = false;
			anim2 = false;
			anim3 = false;
			glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
		}
		
	}
}

void initOpenGLWindow() {
	myWindow.Create(glWindowWidth, glWindowHeight, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
	carusel.LoadModel("models/carusel/carusel3.obj", "models/carusel/");
	rails.LoadModel("models/rails/rails.obj", "models/rails/");
	train.LoadModel("models/rails/train3.obj", "models/rails/");
	teren.LoadModel("models/highlands/teren.obj", "models/highlands/");
	roata.LoadModel("models/roata2/roata.obj", "models/roata2/");
	cart.LoadModel("models/roata2/cart.obj", "models/roata2/");
	bazaroata.LoadModel("models/roata2/bazaroata.obj", "models/roata2/");
	sun.LoadModel("models/sun/sun.obj", "models/sun/");
	sun2.LoadModel("models/sun/sun2.obj", "models/sun/");
	text.LoadModel("models/text/text.obj", "models/text/");
}

void initShaders() {
	myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initUniforms() {
	myBasicShader.useShaderProgram();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
		(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
		0.1f, 3000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 80.0f, 270.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));


	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightDir2 = glm::vec3(0.0f, 80.0f, 270.0f);
	lightRotation2 = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc2 = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir2");
	// send light dir to shader
	glUniform3fv(lightDirLoc2, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation2)) * lightDir2));


	//set light color
	lightColor2 = glm::vec3(1.0f, 0.5f, 0.0f); //orange light
	lightColorLoc2 = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor2");
	// send light color to shader
	glUniform3fv(lightColorLoc2, 1, glm::value_ptr(lightColor2));


	mySkyBox.Load(faces);
}



void renderCarousel(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(5 * rotatieCarusel), glm::vec3(0, 1, 0));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	carusel.Draw(shader);
}

void renderRails(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	rails.Draw(shader);
}


void renderTrain(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(model2, glm::radians(angle), glm::vec3(0, 1, 0));
	model2 = glm::translate(model2, glm::vec3(inc2, inc, inc2));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	train.Draw(shader);
}

void renderTeren(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	teren.Draw(shader);
}

void renderText(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	text.Draw(shader);
}

void renderRoata(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(163.39, 121.59, 44.287));
	model2 = glm::rotate(model2, glm::radians(rotatieCarusel), glm::vec3(1, 0, 0));
	model2 = glm::translate(model2, glm::vec3(-163.39, -121.59, -44.287));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	roata.Draw(shader);
}

void renderCart(gps::Shader shader, float y, float z) {

	model2 = glm::mat4(1.0f);

	model2 = glm::translate(model2, glm::vec3(163.39, 121.59, 44.287));
	model2 = glm::rotate(model2, glm::radians(rotatieCarusel), glm::vec3(1, 0, 0));
	model2 = glm::translate(model2, glm::vec3(-163.39, -121.59, -44.287));

	model2 = glm::translate(model2, glm::vec3(0, y, z));

	model2 = glm::translate(model2, glm::vec3(162.3, 185.2, -27.95));
	model2 = glm::rotate(model2, glm::radians(-rotatieCarusel), glm::vec3(1, 0, 0));
	model2 = glm::translate(model2, glm::vec3(-162.3, -185.2, 27.95));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	cart.Draw(shader);
}

void renderCarts(gps::Shader shader) {
	renderCart(shader, 0, 0);
	renderCart(shader, -28.24, -17.8);
	renderCart(shader, -60.96, -24.17);
	renderCart(shader, -94, -19.25);
	renderCart(shader, -123.2, -3.38);
	renderCart(shader, -145.19, 21.53);
	renderCart(shader, -157.45, 52.49);
	renderCart(shader, -158.27, 85.73);
	renderCart(shader, -147.67, 117.27);
	renderCart(shader, -126.98, 143.76);
	renderCart(shader, -98.56, 161.12);
	renderCart(shader, -66.07, 167.73);
	renderCart(shader, -33.13, 162.81);
	renderCart(shader, -3.88, 146.93);
	renderCart(shader, 18.17, 122.01);
	renderCart(shader, 30.38, 91.05);
	renderCart(shader, 31.23, 57.81);
	renderCart(shader, 20.61, 26.27);

}

void renderBazaRoata(gps::Shader shader) {

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	bazaroata.Draw(shader);
}

void changeLight(gps::Shader shader) {

	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sun.Draw(shader);

}

void changeLight2(gps::Shader shader) {

	lightRotation2 = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(1.0f, 0.0f, 0.0f));
	lightDirLoc2 = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir2");
	// send light dir to shader
	glUniform3fv(lightDirLoc2, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation2)) * lightDir2));

	model2 = glm::mat4(1.0f);
	model2 = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle2), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	sun2.Draw(shader);

}

float p1, p2;
void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	rotatieCarusel += 0.1f;

	skyboxShader.useShaderProgram();
	mySkyBox.Draw(skyboxShader, view, projection);

	if (!inair && a == 1 && inc < 5.0f && !crashed)
	{
		inc += 0.15f;

		if (moving) {
			inc2 += 0.05f;
			hasmoved = true;
		}

		if (inc >= 5.0f)
		{
			a = 0;
			inair = true;
		}
	}
	if (inair && inc > 0.0f && !crashed)
	{
		inc -= 0.15f;
		if (moving) {
			inc2 += 0.05f;
			hasmoved = true;
		}
		if (inc <= 0.0f && hasmoved) {
			inc = 0;
			crashed = true;
			printf("Train crashed!\n");
		}
		if (inc <= 0) {
			inc = 0;
			inair = false;
			a = 0;
		}
	}
	if (!moving)
		nrinc = 0;
	if (crashed)
		a = 0;
	//render the scene
	switch (shadeMode)
	{
	case 0:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 1:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2:
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	}

	myBasicShader.useShaderProgram();

	if(crashed)
		renderText(myBasicShader);


	renderCarousel(myBasicShader);
	renderTeren(myBasicShader);
	renderRails(myBasicShader);
	renderTrain(myBasicShader);

	renderRoata(myBasicShader);
	renderBazaRoata(myBasicShader);
	renderCarts(myBasicShader);
	changeLight(myBasicShader);
	changeLight2(myBasicShader);

	if (animatie) {
		if (x < 150 && anim1) {
			p1 = 550;
			p2 = 0;
			x += 0.1f;
			if (x >= 150) {
				anim1 = false;
				anim2 = true;
			}

		}
		if (z <= 360 && anim2) {
			p1 = 500;
			p2 = 1100;
			z += 0.2f;
			y += 0.2f;
			if (z > 360) {
				anim2 = false;
				anim3 = true;
				x = -190;
				y = 63;
				z = 170;
			}
		}
		if (z >= -50 && anim3) {
			p1 = 550;
			p2 = 300;
			z -= 0.2f;
			if (z < -50) {
				anim3 = false;
				anim1 = true;
				x = 41;
				z = 160;
				y = 67;
			}
		}

		myCamera.setPosition(x, y, z);
		myCamera.rotate(p1, p2); 
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}
}

void cleanup() {
	myWindow.Delete();
	//cleanup code for your own data
}

int main(int argc, const char* argv[]) {

	faces.push_back("skybox/skybox_ft.tga");
	faces.push_back("skybox/skybox_bk.tga");
	faces.push_back("skybox/skybox_up.tga");
	faces.push_back("skybox/skybox_dn.tga");
	faces.push_back("skybox/skybox_rt.tga");
	faces.push_back("skybox/skybox_lf.tga");


	try {
		initOpenGLWindow();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
	setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
} 