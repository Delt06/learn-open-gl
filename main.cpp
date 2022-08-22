#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"
#include <vector>

int window_width = 800;
int window_height = 600;

float delta_time = 0.0f;
double last_frame_time = 0.0;

float last_mouse_x, last_mouse_y;
bool mouse_callback_called = false;

camera scene_camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool use_flashlight = true;
bool flashlight_pressed = false;

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

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (!flashlight_pressed)
        {
            use_flashlight = !use_flashlight;
            flashlight_pressed = true;
        }
    }
    else
    {
        flashlight_pressed = false;
    }
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

unsigned int load_texture(const char* path)
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

    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "LearnOpenGL", nullptr, nullptr);
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
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
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

    const std::vector<glm::vec3> cube_positions{
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    const std::vector<glm::vec3> start_light_positions = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    while (!glfwWindowShouldClose(window))
    {
        const double current_frame_time = glfwGetTime();
        delta_time = static_cast<float>(current_frame_time - last_frame_time);
        last_frame_time = current_frame_time;

        // input
        process_input(window);

        // simulate

        std::vector<glm::vec3> light_positions;

        for (size_t i = 0; i < start_light_positions.size(); i++)
        {
            auto start_light_position = start_light_positions[i];
            const auto angular_speed = 120 + glm::sin(4 * static_cast<double>(i)) * 30;
            auto angle = static_cast<float>(glm::radians(current_frame_time * angular_speed));
            auto rotation = angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f));
            auto light_position = rotation * glm::vec3(0.0f, 0.0f, 1.0f) + start_light_position;
            light_positions.push_back(light_position);
        }

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

        cube_shader.set_vec3("light.direction", -0.2f, -1.0f, -0.3f);
        cube_shader.set_vec3("light.ambient", 0.2f, 0.2f, 0.2f);
        cube_shader.set_vec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        cube_shader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);

        for (size_t i = 0; i < light_positions.size(); i++)
        {
            auto prefix = "pointLights[" + std::to_string(i) + "].";
            cube_shader.set_vec3(prefix + "position", light_positions[i]);
            cube_shader.set_vec3(prefix + "diffuse", 0.5f, 0.5f, 0.5f);
            cube_shader.set_vec3(prefix + "specular", 1.0f, 1.0f, 1.0f);
            cube_shader.set_vec3(prefix + "attenuationCoefficients", 1.0f, 0.09f, 0.032f);
        }

        const float flashlight_intensity = use_flashlight ? 1.0f : 0.0f;
        cube_shader.set_vec3("spotLight.position", scene_camera.position);
        cube_shader.set_vec3("spotLight.direction", scene_camera.direction_front);
        cube_shader.set_vec2("spotLight.cutOff", glm::cos(glm::radians(10.0f)), glm::cos(glm::radians(12.5f)));
        cube_shader.set_vec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f) * flashlight_intensity);
        cube_shader.set_vec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity);

        const auto view = scene_camera.get_view_matrix();
        const auto projection = glm::perspective(glm::radians(scene_camera.zoom),
                                                 static_cast<float>(window_width) / static_cast<float>(window_height),
                                                 0.1f, 100.0f);

        cube_shader.set_mat4("view", view);
        cube_shader.set_mat4("projection", projection);
        glm::mat4 model;


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);
        glBindVertexArray(cube_vao);


        for (size_t i = 0; i < cube_positions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = translate(model, cube_positions[i]);

            const float angle = static_cast<float>(i) * 20.0f;
            model = rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cube_shader.set_mat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        light_shader.use();
        light_shader.set_mat4("projection", projection);
        light_shader.set_mat4("view", view);
        glBindVertexArray(light_vao);

        for (auto light_position : light_positions)
        {
            model = glm::mat4(1.0f);
            model = translate(model, light_position);
            model = scale(model, glm::vec3(0.2f));
            light_shader.set_mat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
