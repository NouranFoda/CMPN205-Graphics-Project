#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"
#include <iostream>

namespace our
{

    void Material::setup() const
    {
        pipelineState.setup();
        shader->use();
    }

    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    void TintedMaterial::setup() const
    {

        Material::setup();
        shader->set("tint", tint);
    }

    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void TexturedMaterial::setup() const
    {

        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        sampler->bind(0);
        // send the unit number to the uniform variable "tex"
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }
    void LitMaterial::setup() const
    {
        Material::setup();
        shader->set("alphaThreshold", alphaThreshold);
        glActiveTexture(GL_TEXTURE0);
        albedo_tex->bind();
        sampler->bind(0);
        shader->set("material.albedo", 0);
        glActiveTexture(GL_TEXTURE1);
        specular_tex->bind();
        sampler->bind(1);
        shader->set("material.specular", 1);
        glActiveTexture(GL_TEXTURE2);
        roughness_tex->bind();
        sampler->bind(2);
        shader->set("material.roughness", 2);
        glActiveTexture(GL_TEXTURE3);
        ao_tex->bind();
        sampler->bind(3);
        shader->set("material.ambient_occlusion", 3);
        glActiveTexture(GL_TEXTURE4);
        emission_tex->bind();
        sampler->bind(4);
        shader->set("material.emission", 4);
    }

    // This function read the material data from a json object
    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        albedo_tex = AssetLoader<Texture2D>::get(data.value("albedo-texture", ""));
        specular_tex = AssetLoader<Texture2D>::get(data.value("specular-texture", ""));
        roughness_tex = AssetLoader<Texture2D>::get(data.value("roughness-texture", ""));
        ao_tex = AssetLoader<Texture2D>::get(data.value("ambient-occlusion-texture", ""));
        emission_tex = AssetLoader<Texture2D>::get(data.value("emission-texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}