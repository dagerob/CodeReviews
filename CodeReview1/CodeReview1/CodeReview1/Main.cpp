#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::vector<glm::vec3> calculateTangents(float t, float b, float c, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
glm::vec3 calculatePoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 m0, glm::vec3 m1);

void init();
void display();
void speedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void UpdateInterpolationData();

const int PATH_LENGHT = 20;
const float B = 0.0f, TEN = 0.0f, C = 0.0f;

GLFWwindow* window;
Shader* shaderProgram;
float cameraSpeed = 0.0f;
bool error = false;
unsigned int VBO, VAO, EBO;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
int pathIndex = -1;

std::vector<glm::vec3> tangents;
glm::quat helpQuat1;
glm::quat helpQuat2;
glm::vec3 lastPoint;

float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

glm::vec3 lookDirections[] = {
	glm::vec3(1, 0, -1),
	glm::vec3(0.7, 0, -1),
	glm::vec3(0.5, 0, -1),
	glm::vec3(0.3, 0, -1),
	glm::vec3(0.1, 0, -1),
	glm::vec3(0, 0, -1),
	glm::vec3(-0.1, 0, -1),
	glm::vec3(-0.4, 0, -1),
	glm::vec3(-0.6, 0, -1),
	glm::vec3(-0.9, 0, -1),
	glm::vec3(-1, 0, -1),
	glm::vec3(-1, 0, -0.8),
	glm::vec3(-1, 0, -0.5),
	glm::vec3(-1, 0, -0.2),
	glm::vec3(-1, 0, 0),
	glm::vec3(-1, 0, 0.1),
	glm::vec3(-1, 0, 0.5),
	glm::vec3(-1, 0, 1),
	glm::vec3(-0.5, 0, 1),
	glm::vec3(0, 0, 1)
};

glm::quat lookDirQuaternions[PATH_LENGHT];

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(0.0f,  -1.0f, 2.0f),
	glm::vec3(0.0f, 0.0f, 4.0f),
	glm::vec3(0.0f,  1.0f,  6.0f),
	glm::vec3(0.0f,  0.0f, 8.0f),
	glm::vec3(6.0f,  0.0f,  0.0f),
	glm::vec3(6.0f,  -1.0f, 2.0f),
	glm::vec3(6.0f, 0.0f, 4.0f),
	glm::vec3(6.0f,  1.0f,  6.0f),
	glm::vec3(6.0f,  0.0f, 8.0f),
	glm::vec3(6.0f,  0.0f,  -10.0f),
	glm::vec3(6.0f,  -1.0f, -2.0f),
	glm::vec3(6.0f, 0.0f, -4.0f),
	glm::vec3(6.0f,  1.0f,  -6.0f),
	glm::vec3(6.0f,  0.0f, -8.0f),
	glm::vec3(0.0f,  0.0f,  -10.0f),
	glm::vec3(0.0f,  -1.0f, -2.0f),
	glm::vec3(0.0f, 0.0f, -4.0f),
	glm::vec3(0.0f,  1.0f,  -6.0f),
	glm::vec3(0.0f,  0.0f, -8.0f),
};


glm::vec3 cameraPath[] = {
	glm::vec3(1.0f,  1.0f,  0.0f),
	glm::vec3(1.0f,  0.0f, 2.0f),
	glm::vec3(1.0f, 1.0f, 4.0f),
	glm::vec3(1.0f,  2.0f,  6.0f),
	glm::vec3(1.0f,  1.0f, 8.0f),
	glm::vec3(5.0f,  1.0f,  0.0f),
	glm::vec3(5.0f,  0.0f, 2.0f),
	glm::vec3(5.0f, 1.0f, 4.0f),
	glm::vec3(5.0f,  2.0f,  6.0f),
	glm::vec3(5.0f,  1.0f, 8.0f),
	glm::vec3(5.0f,  1.0f,  -10.0f),
	glm::vec3(5.0f,  0.0f, -2.0f),
	glm::vec3(5.0f, 1.0f, -4.0f),
	glm::vec3(5.0f,  2.0f,  -6.0f),
	glm::vec3(5.0f,  1.0f, -8.0f),
	glm::vec3(1.0f,  1.0f,  -10.0f),
	glm::vec3(1.0f,  0.0f, -2.0f),
	glm::vec3(1.0f, 1.0f, -4.0f),
	glm::vec3(1.0f,  2.0f,  -6.0f),
	glm::vec3(1.0f,  1.0f, -8.0f),
};

