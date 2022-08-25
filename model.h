#pragma once
#include <string>

#include "mesh.h"
#include "shader.h"
#include <assimp/scene.h>

struct model_params
{
    bool texture_clamp = false;
    bool texture_flip = true;

    static model_params get_default()
    {
        const model_params params;
        return params;
    }
};

unsigned int texture_from_file(const char* path, const std::string& directory, const model_params& model_params,
                               bool gamma = false);

class model
{
public:
    explicit model(const std::string& path, const model_params& params = model_params::get_default());

    void draw(const shader& shader) const;
    void draw(const shader& shader, const std::vector<extra_texture>& extra_textures) const;

private:
    std::vector<mesh> meshes_;
    std::string directory_;
    std::vector<texture> textures_loaded_;
    model_params params_;

    void load_model(const std::string& path);
    void process_node(const aiNode* node, const aiScene* scene);
    mesh process_mesh(const aiMesh* ai_mesh, const aiScene* scene);
    std::vector<texture> load_material_textures(const aiMaterial* material, aiTextureType type,
                                                const std::string& type_name);
};
