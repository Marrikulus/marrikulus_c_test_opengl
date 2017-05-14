
const GLfloat YAW 			= -90.0f;
const GLfloat PITCH 		= 0.0f;
const GLfloat SPEED 		= 0.0f;
const GLfloat SENSITIVITY 	= 0.0f;
const GLfloat ZOOM 			= 0.0f;

typedef enum
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
}
CameraMovement;

typedef struct
{
	vec3 Position;
	vec3 Front;
	vec3 Up;
	vec3 Right;
	vec3 WorldUp;

	GLfloat Yaw;
	GLfloat Pitch;
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;
} Camera;

bool firstMouse = 1;
GLfloat lastX = SCREEN_WIDTH  / 2.0f;
GLfloat lastY = SCREEN_HEIGHT / 2.0f;

Camera initCamera()
{
	return (Camera){
		.Position 			= { 0.0f, 0.0f, 3.0f },
		.Front 				= { 0.0f, 0.0f,-1.0f },
		.Up 				= { 0.0f, 1.0f, 0.0f },
		.Right 				= {-1.0f, 0.0f, 0.0f },
		.WorldUp 			= { 0.0f, 1.0f, 0.0f },
		.Yaw 				= YAW,
		.Pitch 				= PITCH,
		.MovementSpeed 		= SPEED,
		.MouseSensitivity 	= SENSITIVITY,
		.Zoom 				= ZOOM
	};
}

void updateCameraVectors(Camera *camera)
{
	vec3 front = {
		cos(deg2rad(camera->Yaw)) * cos(deg2rad(camera->Pitch)),
		sin(deg2rad(camera->Pitch)),
		sin(deg2rad(camera->Yaw)) * cos(deg2rad(camera->Pitch))
	};
	vec3_norm(camera->Front, front);

	vec3 CameraRight = {};
	vec3_mul_cross(CameraRight,camera->Front, camera->WorldUp);
	vec3_norm(camera->Right, CameraRight);

	vec3 CameraUp = {};
	vec3_mul_cross(CameraUp,camera->Right, camera->Front);
	vec3_norm(camera->Right, CameraUp);
}

void ProcessMouseMovement(Camera *camera, GLfloat xoffset, GLfloat yoffset, GLboolean contrainPitch)
{
	GLfloat sensitivity = 0.03f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera->Pitch += yoffset;
	camera->Yaw   += xoffset;

	if (contrainPitch)
	{
		if (camera->Pitch > 89.0f)
		{
			camera->Pitch = 89.0f;
		}
		if (camera->Pitch < -89.0f)
		{
			camera->Pitch = -89.0f;
		}
	}
	updateCameraVectors(camera);
}


void ProcessMouseScroll(Camera *camera, GLfloat yoffset)
{
	if (camera->Zoom >= 1.0f && camera->Zoom <= 45.0f)
	{
		camera->Zoom -=yoffset;
	}
	if (camera->Zoom <= 1.0f)
	{
		camera->Zoom = 1.0f;
	}
	if (camera->Zoom >= 45.0f)
	{
		camera->Zoom = 45.0f;
	}
}


