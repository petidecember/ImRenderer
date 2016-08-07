//womble is a faggot
#include "ImRenderer.h"
#include <IMGUI\imgui.h>
#include <GLFW\glfw3.h>
#include <iostream>

static float mouseX = 0.0;
static float mouseY = 0.0;

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = static_cast<float>(xpos);
	mouseY = static_cast<float>(ypos);
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

	if (!gladLoadGL())
	{
		glfwTerminate();
	}

	return window;
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

	glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, -20.0f);

	#define QUALITY 16
	float vertices[12 * QUALITY];
	unsigned int triangles[6 * QUALITY];

	float angleStart = -45.0f;
	float angleEnd   = -angleStart;
	float angleDelta = (angleEnd - angleStart) / QUALITY;

	float angleCurrent = angleStart;
	float angleNext    = angleStart + angleDelta;

	glm::vec3 pos_curr_min = glm::vec3();
	glm::vec3 pos_curr_max = glm::vec3();

	glm::vec3 pos_next_min = glm::vec3();
	glm::vec3 pos_next_max = glm::vec3();


	for (int i = 0; i < QUALITY; i++)
	{
        glm::vec3 sphere_current = glm::vec3(std::sin(glm::radians(angleCurrent)),
                                             std::cos(glm::radians(angleCurrent)), 0);

        glm::vec3 sphere_next    = glm::vec3(std::sin(glm::radians(angleNext)),
                                             std::cos(glm::radians(angleNext)), 0);

        pos_curr_min = sphere_current * 4.0f;
        pos_curr_max = sphere_current * 8.0f;

        pos_next_min = sphere_next * 4.0f;
        pos_next_max = sphere_next * 8.0f;

        int a = 4 * i;
        int b = 4 * i + 1;
        int c = 4 * i + 2;
        int d = 4 * i + 3;

        vertices[12 * i + 0] = pos_curr_min.x;
        vertices[12 * i + 1] = pos_curr_min.y;
        vertices[12 * i + 2] = pos_curr_min.z;
        vertices[12 * i + 3] = pos_curr_max.x;
        vertices[12 * i + 4] = pos_curr_max.y;
        vertices[12 * i + 5] = pos_curr_max.z;
        vertices[12 * i + 6] = pos_next_max.x;
        vertices[12 * i + 7] = pos_next_max.y;
        vertices[12 * i + 8] = pos_next_max.z;
        vertices[12 * i + 9] = pos_next_min.x;
        vertices[12 * i + 10] = pos_next_min.y;
        vertices[12 * i + 11] = pos_next_min.z;

        triangles[6 * i] = a;
        triangles[6 * i + 1] = b;
        triangles[6 * i + 2] = c;
        triangles[6 * i + 3] = c;
        triangles[6 * i + 4] = d;
        triangles[6 * i + 5] = a;

        angleCurrent += angleDelta;
        angleNext += angleDelta;
	}

	int shapeID = IR.addShape(vertices, triangles, 12 * QUALITY, 6 * QUALITY);

	while (!shouldClose)
	{
		shouldClose = glfwWindowShouldClose(window);
		glfwPollEvents();

		IR.clear(GL_COLOR_BUFFER_BIT);
		IR.background(0);
		IR.setCamera(cameraPosition, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		IR.drawShapeElements(shapeID, 6 * QUALITY, ImRenderer::DrawType::TRIANGLES);

		IR.quad();

		glfwSwapBuffers(window);
	}
	IR.cleanUp();
	glfwTerminate();
	return 0;
}
