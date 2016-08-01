#include "ImRenderer.h"

ImRenderer::ImRenderer(int _width, int _height, const glm::mat4& projection)
{
	width = _width;
	height = _height;
	aspect = (float)width / (float)height;
	projectionMatrix = projection;

	defaultProgram = 0;
	currentProgram = 0;

	pointVAO = 0;
	pointVBO = 0;
	rectVAO = 0;
	rectVBO = 0;
	triangleVAO = 0;
	triangleVBO = 0;

	color = glm::vec3(255);

	accessVAO = std::vector<GLuint>();
	accessVBO = std::vector<GLuint>();
	accessProgram = std::vector<GLuint>();
	accessUniforms = std::vector<std::pair<GLuint, Uniform>>();

	modelMatrix = glm::mat4();
	viewMatrix = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	initDefShaders();
	initDefShapes();
}

ImRenderer::~ImRenderer()
{
	cleanUp();
}

void ImRenderer::initDefShaders()
{
	GLuint vertex = 0;
	GLuint fragment = 0;
	GLint success = 0;
	GLchar infoLog[512];
	//TODO: Put these in files.
	//TODO: Write a file reader/loader utility class
	const GLchar* vSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(position, 1.0);\n"
		"}\0";
	const GLchar* fSource = "#version 330 core\n"
		"uniform vec3 c;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(c, 1.0f);\n"
		"}\n\0";

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, nullptr);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR: Shader Vertex COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, nullptr);
	glCompileShader(fragment);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR: Shader Fragment COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	defaultProgram = glCreateProgram();
	glAttachShader(defaultProgram, vertex);
	glAttachShader(defaultProgram, fragment);
	glLinkProgram(defaultProgram);

	glGetProgramiv(defaultProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(defaultProgram, 512, NULL, infoLog);
		std::cout << "ERROR: Shader Program LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	currentProgram = defaultProgram;
}

void ImRenderer::initDefShapes()
{
	float PointVertices[3] = { 0.0f, 0.0f, 0.0f };

	glGenVertexArrays(1, &pointVAO);
	glBindVertexArray(pointVAO);
	glGenBuffers(1, &pointVBO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PointVertices), PointVertices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	float RectVertices[18] = {
		// First triangle
		0.5f,  0.5f, 0.0f,  // Top Right
		0.5f, -0.5f, 0.0f,  // Bottom Right
		-0.5f,  0.5f, 0.0f,  // Top Left 
							 // Second triangle
							 0.5f, -0.5f, 0.0f,  // Bottom Right
							 -0.5f, -0.5f, 0.0f,  // Bottom Left
							 -0.5f,  0.5f, 0.0f   // Top Left
	};

	glGenVertexArrays(1, &rectVAO);
	glBindVertexArray(rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RectVertices), RectVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);

	float TriangleVertices[9] = {
		// First triangle
		0.5f, -0.5f, 0.0f,  // Bottom Left
		-0.5f, -0.5f, 0.0f,  // Bottom Right
		0.0f,  0.5f, 0.0f,  // Top 
	};

	glGenVertexArrays(1, &triangleVAO);
	glBindVertexArray(triangleVAO);
	glGenBuffers(1, &triangleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
}

void ImRenderer::clear(GLbitfield bitfield)
{
	glClear(bitfield);
}

void ImRenderer::background(int r, int g, int b, int a)
{
	glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

void ImRenderer::background(int r, int g, int b)
{
	glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
}

void ImRenderer::background(int color)
{
	GLclampf c = color / 255.0f;
	glClearColor(c, c, c, 1.0f);
}

int ImRenderer::addShader(const char* fSource)
{
	GLuint vertex = 0;
	GLuint fragment = 0;
	GLuint program = 0;
	GLint success = 0;
	GLchar infoLog[512];
	//TODO: Put these in files.
	//TODO: Write a file reader/loader utility class
	const GLchar* vSource = "#version 330 core\n"
		"layout (location = 0) in vec3 position;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(position, 1.0);\n"
		"}\0";

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, nullptr);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR: Shader Vertex COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, nullptr);
	glCompileShader(fragment);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR: Shader Fragment COMPILATION_FAILED\n" << infoLog << std::endl;
	};


	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR: Shader Program LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	accessProgram.push_back(program);

	return program;
}

int ImRenderer::addShape(const float* vertices)
{
	unsigned int id;
	glGenVertexArrays(1, &id);
	accessVAO.push_back(id);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	accessVBO.push_back(vbo);

	glBindVertexArray(id);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(0);

	return id;
}

