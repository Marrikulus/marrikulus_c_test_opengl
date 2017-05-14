#include <stdio.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_GL_3
#include <GLFW/glfw3.h>
#include <linmath.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#define GL_LOG_FILE "gl.log"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

#ifndef deg2rad
#define deg2rad(a)  ((a)*(M_PI/180))
#endif
#ifndef rad2deg
#define rad2deg(a)  ((a)*(180/M_PI))
#endif
#include <Camera.h>
#include <Shader.h>

///////////////////////////////////////////////////
////////	HEAD

typedef struct
{
	vec3 position;
	vec2 uv;
} Vertex;


void mouseCallback(GLFWwindow* window, double positionX, double positionY);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void error_callback(int error, const char* description);
void window_close_callback(GLFWwindow* window);

GLuint makeTexture(char* path, GLenum format);

int gl_log_err(const char* message, ...);
int gl_log(const char* message, ...);
int restart_gl_log();
void updateFpsCounter(GLFWwindow* window);
void moveCamera();

//static void show_info_log( GLuint object, PFNGLGETSHADERIVPROC glGet__iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);


///////////////////////////////////////////////////
////////	GLOBALS

// window
GLFWwindow* window;
//const unsigned int WIDTH = 800, HEIGHT = 600;
GLfloat mixValue = 1.0f;

Camera camera;
ShaderProgram shader;
// Time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Keyboard
bool keys[1024];


///////////////////////////////////////////////////



//static GLuint makeBuffer(
//	GLenum target,
//	const void *bufferData,
//	GLsizei bufferSize
//){
//	GLuint buffer;
//	glGenBuffers(1, &buffer);
//	glBindBuffer(target, buffer);
//	glBufferData(target, bufferSize, bufferData, GL_STATIC_DRAW);
//	return buffer;
//}


int init_game(int width, int height) {
	assert(restart_gl_log());

	gl_log("starting GLFW\n%s\n", glfwGetVersionString());
	glfwSetErrorCallback(error_callback);
	int initResult = glfwInit();
	if (!initResult) {
		gl_log_err("Unable to init GLFW %d \n",initResult);
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* mon = glfwGetPrimaryMonitor();
	const GLFWvidmode* vmode = glfwGetVideoMode(mon);

	window = glfwCreateWindow(
		SCREEN_WIDTH /*vmode->width*/, SCREEN_HEIGHT/*vmode->height*/, "Merancia", NULL, NULL
	);

	if (!window)
	{
		printf("Unable to create window!\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
	{
		return gl_log_err("Unable to init glad \n");
	}

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_DEPTH_TEST); // enable depth-testing

	camera = initCamera();
	updateCameraVectors(&camera);
	//callbacks
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);

	return 0;
}

GLuint makeTexture(char* path, GLenum format)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	int width,height,nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("failed to load texture!\n");
		stbi_image_free(data);
		return 0;
	}
	stbi_image_free(data);

	return texture;
}
static double previousSeconds = 0;



void updateFpsCounter(GLFWwindow* window)
{
	static int frameCount;
	if (previousSeconds == 0)
	{
		previousSeconds = glfwGetTime();
	}
	double currentSeconds = glfwGetTime();
	double elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25)
	{
		double fps = (double)frameCount / elapsedSeconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		previousSeconds = currentSeconds;
		frameCount = 0;
	}
	frameCount++;
}



