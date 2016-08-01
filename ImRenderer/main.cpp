#include <IMGUI\imgui.h>
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "ImRenderer.h"

static float mouseX = 0.0;
static float mouseY = 0.0;

GLFWwindow* createWindow(int width, int height, const char* title);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
float lerp(float v0, float v1, float t);
glm::vec2 ScreenToWorld(const glm::vec2& point, float width, float height, const glm::vec3& camera, const glm::mat4& projection, const glm::mat4& view);

int main()
{
	int shouldClose = 0;
	int width = 800;
	int height = 600;
	float aspect = (float)width / (float)height;
	GLFWwindow* window = createWindow(width, height, "Immediate Renderer Test");
	glfwSwapInterval(1);

	ImRenderer IR = ImRenderer(width, height, glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f));

	glfwSetCursorPosCallback(window, cursor_position_callback);

	glm::vec2 mouse = glm::vec2();
	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -20.0f);
	glm::vec2 position = glm::vec2();

	const char* fSource = "#version 330 core\n"
		"uniform vec3 c;"
		"uniform vec2 position;"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"vec2 uv = -1.0 + 2.0 * gl_FragCoord.xy / vec2(800.0, 600.0);\n"
		"vec2 pos = -1.0 + 2.0 * position / vec2(800.0, 600.0);\n"
		"float p = length(uv + vec2(-pos.x, pos.y)) - 0.25;\n"
		"p = smoothstep(p, p+0.02, length(p));\n"
		"color = vec4(p, p, p, 1.0);\n"
		"}\n\0";

	int id = IR.addShader(fSource);
	GLfloat* uniform[2] = { &mouseX, &mouseY };
	ImRenderer::Uniform _uniform = {"position", 2, uniform };
	IR.setUniform(id, _uniform);

	while (!shouldClose)
	{
		shouldClose = glfwWindowShouldClose(window);
		glfwPollEvents();

		IR.clear(GL_COLOR_BUFFER_BIT);
		IR.background(0);
		IR.setCamera(cameraPosition, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		mouse = ScreenToWorld(glm::vec2(mouseX, mouseY), (float)width, (float)height, cameraPosition, IR.getProjectionMatrix(), IR.getViewMatrix());
		position = glm::vec2(lerp(position.x, mouse.x, 0.05f), lerp(position.y, mouse.y, 0.05f));

		IR.setColor(125, 255, 255);
		IR.setShader(id);
		IR.scale(aspect * (-cameraPosition.z * 2.0f), -cameraPosition.z * 2.0f, 1);
		IR.quad();

		IR.setColor(255, 0, 0);
		IR.translate(position.x, position.y, 0.0f);
		IR.rotate(glm::vec3(0.0f, 0.0f, 1.0f), glm::radians(-90.0f) + std::atan2(mouse.y - position.y, mouse.x - position.x));
		IR.triangle(4, 4);

		glfwSwapBuffers(window);
	}
	IR.cleanUp();
	glfwTerminate();
	return 0;
}

GLFWwindow* createWindow(int width, int height, const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(width, height, "Immediate Renderer Test", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
	}

	return window;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = static_cast<float>(xpos);
	mouseY = static_cast<float>(ypos);
}

float lerp(float v0, float v1, float t) {
	return (1 - t)*v0 + t*v1;
}

glm::vec2 ScreenToWorld(const glm::vec2& point, float width, float height, const glm::vec3& camera, const glm::mat4& projection, const glm::mat4& view)
{
	// normalize mouse position from window pixel space to between -1, 1
	GLfloat normMouse_x = (2.0f * point.x) / width - 1.0f;
	GLfloat normMouse_y = 1.0f - (2.0f * point.y) / height;

	glm::vec4 ray_clip = glm::vec4(glm::vec2(normMouse_x, normMouse_y), -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	glm::vec4 ray_world = glm::inverse(view) * ray_eye;

	ray_world = glm::normalize(ray_world);

	float l = -(camera.z / ray_world.z);

	return glm::vec2(camera.x + l * ray_world.x, camera.y + l * ray_world.y);
}