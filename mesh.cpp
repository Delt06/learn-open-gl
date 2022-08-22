// ReSharper disable CppClangTidyPerformanceNoIntToPtr
#include "mesh.h"

#include "glad/glad.h"

mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned> indices,
           std::vector<texture> textures)
    :
    vertices(std::move(vertices)),
    indices(std::move(indices)),
    textures(std::move(textures)),
    vao_(0), vbo_(0), ebo_(0)
{
    setup_mesh();
}

void mesh::draw(const shader& shader) const
{
    unsigned int diffuse_number = 1, specular_number = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuse_number++);
        else if (name == "texture_specular")
            number = std::to_string(specular_number++);

        shader.set_int("material." + name + number, static_cast<int>(i));
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void mesh::setup_mesh()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(vertex)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned)), indices.data(),
                 GL_STATIC_DRAW);

    // positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), static_cast<void*>(nullptr));
    // normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(offsetof(vertex, normal)));
    // tex coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex),
                          reinterpret_cast<void*>(offsetof(vertex, tex_coords)));

    glBindVertexArray(0);
}