void start_game(){
	printf("Starting Game\n");

	//-------------Loading----------------//
	shader = makeShaderProgram("../Shaders/FragmentShader.frag", "../Shaders/VertexShader.vert");

	if (shader.program == 0)
	{
		printf("Shader Failed\n");
		return;
	}


//	vec4 vector = {1.0f, 0.0f, 0.0f, 1.0f};
//	vec4 newdirection = {0.0f, 0.0f, 0.0f, 0.0f};
//	mat4x4 trans;
//	mat4x4_identity(trans);
//	mat4x4_translate(trans, 1,1,0);
//	printf("x:%f y:%f z:%f w:%f\n", trans[0][0], trans[1][0],trans[2][0],trans[3][0]);
//	printf("x:%f y:%f z:%f w:%f\n", trans[0][1], trans[1][1],trans[2][1],trans[3][1]);
//	printf("x:%f y:%f z:%f w:%f\n", trans[0][2], trans[1][2],trans[2][2],trans[3][2]);
//	printf("x:%f y:%f z:%f w:%f\n", trans[0][3], trans[1][3],trans[2][3],trans[3][3]);
//	printf("\n");
//	vec4 vector2 = {};
//	mat4x4_mul_vec4(vector2, trans, vector);
//	printf("vec x:%f y:%f z:%f w:%f\n", vector2[0], vector2[1],vector2[2],vector2[3]);
	//printf("%f\n", M_PI);
	//printf("x:%f y:%f z:%f w:%f\n", trans[0][0], trans[1][0],trans[2][0],trans[3][0]);
	//printf("x:%f y:%f z:%f w:%f\n", trans[0][1], trans[1][1],trans[2][1],trans[3][1]);
	//printf("x:%f y:%f z:%f w:%f\n", trans[0][2], trans[1][2],trans[2][2],trans[3][2]);
	//printf("x:%f y:%f z:%f w:%f\n", trans[0][3], trans[1][3],trans[2][3],trans[3][3]);

	//-------------Data----------------//

	//GLfloat vertices[] = {
	//	// Positions 		  //Colors
	//	 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f, // Bottom Right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, // Bottom Left
	//	 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f, // Top
	//};

	//GLfloat texCoords[] = {
	//0.0f, 0.0f,  // Lower-left corner
	//1.0f, 0.0f,  // Lower-right corner
	//0.5f, 1.0f   // Top-center corner
	//};

	//GLfloat boxVertices[] = {
	//	 0.5f,  0.5f, 0.0f,  // Top Right
	//	 0.5f, -0.5f, 0.0f,  // Bottom Right
	//	-0.5f, -0.5f, 0.0f,  // Bottom Left
	//	-0.5f,  0.5f, 0.0f   // Top Left
	//};

	//1.0f, 0.0f, 0.0f,
	//0.0f, 1.0f, 0.0f,
	//0.0f, 0.0f, 1.0f,
	//1.0f, 1.0f, 0.0f,

	//Vertex vertices[] = {
	//	// positions            // texture coords
	//	 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
	//	 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
	//	-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
	//};


	//GLuint indices[] = {  // Note that we start from 0!
	//	0, 1, 3,   // First Triangle
	//	1, 2, 3    // Second Triangle
	//};

	Vertex vertices[] = {
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},
		{{ 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}},
		{{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}},
		{{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}},
		{{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}},
		{{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}
	};

	vec3 cubePositions[10] = {
		 0.0f,  0.0f,  0.0f,
		 2.0f,  5.0f, -15.0f,
		-1.5f, -2.2f, -2.5f,
		-3.8f, -2.0f, -12.3f,
		 2.4f, -0.4f, -3.5f,
		-1.7f,  3.0f, -7.5f,
		 1.3f, -2.0f, -2.5f,
		 1.5f,  2.0f, -2.5f,
		 1.5f,  0.2f, -1.5f,
		-1.3f,  1.0f, -1.5f,
	};

	//-------------Buffer Creation----------------//

	//glPolygonMode(GL_BACK, GL_LINE);

	unsigned int vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3* sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(GLfloat)));
		//glEnableVertexAttribArray(2);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
	unsigned int textureContainer 	= makeTexture("../data/container.jpg", GL_RGB);
	unsigned int textureSmile 		= makeTexture("../data/awesomeface.png", GL_RGBA);

	//-------------loop----------------//
	GLint loc1 = glGetUniformLocation(shader.program, "textureContainer");
	GLint loc2 = glGetUniformLocation(shader.program, "textureSmile");
	GLint alphaLoc = glGetUniformLocation(shader.program, "inalpha");

	while(!glfwWindowShouldClose(window)) {
		updateFpsCounter(window);
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		// ----- Clear ------ //
		glClearColor(0.010f, 0.01f, 0.01f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);

		glUseProgram(shader.program);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureContainer);
		glUniform1i(loc1, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureSmile);
		glUniform1i(loc2, 1);

		glUniform1f(alphaLoc, mixValue);


		mat4x4 view = {};
		mat4x4_identity(view);

		vec3 direction = {};
		vec3_add(direction,camera.Position, camera.Front);
		mat4x4_look_at(view, camera.Position, direction ,camera.Up);

		mat4x4 projection = {};
		mat4x4_identity(projection);
		mat4x4_perspective(projection,deg2rad(camera.Zoom), SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

		glUniformMatrix4fv(shader.viewLocation, 		1, GL_FALSE, *view);
		glUniformMatrix4fv(shader.projectionLocation, 	1, GL_FALSE, *projection);

		glBindVertexArray(vao);
		for (int i = 0; i < 10; i++)
		{
			mat4x4 model = {};
			mat4x4_identity(model);
			mat4x4_translate(model,cubePositions[i][0],cubePositions[i][1],cubePositions[i][2]);
			GLfloat angle = 10.0f * i * glfwGetTime();
			mat4x4_rotate(model, model, 1.0f, 0.3f, 0.5f, deg2rad(angle));

			glUniformMatrix4fv(shader.modelLocation, 		1, GL_FALSE, *model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	//-------------Deleting----------------//
	printf("Deleting Data\n");
	glDeleteVertexArrays(1, &vao);
}

void closing_game()
{
	printf("closing!\n");
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ProcessMouseScroll(&camera, yoffset);
}

void mouseCallback(GLFWwindow* window, double positionX, double positionY)
{
	if (firstMouse)
	{
		lastX = positionX;
		lastY = positionY;
		firstMouse = false;
	}

	GLfloat xoffset = positionX - lastX;
	GLfloat yoffset = lastY - positionY;
	lastX = positionX;
	lastY = positionY;
	ProcessMouseMovement(&camera, xoffset, yoffset, true);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	else if(action == GLFW_RELEASE)
	{
		keys[key] = false;
	}
	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		mixValue += 0.1f;
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		mixValue -= 0.1f;
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
}

void error_callback(int error, const char* description)
{
	gl_log_err("GLFW ERROR: code %i msg: %s\n", error, description);
}


void window_close_callback(GLFWwindow* window)
{
	//if (!time_to_close)
	//    glfwSetWindowShouldClose(window, GLFW_FALSE);

	printf("trying to close window \n");
}

int restart_gl_log() {
  FILE* file = fopen(GL_LOG_FILE, "w");
  if(!file) {
	fprintf(stderr,
	  "ERROR: could not open GL_LOG_FILE log file %s for writing\n",
	  GL_LOG_FILE);
	return 0;
  }
  time_t now = time(NULL);
  char* date = ctime(&now);
  fprintf(file, "GL_LOG_FILE log. local time %s\n", date);
  fclose(file);
  return 1;
}

int gl_log(const char* message, ...) {
  va_list argptr;
  FILE* file = fopen(GL_LOG_FILE, "a");
  if(!file) {
	fprintf(
	  stderr,
	  "ERROR: could not open GL_LOG_FILE %s file for appending\n",
	  GL_LOG_FILE
	);
	return 0;
  }
  va_start(argptr, message);
  vfprintf(file, message, argptr);
  va_end(argptr);
  fclose(file);
  return 1;
}

int gl_log_err(const char* message, ...) {
  va_list argptr;
  FILE* file = fopen(GL_LOG_FILE, "a");
  if(!file) {
	fprintf(stderr,
	  "ERROR: could not open GL_LOG_FILE %s file for appending\n",
	  GL_LOG_FILE);
	return 0;
  }
  va_start(argptr, message);
  vfprintf(file, message, argptr);
  va_end(argptr);
  va_start(argptr, message);
  vfprintf(stderr, message, argptr);
  va_end(argptr);
  fclose(file);
  return 1;
}

