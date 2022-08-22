#include "model.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

unsigned int texture_from_file(const char* path, const std::string& directory, const model_params& model_params,
                               bool gamma)
{
    auto filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, nr_components;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nr_components, 0);
    if (data)
    {
        GLint format = 0;
        if (nr_components == 1)
            format = GL_RED;
        else if (nr_components == 3)
            format = GL_RGB;
        else if (nr_components == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        const auto wrap_mode = model_params.texture_clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << " (" << stbi_failure_reason() << ")" <<
            std::endl;
        stbi_image_free(data);
    }

    return texture_id;
}

model::model(const std::string& path, const model_params& params):
    params_(params)
{
    load_model(path);
}

void model::draw(const shader& shader) const
{
    for (auto& mesh : meshes_)
    {
        mesh.draw(shader);
    }
}

void model::load_model(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory_ = path.substr(0, path.find_last_of('/'));
    process_node(scene->mRootNode, scene);
}

void model::process_node(const aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(node->mChildren[i], scene);
    }
}

mesh model::process_mesh(const aiMesh* ai_mesh, const aiScene* scene)
{
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<texture> textures;

    for (size_t i = 0; i < ai_mesh->mNumVertices; ++i)
    {
        vertex vertex{};
        const aiVector3D position = ai_mesh->mVertices[i];
        vertex.position = glm::vec3(position.x, position.y, position.z);
        const aiVector3D normal = ai_mesh->mNormals[i];
        vertex.normal = glm::vec3(normal.x, normal.y, normal.z);

        if (ai_mesh->mTextureCoords[0])
        {
            const aiVector3D tex_coords = ai_mesh->mTextureCoords[0][i];
            vertex.tex_coords = glm::vec2(tex_coords.x, tex_coords.y);
        }
        else
        {
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (size_t face_index = 0; face_index < ai_mesh->mNumFaces; ++face_index)
    {
        const aiFace face = ai_mesh->mFaces[face_index];
        for (size_t i = 0; i < face.mNumIndices; ++i)
        {
            indices.push_back(face.mIndices[i]);
        }
    }

    const aiMaterial* material = scene->mMaterials[ai_mesh->mMaterialIndex];
    std::vector<texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    std::vector<texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

    return {vertices, indices, textures};
}

std::vector<texture> model::load_material_textures(const aiMaterial* material, const aiTextureType type,
                                                   const std::string& type_name)
{
    stbi_set_flip_vertically_on_load(params_.texture_flip);
    std::vector<texture> textures;

    for (unsigned int texture_index = 0; texture_index < material->GetTextureCount(type); ++texture_index)
    {
        aiString path;
        material->GetTexture(type, texture_index, &path);

        bool skip = false;
        for (auto& loaded_index : textures_loaded_)
        {
            if (std::strcmp(loaded_index.path.data(), path.C_Str()) == 0)
            {
                textures.push_back(loaded_index);
                skip = true;
                break;
            }
        }

        if (skip) continue;

        texture texture;
        texture.id = texture_from_file(path.C_Str(), directory_, params_);
        texture.type = type_name;
        texture.path = path.C_Str();
        textures.push_back(texture);
        textures_loaded_.push_back(texture);
    }

    return textures;
}
