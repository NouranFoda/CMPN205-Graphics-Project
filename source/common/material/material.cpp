#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 7) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        glActiveTexture(GL_TEXTURE0);
        if(texture) texture->bind();
        else Texture2D::unbind();
        if(sampler) sampler->bind(0);
        else Sampler::unbind(0);
        shader->set("tex", 0);
        
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LitMaterial::setup() const {
        
        Material::setup();
        shader->set("alphaThreshold", alphaThreshold);
        
        glActiveTexture(GL_TEXTURE0);
        if(albedo_tex) albedo_tex->bind();
        else Texture2D::unbind();
        sampler->bind(0);
        shader->set("material.albedo_tex", 0);

        glActiveTexture(GL_TEXTURE1);
        if(specular_tex) specular_tex->bind();
        else Texture2D::unbind();
        sampler->bind(1);
        shader->set("material.specular_tex", 1);

        glActiveTexture(GL_TEXTURE2);
        if(roughness_tex) roughness_tex->bind();
        else Texture2D::unbind();
        sampler->bind(2);
        shader->set("material.roughness_tex", 2);

        glActiveTexture(GL_TEXTURE3);
        if(ao_tex) ao_tex->bind();
        else Texture2D::unbind();
        sampler->bind(3);
        shader->set("material.ao_tex", 3);

        glActiveTexture(GL_TEXTURE4);
        if(emission_tex) emission_tex->bind();
        else Texture2D::unbind();
        sampler->bind(4);
        shader->set("material.emission_tex", 4);
    }

    void LitMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        albedo_tex = AssetLoader<Texture2D>::get(data.value("albedo-tex", ""));
        specular_tex = AssetLoader<Texture2D>::get(data.value("specular-tex", ""));
        roughness_tex = AssetLoader<Texture2D>::get(data.value("roughness-tex", ""));
        ao_tex = AssetLoader<Texture2D>::get(data.value("ao-tex", ""));
        emission_tex = AssetLoader<Texture2D>::get(data.value("emission-tex", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}