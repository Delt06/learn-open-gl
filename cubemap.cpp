#include "cubemap.h"

#include <iostream>

#include "stb_image.h"

cubemap::cubemap(const std::string texture_faces_paths[sides], const bool flip_vertically): texture_id_(0)
{
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

    int width, height, num_channels;

    stbi_set_flip_vertically_on_load(flip_vertically);

    for (unsigned int i = 0; i < sides; i++)
    {
        auto path = texture_faces_paths[i];
        const auto filename = path.c_str();
        const auto data = stbi_load(filename, &width, &height, &num_channels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " + path << std::endl;
        }

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void cubemap::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
}

GLuint cubemap::get_id() const
{
    return texture_id_;
}
