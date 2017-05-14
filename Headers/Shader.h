#include <MarLib.h>

typedef struct {
	GLuint vertexShader, fragmentShader, program;
	GLint modelLocation, viewLocation, projectionLocation;
} ShaderProgram;

static void show_info_log(
	GLuint object,
	PFNGLGETSHADERIVPROC glGet__iv,
	PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
	GLint log_length;
	char *log;

	glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	log = malloc(log_length);
	glGet__InfoLog(object, log_length, NULL, log);
	fprintf(stderr, "%s", log);
	free(log);
}

ShaderProgram makeShaderProgram(char * fragmentShaderPath, char * vertexShaderPath);
GLuint makeShader(GLenum type, const char *code);
GLuint makeProgram(GLuint vertexShader, GLuint fragmentShader);

ShaderProgram makeShaderProgram(char * fragmentShaderPath, char * vertexShaderPath)
{
	char * fragmentShaderCode = ReadFile(fragmentShaderPath);
	char * vertexShaderCode = ReadFile(vertexShaderPath);

	GLuint vert 	= makeShader(GL_VERTEX_SHADER, vertexShaderCode);
	GLuint frag 	= makeShader(GL_FRAGMENT_SHADER, fragmentShaderCode);
	GLuint program 	= makeProgram(vert, frag);

	GLint modelLocation 		= glGetUniformLocation(program, "model");
	GLint viewLocation 			= glGetUniformLocation(program, "view");
	GLint projectionLocation 	= glGetUniformLocation(program, "projection");

	return (ShaderProgram){
		.vertexShader 		= vert,
		.fragmentShader 	= frag,
		.program 			= program,
		.modelLocation		= modelLocation,
		.viewLocation		= viewLocation,
		.projectionLocation	= projectionLocation
	};

}

GLuint makeShader(GLenum type, const char *code)
{
	GLuint shader;
	GLint shader_ok;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
	if (!shader_ok)
	{
		show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}


GLuint makeProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLint programOk;

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &programOk);
	if (!programOk)
	{
		show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
		glDeleteProgram(program);
		return 0;
	}
	return program;
}