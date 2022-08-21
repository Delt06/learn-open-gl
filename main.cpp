#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"

int window_width = 800;
int window_height = 600;

float delta_time = 0.0f;
double last_frame_time = 0.0;

float last_mouse_x, last_mouse_y;
bool mouse_callback_called = false;

camera scene_camera(glm::vec3(0.0f, 0.0f, 3.0f));

void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
	glViewport(0, 0, width, height);
	window_width = width;
	window_height = height;
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene_camera.process_keyboard(forward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene_camera.process_keyboard(backward, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene_camera.process_keyboard(left, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene_camera.process_keyboard(right, delta_time);
}

void mouse_callback(GLFWwindow* window, const double mouse_x, const double mouse_y)
{
	const auto mouse_x_f = static_cast<float>(mouse_x);
	const auto mouse_y_f = static_cast<float>(mouse_y);

	if (!mouse_callback_called)
	{
		last_mouse_x = mouse_x_f;
		last_mouse_x = mouse_y_f;
		mouse_callback_called = true;
	}

	const float offset_x = mouse_x_f - last_mouse_x;
	const float offset_y = -(mouse_y_f - last_mouse_y);
	last_mouse_x = mouse_x_f;
	last_mouse_y = mouse_y_f;

	scene_camera.process_mouse_movement(offset_x, offset_y);
}

void scroll_callback(GLFWwindow* window, double offset_x, const double offset_y)
{
	scene_camera.process_mouse_scroll(static_cast<float>(offset_y));
}

unsigned int load_texture(char const* path)
{
	unsigned int texture_id;
	glGenTextures(1, &texture_id);

	int width, height, nr_components;
	unsigned char* data = stbi_load(path, &width, &height, &nr_components, 0);
	if (data)
	{
		GLenum format = 0;
		if (nr_components == 1)
			format = GL_RED;
		else if (nr_components == 3)
			format = GL_RGB;
		else if (nr_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return texture_id;
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", nullptr, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, window_width, window_height);

	constexpr float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	constexpr unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3,
	};

	unsigned int cube_vao; // vertex array object
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	unsigned int vbo; // vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	constexpr size_t stride = 8 * sizeof(float);
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int light_vao;
	glGenVertexArrays(1, &light_vao);
	glBindVertexArray(light_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	const shader cube_shader("./shaders/shader.vert", "./shaders/shader.frag");
	const shader light_shader("./shaders/shader.vert", "./shaders/light_shader.frag");

	const unsigned int diffuse_map = load_texture("./images/container2.png");
	const unsigned int specular_map = load_texture("./images/container2_specular.png");

	constexpr glm::vec3 light_position(1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		const double current_frame_time = glfwGetTime();
		delta_time = static_cast<float>(current_frame_time - last_frame_time);
		last_frame_time = current_frame_time;

		// input
		process_input(window);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		cube_shader.use();
		cube_shader.set_vec3("viewPos", scene_camera.position);

		cube_shader.set_vec3("material.diffuse", 1.0f, 1.0f, 1.0f);
		cube_shader.set_int("material.diffuseMap", 0);
		cube_shader.set_vec3("material.specular", 1.0f, 1.0f, 1.0f);
		cube_shader.set_int("material.specularMap", 1);
		cube_shader.set_float("material.shininess", 32.0f);

		cube_shader.set_vec3("light.ambient", 0.2f, 0.2f, 0.2f);
		cube_shader.set_vec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		cube_shader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);
		cube_shader.set_vec3("light.position", light_position);


		const auto view = scene_camera.get_view_matrix();
		const auto projection = glm::perspective(glm::radians(scene_camera.zoom),
		                                         static_cast<float>(window_width) / static_cast<float>(window_height),
		                                         0.1f, 100.0f);

		cube_shader.set_mat4("view", view);
		cube_shader.set_mat4("projection", projection);
		auto model = glm::mat4(1.0f);
		model = rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0, 1, 0));
		cube_shader.set_mat4("model", model);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse_map);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular_map);
		glBindVertexArray(cube_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		light_shader.use();
		light_shader.set_mat4("view", view);
		light_shader.set_mat4("projection", projection);
		model = glm::mat4(1.0f);
		model = translate(model, light_position);
		model = scale(model, glm::vec3(0.2f));
		light_shader.set_mat4("model", model);
		glBindVertexArray(light_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
