#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr int initial_width = 800;
	constexpr int initial_height = 600;

	GLFWwindow* window = glfwCreateWindow(initial_width, initial_height, "LearnOpenGL", nullptr, NULL);
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
		// positions      // colors
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
	};

	constexpr unsigned int indices[] = {
		0, 1, 2,
	};

	unsigned int vao; // vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo; // vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	constexpr size_t stride = 6 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int ebo; // element buffer object
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	const shader shader("./shaders/shader.vert", "./shaders/shader.frag");


	while (!glfwWindowShouldClose(window))
	{
		// input
		process_input(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		const auto time_value = static_cast<float>(glfwGetTime());
		shader.set_float("time", time_value);

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
