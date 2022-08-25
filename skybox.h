#pragma once
#include "cubemap.h"
#include "shader.h"

class skybox
{
public:
    explicit skybox(cubemap cubemap, shader shader);

    void draw(const glm::mat4x4& view, const glm::mat4x4& projection) const;

private:
    cubemap cubemap_;
    shader shader_;
    GLuint vao_;
    GLuint vbo_;

    void load_mesh();
};
