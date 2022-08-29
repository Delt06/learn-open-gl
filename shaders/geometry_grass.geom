#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

out float Occlusion;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int segments;
uniform float width;
uniform float height;
uniform float bendDegree;

vec3 rotateAboutAxis(vec3 vector, vec3 axis, float angle)
{
    angle = radians(angle);
    float s = sin(angle);
    float c = cos(angle);
    float one_minus_c = 1.0 - c;

    axis = normalize(axis);
    mat3 rot_mat;
    rot_mat[0] = vec3(one_minus_c * axis.x * axis.x + c, one_minus_c * axis.x * axis.y - axis.z * s, one_minus_c * axis.z * axis.x + axis.y * s);
    rot_mat[1] = vec3(one_minus_c * axis.x * axis.y + axis.z * s, one_minus_c * axis.y * axis.y + c, one_minus_c * axis.y * axis.z - axis.x * s);
    rot_mat[2] = vec3(one_minus_c * axis.z * axis.x - axis.y * s, one_minus_c * axis.y * axis.z + axis.x * s, one_minus_c * axis.z * axis.z + c);
    return rot_mat * vector;
}

vec4 transformObjectToHClip(vec3 positionOS) {
    return projection * view * model * vec4(positionOS, 1);
}

void emitGrassVertex(const in vec3 pivot, const float t, const float side) {
    float dx = mix(width * 0.5, 0, t);
    float dy = t * height;

    vec3 offset = vec3(dx * side, dy, 0);
    offset = rotateAboutAxis(offset, vec3(1, 0, 0), bendDegree * t);

    gl_Position = transformObjectToHClip(pivot + offset);
    Occlusion = t;
    EmitVertex();
}

void main() {
    vec3 pivot = gl_in[0].gl_Position.xyz;

    for (int i = 0; i < segments; i++) {
        float t = float(i) / (segments - 1);

        emitGrassVertex(pivot, t, -1);
        emitGrassVertex(pivot, t, 1);
    }

    emitGrassVertex(pivot, 1, 0);
}