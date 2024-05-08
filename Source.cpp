#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include "Sphere.h"

using namespace std;

float radius = 9.0; // (8 <= radius <= 10)

// Sphere
Sphere lightSourceSphere(48);
float sphereDiameter = 0.5f;

float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

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

/* Vertex buffer objektum és vertex array objektum az adattároláshoz. */
/* Vertex buffer object ID and vertex array object ID for data storing. */
GLuint cubeVBO, cubeVAO, lightSourceVAO, lightSourceVBO;

int			window_width = 600;
int			window_height = 600;
char		window_title[] = "Assignment 3";
/** A normál billentyûk a [0..255] tartományban vannak, a nyilak és a speciális billentyûk pedig a [256..511] tartományban helyezkednek el. */
/** Normal keys are fom [0..255], arrow and special keys are from [256..511]. */
GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;
GLuint		lightningShader;
GLuint		lightCubeShader;
GLuint		transformLoc;

/** Vetítési és kamera mátrixok felvétele. */
glm::mat4		model = glm::mat4(1.0f),
view,
projection = glm::perspective(glm::radians(55.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);

GLdouble		deltaTime, lastTime = glfwGetTime();
GLfloat			cameraSpeed;

/* Vegyük fel a kamera pozicót tároló változót, illetve a tengelyekhezz szükséges vektorokat. */
glm::vec3		cameraPos = glm::vec3(radius, 0.0f, 0.0f), // (r; 0; 0), where (8 <= r <= 10)
cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f),
cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f),
cameraDirection = glm::normalize(cameraPos - cameraTarget),
cameraRight = glm::normalize(glm::cross(cameraUpVector, cameraDirection));
//cameraUp		= glm::cross(cameraDirection, cameraRight); // Ugyan az, mint a cameraUpVector

// Lighting
std::vector<glm::vec3> spherePoints;

glm::vec3		objectColor = glm::vec3(1.0f, 1.0f, 1.0f),
lightColor = glm::vec3(0.0f, 1.0f, 1.0f),
lightPos = glm::vec3(2.0f * radius, 0.0f, 0.0f);

glm::mat4		lightModel = glm::translate(glm::mat4(1.0f), lightPos);


bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram(const char* vertexShader, const char* fragmentShader) {

	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	string vertShaderStr = readShaderSource(vertexShader);
	string fragShaderStr = readShaderSource(fragmentShader);

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}


	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	// Shader program objektum létrehozása. Eltároljuk az ID értéket.
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return vfProgram;
}

