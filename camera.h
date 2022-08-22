#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum camera_movement
{
    forward,
    backward,
    left,
    right
};

// Default camera values
constexpr float default_yaw = -90.0f;
constexpr float default_pitch = 0.0f;
constexpr float default_speed = 2.5f;
constexpr float default_sensitivity = 0.1f;
constexpr float default_zoom = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class camera
{
public:
    // camera Attributes
    glm::vec3 position{};
    glm::vec3 direction_front;
    glm::vec3 direction_up{};
    glm::vec3 direction_right{};
    glm::vec3 world_up{};
    // euler Angles
    float yaw{};
    float pitch{};
    // camera options
    float movement_speed;
    float mouse_sensitivity;
    float zoom;

    // constructor with vectors
    explicit camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = default_yaw, float pitch = default_pitch) :
        position(position),
        direction_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        world_up(up),
        yaw(yaw),
        pitch(pitch),
        movement_speed(default_speed),
        mouse_sensitivity(default_sensitivity),
        zoom(default_zoom)
    {
        update_camera_vectors();
    }

    // constructor with scalar values
    camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch) :
        position(glm::vec3(pos_x, pos_y, pos_z)),
        direction_front(glm::vec3(0.0f, 0.0f, -1.0f)),
        world_up(glm::vec3(up_x, up_y, up_z)),
        yaw(yaw),
        pitch(pitch),
        movement_speed(default_speed),
        mouse_sensitivity(default_sensitivity),
        zoom(default_zoom)
    {
        update_camera_vectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 get_view_matrix() const
    {
        return lookAt(position, position + direction_front, direction_up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void process_keyboard(const camera_movement direction, const float delta_time)
    {
        const float velocity = movement_speed * delta_time;
        switch (direction)
        {
        case forward:
            position += direction_front * velocity;
            break;
        case backward:
            position -= direction_front * velocity;
            break;
        case left:
            position -= direction_right * velocity;
            break;
        case right:
            position += direction_right * velocity;
            break;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_movement(float offset_x, float offset_y, const GLboolean constrain_pitch = true)
    {
        offset_x *= mouse_sensitivity;
        offset_y *= mouse_sensitivity;

        yaw += offset_x;
        pitch += offset_y;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrain_pitch)
        {
            constexpr float pitch_limit = 89.0f;
            if (pitch > pitch_limit)
                pitch = pitch_limit;
            if (pitch < -pitch_limit)
                pitch = -pitch_limit;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        update_camera_vectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void process_mouse_scroll(const float offset_y)
    {
        zoom -= offset_y;

        constexpr float min_zoom = 1.0f;
        if (zoom < min_zoom)
            zoom = min_zoom;

        constexpr float max_zoom = 45.0f;
        if (zoom > max_zoom)
            zoom = max_zoom;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction_front = normalize(front);
        // also re-calculate the Right and Up vector
        direction_right = normalize(cross(front, world_up));
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        direction_up = normalize(cross(direction_right, front));
    }
};
