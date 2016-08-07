#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "glad/glad.h"

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
	GLint width, height;
	GLfloat aspect;
	//END WINDOW

	//Shader
	GLuint defaultProgram;
	GLuint currentProgram;
	//END SHADER

	//Shape related variables, hard coded
	GLuint pointVAO;
	GLuint pointVBO;
	GLuint rectVAO;
	GLuint rectVBO;
	GLuint rectEBO;
	GLuint triangleVAO;
	GLuint triangleVBO;

	glm::vec3 color;
	//END SHAPE

	//Shader, VAO & VBO ID access for custom shapes
	std::vector<GLuint> accessVAO;
	std::vector<GLuint> accessVBO;
	std::vector<GLuint> accessEBO;
	std::vector<GLuint> accessProgram;
	std::vector<std::pair<GLuint, Uniform>> accessUniforms;
	//END CUSTOM SHAPE

	//Standard matrix variables
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	//END MVP

	enum Primitive {
		POINT,
		TRIANGLE,
		QUAD
	};

public:
	ImRenderer(int width, int height, const glm::mat4& projection);
	~ImRenderer();
private:
	void initDefShaders();
	void initDefShapes();

	void drawPrimitive(Primitive primitive);
public:

	void clear(GLbitfield bitfield);

	/*NOTE: This function doesn't take in a byte value*/
	void background(int color);
	void background(int r, int g, int b, int a);
	void background(int r, int g, int b);

	int addShader(const char* fSource);
	int addShape(const float* vertices, const unsigned int* indices, size_t verticesSize, size_t indicesSize);
	void drawShape(int id, int count, DrawType type);
	void drawShapeElements(int id, int count, DrawType type);

	void setColor(float r, float g, float b);
	void setShader(unsigned int id);
	void setUniform(unsigned int id, Uniform uniform);
	void restoreShader() { currentProgram = defaultProgram; }

	//Primitive Draw Functions
	void point();
	void pointSize(GLfloat size);

	void triangle(float width, float height);

	void quad();
	//END PRIMITIVE DRAW

	void setCamera(const glm::vec3& pos, const glm::vec3& dir);
	void setCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

	void translate(float x, float y, float z);
	void rotate(const glm::vec3& axis, float radians);
	void scale(float x, float y, float z);

	void cleanUp();


	//Utility Functions
	glm::mat4 getModelMatrix() { return modelMatrix; }
	glm::mat4 getViewMatrix() { return viewMatrix; }
	glm::mat4 getProjectionMatrix() { return projectionMatrix; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	float getAspect() { return aspect; }
};