/** Kiszámoljuk a kamera mátrixokat. */
void computeCameraMatrix() {
	/* A paraméterek rendre: az új koordinátarendszerünk középpontja (hol a kamera), merre néz a kamera, mit tekintünk ,,fölfele" iránynak */
	//cameraTarget = cameraPos + cameraDirection;
	//float camX = sin(glfwGetTime()) * radius;
	//float camY = cos(glfwGetTime()) * radius;
	//view = glm::lookAt(glm::vec3(camX, camY, 0.0f), cameraTarget, cameraUpVector);

	view = glm::lookAt(cameraPos, cameraTarget, cameraUpVector);
	//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void init(GLFWwindow* window) {
	glEnable(GL_DEPTH_TEST);

	lightningShader = createShaderProgram("basicLightingVertexShader.glsl", "basicLightingFragmentShader.glsl");
	lightCubeShader = createShaderProgram("lightSphereVertexShader.glsl", "lightSphereFragmentShader.glsl");

	// Sphere
	std::vector<int> ind = lightSourceSphere.getIndices();
	std::vector<glm::vec3> vert = lightSourceSphere.getVertices();

	std::vector<float> pvalues; // vertex positions

	int numIndices = lightSourceSphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
	}

	// VAOs and VBOs

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// lightSourceVAO and VBO
	glGenVertexArrays(1, &lightSourceVAO);
	glGenBuffers(1, &lightSourceVBO);

	glBindBuffer(GL_ARRAY_BUFFER, lightSourceVBO);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindVertexArray(lightSourceVAO);

	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glUseProgram(renderingProgram);
	//glUseProgram(lightRenderingProgram);

	glPointSize(10.0f);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void display(GLFWwindow* window, double currentTime) {
	/* Töröljük le a kiválasztott buffereket! Fontos lehet minden egyes alkalommal törölni! */
	/* Let's clear the selected buffers! Usually importand to clear it each time! */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Let's compensate for the different time that is needed for calculating a frame! */
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	// this makes the animation continous, because camera speed proportional with elapsed time
	cameraSpeed = 2.5f * (GLfloat)deltaTime;

	// we will use two set ups for moving: WASD and the arrow keys
	if ((keyboard[GLFW_KEY_W]) || (keyboard[GLFW_KEY_UP]))
		cameraPos += cameraSpeed * cameraUpVector; // Moving UP along the Z axis

	if ((keyboard[GLFW_KEY_S]) || (keyboard[GLFW_KEY_DOWN]))
		cameraPos -= cameraSpeed * cameraUpVector;

	if ((keyboard[GLFW_KEY_A]) || (keyboard[GLFW_KEY_LEFT]))
		cameraPos = glm::rotateZ(cameraPos, -cameraSpeed);

	if ((keyboard[GLFW_KEY_D]) || (keyboard[GLFW_KEY_RIGHT]))
		cameraPos = glm::rotateZ(cameraPos, cameraSpeed);


	computeCameraMatrix();

	glUseProgram(lightningShader);
	glUniform3fv(glGetUniformLocation(lightningShader, "objectColor"), 1, glm::value_ptr(objectColor));
	glUniform3fv(glGetUniformLocation(lightningShader, "lightColor"), 1, glm::value_ptr(lightColor));
	glUniform3fv(glGetUniformLocation(lightningShader, "lightPos"), 1, glm::value_ptr(lightPos));

	// View/projection transformations
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(cubeVAO);

	// Middle
	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Top
	model = glm::translate(glm::mat4(1.0f), cameraUpVector * 2.0f);
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/*// Middle-right TEST
	model = glm::translate(glm::mat4(1.0f), cameraRight * 2.0f);
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);*/

	// Bottom
	model = glm::translate(glm::mat4(1.0f), cameraUpVector * -2.0f);
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Light source
	glUseProgram(lightCubeShader);

	glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));

	lightPos = glm::rotateZ(lightPos, cameraSpeed / 2.0f);
	model = glm::translate(glm::mat4(1.0f), lightPos);
	model = glm::scale(model, glm::vec3(sphereDiameter / 2.0));
	glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glUniform3fv(glGetUniformLocation(lightCubeShader, "lightColor"), 1, glm::value_ptr(lightColor));

	glBindVertexArray(lightSourceVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, lightSourceSphere.getNumIndices());

	/* Leválasztjuk a VAO-t, nehogy bármilyen érték felülíródjon. */
	/* Detach VAO for safety reasons, not to modify it accidentaly. */
	glBindVertexArray(0);
}

/** Felesleges objektumok törlése. */
/** Clenup the unnecessary objects. */
void cleanUpScene() {
	/** Töröljük a vertex array és a vertex buffer objektumokat. */
	/** Destroy the vertex array and vertex buffer objects. */
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightSourceVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &lightSourceVBO);

	/** Töröljük a shader programot. */
	/** Let's delete the shader program. */
	//glDeleteProgram(renderingProgram);

	/** Töröljük a GLFW ablakot. */
	/** Destroy the GLFW window. */
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t. */
	/** Stop the GLFW system. */
	glfwTerminate();
	/** Kilépés EXIT_SUCCESS kóddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	exit(EXIT_SUCCESS);
	//	glfwSetWindowShouldClose(window, true);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	GLfloat			aspect_ratio = 1.0;

	window_width = width;
	window_height = height;

	if (window_height != 0)
		aspect_ratio = (GLfloat)window_width / (GLfloat)window_height;
	/** A kezelt képernyõ beállítása a teljes (0, 0, width, height) területre. */
	/** Set the viewport for the full (0, 0, width, height) area. */
	glViewport(0, 0, width, height);

	/** Vetítési mátrix kiszámítása. */
	/** After resize, projection shall be recalculated. */
	projection = glm::perspective(glm::radians(55.0f), aspect_ratio, 0.1f, 100.0f);
	/** Átadjuk az új mátrixot a vertex shader-nek. */
	/** Pass the new matrix to the vertex shader. */
	glUniformMatrix4fv(glGetUniformLocation(lightningShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/** ESC billentyûre kilépés. */
	/** Exit on ESC key. */
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
		cleanUpScene();
	}

	/** A billentyûk lenyomásának és felengedésének regisztrálása. Lehetõvé teszi gombkombinációk használatát. */
	/** Let's register press and release events for keys. Enables the usage of key combinations. */
	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	/** Az egér mutató helyét kezelõ függvény. */
	/** Callback function for mouse position change. */
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	/** Az egér gombjaihoz köthetõ események kezelése. */
	/** Callback function for mouse button events. */
}

