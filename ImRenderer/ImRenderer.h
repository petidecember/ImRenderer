#pragma once
#include <GL\glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

class ImRenderer {
public:
	enum DrawType {
		TRIANGLES = 0x0004,
		TRIANGLE_STRIP = 0x0005,
		QUADS = 0x0007
	};

	struct Uniform {
		GLchar* name;
		GLsizei count;
		GLfloat** variable;
	};
private:
	//Window Related Variables
	static GLint width, height;
	static GLfloat aspect;
	//END WINDOW

	//Shader
	static GLuint program;
	static GLuint currentProgram;
	//END SHADER

	//Shape related variables, hard coded
	static GLuint pointVAO;
	static GLuint pointVBO;
	static GLuint rectVAO;
	static GLuint rectVBO;
	static GLuint triangleVAO;
	static GLuint triangleVBO;

	static glm::vec3 color;
	//END SHAPE

	//Shader, VAO & VBO ID access for custom shapes
	static std::vector<GLuint> accessVAO;
	static std::vector<GLuint> accessVBO;
	static std::vector<GLuint> accessProgram;
	static std::vector<std::pair<GLuint, Uniform>> accessUniforms;
	//END CUSTOM SHAPE

	//Standard matrix variables
	static glm::mat4 modelMatrix;
	static glm::mat4 viewMatrix;
	static glm::mat4 projectionMatrix;
	//END MVP

	enum Primitive {
		POINT,
		TRIANGLE,
		QUAD
	};

public:
	static void init(int width, int height, glm::mat4 projection);
private:
	static void initShaders();
	static void initShapes();

	static void drawPrimitive(Primitive primitive);
public:

	static void clear(GLbitfield bitfield);
	
	/*NOTE: This function doesn't take in a byte value*/
	static void background(int color);
	static void background(int r, int g, int b, int a);
	static void background(int r, int g, int b);

	static int initShader(const char* fSource);
	static int initShape(const float* vertices);
	static void drawShape(int id, int count, DrawType type);

	static void setColor(float r, float g, float b);
	static void setShader(unsigned int id);
	static void setUniform(unsigned int id, Uniform uniform);
	static void restoreShader() { currentProgram = program; }
	static void point();
	static void triangle(float width, float height);
	static void quad();

	static void setCamera(glm::vec3 pos, glm::vec3 dir);
	static void setCamera(glm::vec3 pos, glm::vec3 dir, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

	static void translate(float x, float y, float z);
	static void rotate(glm::vec3 axis, float radians);
	static void scale(float x, float y, float z);

	static void cleanUp();

	static void pointSize(GLfloat size);

	static glm::mat4 getModelMatrix() { return modelMatrix; }
	static glm::mat4 getViewMatrix() { return viewMatrix; }
	static glm::mat4 getProjectionMatrix() { return projectionMatrix; }
	static int getWidth() { return width; }
	static int getHeight() { return height; }
	static float getAspect() { return aspect; }
};