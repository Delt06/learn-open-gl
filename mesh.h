#pragma once

#include <string>
#include <vector>
#include <assimp/types.h>
#include <glm/glm.hpp>

#include "shader.h"

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

struct texture
{
    unsigned int id;
    std::string type;
    std::string path;
};

struct extra_texture
{
    unsigned int id;
    std::string uniform_name;
    GLenum type;
};

class mesh
{
public:
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<texture> textures;

    mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);

    void draw(const shader& shader, const std::vector<extra_texture>& extra_textures, GLenum mode = GL_TRIANGLES) const;

private:
    unsigned int vao_, vbo_, ebo_;
    void setup_mesh();
};