/*glm::vec3 cameraPath[] = {
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f),
	glm::vec3(0.0f,0.0f,3.0f)
};*/

int main()
{
	init();
	if (error)
		return -1;

	display();

	delete shaderProgram;
	glfwTerminate();
	return 0;
}

void init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(800, 600, "CodeReview1", NULL, NULL);
	// create window
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		error = true;
		return;
	}
	glfwMakeContextCurrent(window);

	// initialize glad with the pointer to the corresponding functions for the drivers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		error = true;
		return;
	}
	 // we create the viewport and set the resize callback function
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	 // we set the callback for the speed input
	glfwSetKeyCallback(window, speedCallback);

	// we load and compile the vertex and fragment shader to display the cubes in the scene
	shaderProgram = new Shader("D:/DanielPers/Master/Semester 1/EchtzeitGrafik/CodeReview1/CodeReview1/CodeReview1/VertexShader.txt",
		"D:/DanielPers/Master/Semester 1/EchtzeitGrafik/CodeReview1/CodeReview1/CodeReview1/FragmentShader.txt");
	// we enable the depht buffer
	glEnable(GL_DEPTH_TEST);

	// we create the OpenGL objects to store the vertex and normal data of the cubes
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// we load the ertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// we bind the vertex data to this OpenGL object
	glBindVertexArray(VAO);

	// we tell OpenGL which data is being sent to the shaders
	// the vertex coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// the normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// we finish by bindint the buffers to tell OpenGL which buffer to use.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void display() {
	// we create the array of quaternions represening the lookDirections stored in the array of vec3s
	glm::vec3 initialOrientation = glm::vec3(0,0,-1);
	for (int i = 0; i < PATH_LENGHT; i++) {
		lookDirQuaternions[i] = glm::rotation(glm::normalize(initialOrientation), glm::normalize(lookDirections[i]));
	}

	// we initialize some matrixes
	// the view matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;
	// the projection matrix has to be made only once
	projection = glm::perspective(glm::radians(-45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	// we initialize all data for the interpolation
	float t = 0.0f, tq = 1.0f;
	pathIndex = -1;
	lastPoint = cameraPath[0];
	float thisDistance = 0.0f;
	UpdateInterpolationData();

	// the main draw loop
	while (!glfwWindowShouldClose(window))
	{
		// we calculate the deltaTime to maintain a constant speed acrros frames and different machines
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// function to proccess the user's intput
		processInput(window);

		// we clear last scene and paint the background with black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// we tell OpenGL which VAO we want to draw
		glBindVertexArray(VAO);

		// we use our shader class to tell OpenGL which shaers we want to use
		shaderProgram->use();

		// we load some data to the shaders, like the lightsource's position and the projection matrix
		shaderProgram->setVec3F("lightPos", glm::vec3(8, 10, 7));
		shaderProgram->setMat4F("projection", projection);

		// we have to maintain a contant speed
		// t has no linear relation with how far between the two interpolated points one is
		// we use the general definition ov velocity, velocity is the rate of change of the position
		//		velocity is equal to the vector between point 1 and 2
		// we want to maintain the same speed (magnitude of velocity) 
		// we add a small vale to t until we obtain a point whose distance to the starting pint is equal to the velocity
		// we save that point and t and process the rest
		// if the next point would need a t larger than one, we rest t and calculate the tangents for the next interval
		// we multiply the speed with deltaTime to maintain a normal velocity in different systems and framerates
		glm::vec3 thisPoint;
		while (thisDistance < cameraSpeed * deltaTime) {
			if (t < 1) {
				t += 0.0001f;
			}
			else {
				t = 0;
				UpdateInterpolationData();
			}
			thisPoint = calculatePoint(t, cameraPath[pathIndex], cameraPath[pathIndex + 1], tangents[0], tangents[1]);
			thisDistance = glm::distance(lastPoint, thisPoint);
		}
		lastPoint = calculatePoint(t, cameraPath[pathIndex], cameraPath[pathIndex + 1], tangents[0], tangents[1]);
		thisDistance = 0.0f;
		// we update the rotation using the same value t
		glm::quat thisRot = glm::squad(lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1], helpQuat1, helpQuat1, t);

		// we update the view matrix
		view = glm::lookAt(lastPoint,
			lastPoint + thisRot * initialOrientation,
			glm::vec3(0.0f, 1.0f, 0.0f));
		shaderProgram->setMat4F("view", view);

		// we calculate the transformation matrixes of all the cubes
		for (int i = 0; i < 20; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			shaderProgram->setMat4F("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

std::vector<glm::vec3> calculateTangents(float t, float b, float c, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	std::vector<glm::vec3> result(2);

	float coef1, coef2, coef3, coef4;
	coef1 = (1 - t)*(1 + b)*(1 + c) / 2;
	coef2 = (1 - t)*(1 - b)*(1 - c) / 2;
	coef3 = (1 - t)*(1 + b)*(1 - c) / 2;
	coef4 = (1 - t)*(1 - b)*(1 + c) / 2;

	result[0] = coef1 * (p1 - p0) + coef2 * (p2 - p1);
	result[1] = coef3 * (p2 - p1) + coef4 * (p3 - p2);
	return result;
}

glm::vec3 calculatePoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 m0, glm::vec3 m1) {
	float h00, h10, h01, h11;
	h00 = (1 + 2 * t)*(1 - t)*(1 - t);
	h10 = t * (1 - t)*(1 - t);
	h01 = t * t*(3 - 2 * t);
	h11 = t * t*(t - 1);
	return h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1;
}

void UpdateInterpolationData() {
	if (pathIndex < PATH_LENGHT - 2) {
		pathIndex++;
		std::cout << pathIndex << std::endl;
		if (pathIndex == 0) {
			tangents = calculateTangents(B, TEN, C, cameraPath[0], cameraPath[0], cameraPath[1], cameraPath[2]);
			helpQuat1 = glm::intermediate(lookDirQuaternions[0], lookDirQuaternions[0], lookDirQuaternions[1]);
			helpQuat2 = glm::intermediate(lookDirQuaternions[0], lookDirQuaternions[1], lookDirQuaternions[2]);
		}
		else if (pathIndex == PATH_LENGHT - 2) {
			tangents = calculateTangents(B, TEN, C, cameraPath[pathIndex - 1], cameraPath[pathIndex], cameraPath[pathIndex + 1], cameraPath[pathIndex + 1]);
			helpQuat1 = glm::intermediate(lookDirQuaternions[pathIndex - 1], lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1]);
			helpQuat2 = glm::intermediate(lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1], lookDirQuaternions[pathIndex + 1]);
		}
		else {
			tangents = calculateTangents(B, TEN, C, cameraPath[pathIndex - 1], cameraPath[pathIndex], cameraPath[pathIndex + 1], cameraPath[pathIndex + 2]);
			helpQuat1 = glm::intermediate(lookDirQuaternions[pathIndex - 1], lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1]);
			helpQuat2 = glm::intermediate(lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1], lookDirQuaternions[pathIndex + 2]);
		}
	}
	else {
		pathIndex = 0;
		tangents = calculateTangents(B, TEN, C, cameraPath[0], cameraPath[0], cameraPath[1], cameraPath[2]);
		helpQuat1 = glm::intermediate(lookDirQuaternions[PATH_LENGHT - 1], lookDirQuaternions[0], lookDirQuaternions[1]);
		helpQuat2 = glm::intermediate(lookDirQuaternions[0], lookDirQuaternions[1], lookDirQuaternions[2]);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// code to update the Viewport if the window changes size
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	// code to quit if ESC is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void speedCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		cameraSpeed += 0.5f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS && cameraSpeed >= 0.1f) {
		cameraSpeed -= 0.5f;
	}

	
}