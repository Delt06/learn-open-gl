#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

constexpr int info_log_size = 512;
char info_log[info_log_size];
int success;

std::string read_all_lines(const std::string& path)
{
	const std::ifstream file(path);
	std::string result;

	if (file)
	{
		std::ostringstream output_stream;
		output_stream << file.rdbuf();
		result = output_stream.str();
	}

	return result;
}

void compile_shader(const unsigned int shader, const char* source)
{
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, info_log_size, nullptr, info_log);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << info_log << std::endl;
	}
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr int initial_width = 800;
	constexpr int initial_height = 600;

	GLFWwindow* window = glfwCreateWindow(initial_width, initial_height, "LearnOpenGL", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, initial_width, initial_height);

	constexpr float vertices[] = {
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.05f,
		-0.5f, 0.5f, 0.05f,
	};

	constexpr unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

	unsigned int vao; // vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo; // vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);

	unsigned int ebo; // element buffer object
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const std::string vertex_shader_source = read_all_lines("./shaders/vertex.glsl");
	const std::string fragment_shader_source = read_all_lines("./shaders/fragment.glsl");

	const unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	compile_shader(vertex_shader, vertex_shader_source.c_str());
	const unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	compile_shader(fragment_shader, fragment_shader_source.c_str());

	const unsigned int shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_program, info_log_size, nullptr, info_log);
		std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << info_log << std::endl;
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);


	while (!glfwWindowShouldClose(window))
	{
		// input
		process_input(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof indices / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
