// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>

#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "camera.h"
#include <vector>

#include "cubemap.h"
#include "model.h"
#include "skybox.h"
#include "stb_image.h"

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

    glEnable(GL_CULL_FACE);

    glViewport(0, 0, window_width, window_height);

    const shader lit_shader("./shaders/shader.vert", "./shaders/shader.frag");
    const shader light_shader("./shaders/shader.vert", "./shaders/light_shader.frag");
    const shader grass_shader("./shaders/shader.vert", "./shaders/alpha_clip.frag");
    const shader transparent_shader("./shaders/shader.vert", "./shaders/unlit_alpha.frag");

    model backpack("./assets/backpack/backpack.obj");
    model cube("./assets/cube.obj");

    std::string skybox_sides[] = {
        "./assets/skybox/right.jpg",
        "./assets/skybox/left.jpg",
        "./assets/skybox/top.jpg",
        "./assets/skybox/bottom.jpg",
        "./assets/skybox/front.jpg",
        "./assets/skybox/back.jpg",
    };
    cubemap skybox_cubemap(skybox_sides);
    const shader skybox_shader("./shaders/skybox.vert", "./shaders/skybox.frag");
    skybox scene_skybox(skybox_cubemap, skybox_shader);

    model_params grass_model_params;
    grass_model_params.texture_clamp = true;
    grass_model_params.texture_flip = false;
    model grass("./assets/grass/grass.obj", grass_model_params);

    model glass_box("./assets/glass_box/glass_box.obj");

    const std::vector<glm::vec3> model_positions{
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

    const std::vector<glm::vec3> vegetation = {
        (glm::vec3(-1.5f, 0.0f, -0.48f)),
        (glm::vec3(1.5f, 0.0f, 0.51f)),
        (glm::vec3(0.0f, 0.0f, 0.7f)),
        (glm::vec3(-0.3f, 0.0f, -2.3f)),
        (glm::vec3(0.5f, 0.0f, -0.6f)),
    };

    const std::vector<glm::vec3> glass_boxes
    {
        glm::vec3(0.0f, -0.48f, -1.5f),
        glm::vec3(1.5f, 0.51f, 0.0f),
        glm::vec3(0.0f, 0.7f, 0.0f),
        glm::vec3(-2.3f, 0.0f, -0.3f),
        glm::vec3(0.5f, -0.6f, 0.0f)
    };

    // create frame buffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create color attachment for the frame buffer
    unsigned int texture_color_buffer;
    glGenTextures(1, &texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach color to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);

    // create depth/stencil attachment for the frame buffer
    unsigned int depth_stencil_rbo;
    glGenRenderbuffers(1, &depth_stencil_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach depth/stencil to the frame buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rbo);

    // validate the frame buffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float blit_quad_vertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    unsigned int blit_quad_indices[] = {
        0, 1, 2, 3, 4, 5,
    };

    unsigned int blit_quad_vao;
    glGenVertexArrays(1, &blit_quad_vao);

    unsigned int blit_quad_vbo, blit_quad_ebo;
    glGenBuffers(1, &blit_quad_vbo);
    glGenBuffers(1, &blit_quad_ebo);

    glBindVertexArray(blit_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, blit_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof blit_quad_vertices), &blit_quad_vertices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, blit_quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof blit_quad_indices, &blit_quad_indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, reinterpret_cast<void*>(sizeof(float) * 2));

    glBindVertexArray(0);

    shader post_fx_shader("./shaders/blit.vert", "./shaders/postfx.frag");

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
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glCullFace(GL_BACK);

        const auto view = scene_camera.get_view_matrix();
        const auto projection = glm::perspective(glm::radians(scene_camera.zoom),
                                                 static_cast<float>(window_width) / static_cast<float>(window_height),
                                                 0.1f, 100.0f);

        // opaque pass
        glBlendFunc(GL_ONE, GL_ZERO);

        lit_shader.use();

        lit_shader.set_vec3("viewPos", scene_camera.position);

        lit_shader.set_vec3("material.diffuse", 1.0f, 1.0f, 1.0f);
        lit_shader.set_vec3("material.specular", 1.0f, 1.0f, 1.0f);
        lit_shader.set_float("material.shininess", 32.0f);
        lit_shader.set_float("material.reflectivity", 0.5f);

        lit_shader.set_vec3("light.direction", -0.2f, -1.0f, -0.3f);
        lit_shader.set_vec3("light.ambient", 0.2f, 0.2f, 0.2f);
        lit_shader.set_vec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        lit_shader.set_vec3("light.specular", 1.0f, 1.0f, 1.0f);


        for (size_t i = 0; i < light_positions.size(); i++)
        {
            auto prefix = "pointLights[" + std::to_string(i) + "].";
            lit_shader.set_vec3(prefix + "position", light_positions[i]);
            lit_shader.set_vec3(prefix + "diffuse", 0.5f, 0.5f, 0.5f);
            lit_shader.set_vec3(prefix + "specular", 1.0f, 1.0f, 1.0f);
            lit_shader.set_vec3(prefix + "attenuationCoefficients", 1.0f, 0.09f, 0.032f);
        }

        const float flashlight_intensity = use_flashlight ? 1.0f : 0.0f;
        lit_shader.set_vec3("spotLight.position", scene_camera.position);
        lit_shader.set_vec3("spotLight.direction", scene_camera.direction_front);
        lit_shader.set_vec2("spotLight.cutOff", glm::cos(glm::radians(10.0f)), glm::cos(glm::radians(12.5f)));
        lit_shader.set_vec3("spotLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f) * flashlight_intensity);
        lit_shader.set_vec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f) * flashlight_intensity);

        std::vector<extra_texture> extra_textures;
        extra_textures.push_back(extra_texture{skybox_cubemap.get_id(), "skybox", GL_TEXTURE_CUBE_MAP});

        lit_shader.set_mat4("view", view);
        lit_shader.set_mat4("projection", projection);
        glm::mat4 model;


        for (size_t i = 0; i < model_positions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = translate(model, model_positions[i]);

            const float angle = static_cast<float>(i) * 20.0f;
            model = rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            model = scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            lit_shader.set_mat4("model", model);

            backpack.draw(lit_shader, extra_textures);
        }

        light_shader.use();
        light_shader.set_mat4("projection", projection);
        light_shader.set_mat4("view", view);

        for (auto light_position : light_positions)
        {
            model = glm::mat4(1.0f);
            model = translate(model, light_position);
            model = scale(model, glm::vec3(0.2f));
            light_shader.set_mat4("model", model);
            cube.draw(light_shader);
        }

        grass_shader.use();
        grass_shader.set_mat4("projection", projection);
        grass_shader.set_mat4("view", view);
        grass_shader.set_float("material.alphaClipThreshold", 0.01f);

        for (auto grass_position : vegetation)
        {
            model = glm::mat4(1.0f);
            model = translate(model, grass_position);
            grass_shader.set_mat4("model", model);
            grass.draw(grass_shader);
        }

        // skybox
        scene_skybox.draw(view, projection);

        // transparent pass
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        transparent_shader.use();
        transparent_shader.set_mat4("projection", projection);
        transparent_shader.set_mat4("view", view);

        // sort geometry
        std::map<float, glm::vec3> sorted_glass_boxes;
        for (auto glass_box_position : glass_boxes)
        {
            float distance = length(scene_camera.position - glass_box_position);
            sorted_glass_boxes[distance] = glass_box_position;
        }

        for (auto it = sorted_glass_boxes.rbegin(); it != sorted_glass_boxes.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = translate(model, it->second);
            model = scale(model, glm::vec3(0.2f));
            transparent_shader.set_mat4("model", model);
            glass_box.draw(transparent_shader);
        }

        // post fx
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        post_fx_shader.use();
        post_fx_shader.set_float("intensity", 0.0f); // disable
        glBindVertexArray(blit_quad_vao);
        glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
        glDrawElements(GL_TRIANGLES, sizeof blit_quad_indices / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr);

        // check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