int main(void) {
	/** Próbáljuk meg inicializálni a GLFW-t! */
	/** Try to initialize GLFW! */
	if (!glfwInit())
		exit(EXIT_FAILURE);

	/** A használni kívánt OpenGL verzió: 4.3. */
	/** The needed OpenGL version: 4.3. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed.
	/** Próbáljuk meg létrehozni az ablakunkat. */
	/** Let's try to create a window for drawing. */
	/** GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor * monitor, GLFWwindow * share) */
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	/** Select the OpenGL context (window) for drawing. */
	glfwMakeContextCurrent(window);

	/** A képernyõ átméretezés kezelése. */
	/** Callback function for window size change. */
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	/** Billentyûzethez köthetõ események kezelése. */
	/** Callback function for keyboard events. */
	glfwSetKeyCallback(window, keyCallback);
	/** Az egér mutató helyét kezelõ függvény megadása. */
	/** Callback function for mouse position change. */
	glfwSetCursorPosCallback(window, cursorPosCallback);
	/** Az egér gombjaihoz köthetõ események kezelése. */
	/** Callback function for mouse button events. */
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények, probléma esetén kilépés EXIT_FAILURE értékkel. */
	/** Initalize GLEW, so the OpenGL functions will be available, on problem exit with EXIT_FAILURE code. */
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	/** 0 = v-sync kikapcsolva, 1 = v-sync bekapcsolva, n = n db képkockányi idõt várakozunk */
	/** 0 = v-sync off, 1 = v-sync on, n = n pieces frame time waiting */
	glfwSwapInterval(1);

	/** A window ablak minimum és maximum szélességének és magasságának beállítása. */
	/** The minimum and maximum width and height values of the window object. */
	// glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	/** A window oldalarányának megadása a számláló és az osztó segítségével. */
	/** Setting the aspect ratio using the numerator and the denominator values. */
	// glfwSetWindowAspectRatio(window, 1, 1);

	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések, pl. a shader objektumok létrehozása. */
	/** The first initialization steps of the program, e.g.: creating the shader objects. */
	init(window);

	/** A megadott window struktúra "close flag" vizsgálata. */
	/** Checks the "close flag" of the specified window. */
	while (!glfwWindowShouldClose(window)) {
		/** A kód, amellyel rajzolni tudunk a GLFWwindow objektumunkba. */
		/** Call display function which will draw into the GLFWwindow object. */
		display(window, glfwGetTime());
		/** Double buffered mûködés. */
		/** Double buffered working = swap the front and back buffer here. */
		glfwSwapBuffers(window);
		/** Események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás. */
		/** Handle events related to our window, e.g.: pressing a key or moving the mouse. */
		glfwPollEvents();
	}

	/** Felesleges objektumok törlése. */
	/** Clenup the unnecessary objects. */
	cleanUpScene();

	/** Kilépés EXIT_SUCCESS kóddal. */
	/** Stop the software and exit with EXIT_SUCCESS code. */
	return EXIT_SUCCESS;
}