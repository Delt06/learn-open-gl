#pragma once
#include <string>
#include <glad/glad.h>

class cubemap
{
public:
    static constexpr size_t sides = 6;

    explicit cubemap(const std::string texture_faces_paths[sides], bool flip_vertically = false);

    void bind() const;

private:
    GLuint texture_id_;
};