void ImRenderer::drawShape(int id, int count, DrawType type)
{
	glBindVertexArray(id);

	glUseProgram(currentProgram);
	glUniformMatrix4fv(glGetUniformLocation(currentProgram, "model"), 1, false, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(currentProgram, "view"), 1, false, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(currentProgram, "projection"), 1, false, glm::value_ptr(projectionMatrix));
	glUniform3f(glGetUniformLocation(currentProgram, "c"), color.r, color.g, color.b);

	glDrawArrays(type, 0, count);

	glBindVertexArray(0);

	modelMatrix = glm::mat4();
	color = glm::vec3(1);
	currentProgram = defaultProgram;
}

void ImRenderer::setColor(float r, float g, float b)
{
	color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

void ImRenderer::setShader(unsigned int id)
{
	currentProgram = id;
}

void ImRenderer::setUniform(unsigned int id, Uniform uniform)
{
	accessUniforms.emplace_back(id, uniform);
}

void ImRenderer::drawPrimitive(Primitive primitive)
{
	glUseProgram(currentProgram);
	glUniformMatrix4fv(glGetUniformLocation(currentProgram, "model"), 1, false, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(currentProgram, "view"), 1, false, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(currentProgram, "projection"), 1, false, glm::value_ptr(projectionMatrix));
	glUniform3f(glGetUniformLocation(currentProgram, "c"), color.r, color.g, color.b);

	if (currentProgram != defaultProgram)
	{
		std::vector<std::pair<GLuint, Uniform>>::iterator it = std::find_if(accessUniforms.begin(), accessUniforms.end(), [&](const std::pair<GLuint, Uniform>& pair) -> bool { return pair.first == currentProgram; });
		Uniform* uniform = &it->second;

		switch (uniform->count)
		{
		case 1:
			glUniform1fv(glGetUniformLocation(currentProgram, uniform->name), 1, uniform->variable[0]);
			break;
		case 2:
			glUniform2fv(glGetUniformLocation(currentProgram, uniform->name), 1, uniform->variable[0]);
			break;
		case 3:
			glUniform3fv(glGetUniformLocation(currentProgram, uniform->name), 1, uniform->variable[0]);
			break;
		case 4:
			glUniform4fv(glGetUniformLocation(currentProgram, uniform->name), 1, uniform->variable[0]);
			break;
		case 16:
			glUniformMatrix4fv(glGetUniformLocation(currentProgram, uniform->name), 1, false, uniform->variable[0]);
			break;
		}
	}

	switch (primitive)
	{
	case POINT:
		glBindVertexArray(pointVAO);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
		break;

	case TRIANGLE:
		glBindVertexArray(triangleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		break;
	
	case QUAD:
		glBindVertexArray(rectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		break;
	}

	modelMatrix = glm::mat4();
	color = glm::vec3(1);
	currentProgram = defaultProgram;
}

void ImRenderer::point()
{
	drawPrimitive(Primitive::POINT);
}

void ImRenderer::triangle(float width, float height)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, 1.0f));
	drawPrimitive(Primitive::TRIANGLE);
}

void ImRenderer::quad()
{
	drawPrimitive(Primitive::QUAD);
}

//TODO: take in a camera class later
void ImRenderer::setCamera(const glm::vec3& pos, const glm::vec3& dir)
{
	viewMatrix = glm::lookAt(pos, pos + dir, glm::vec3(0.0f, 1.0f, 0.0f));
}

void ImRenderer::setCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up)
{
	viewMatrix = glm::lookAt(pos, pos + dir, up);
}

void ImRenderer::translate(float x, float y, float z)
{
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
}

void ImRenderer::rotate(const glm::vec3& axis, float radians)
{
	modelMatrix = glm::rotate(modelMatrix, radians, axis);
}

void ImRenderer::scale(float x, float y, float z)
{
	modelMatrix = glm::scale(modelMatrix, glm::vec3(x, y, z));
}

void ImRenderer::pointSize(GLfloat size)
{
	glPointSize(size);
}

void ImRenderer::cleanUp()
{
	glDeleteProgram(defaultProgram);
	if (accessProgram.size() != 0) { glDeleteProgramsARB(accessProgram.size(), &accessProgram[0]); }
	glDeleteBuffers(1, &pointVBO);
	glDeleteBuffers(1, &rectVBO);
	glDeleteBuffers(1, &triangleVBO);
	if (accessVBO.size() != 0) { glDeleteBuffers(accessVBO.size(), &accessVBO[0]); }
	glDeleteVertexArrays(1, &pointVAO);
	glDeleteVertexArrays(1, &rectVAO);
	glDeleteVertexArrays(1, &triangleVAO);
	if (accessVAO.size() != 0) { glDeleteVertexArrays(accessVAO.size(), &accessVAO[0]); }
}