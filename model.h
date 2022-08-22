#pragma once
#include <string>

#include "mesh.h"
#include "shader.h"
#include <assimp/scene.h>

unsigned int texture_from_file(const char* path, const std::string& directory, bool gamma = false);

class model
{
public:
    explicit model(const std::string& path)
    {
        load_model(path);
    }

    void draw(const shader& shader) const;

private:
    std::vector<mesh> meshes_;
    std::string directory_;
    std::vector<texture> textures_loaded_;

    void load_model(const std::string& path);
    void process_node(const aiNode* node, const aiScene* scene);
    mesh process_mesh(const aiMesh* ai_mesh, const aiScene* scene);
    std::vector<texture> load_material_textures(const aiMaterial* material, aiTextureType type,
                                                const std::string& type_name);
};
