#include "light.hpp"
#include <iostream>


namespace our {
    // Reads Light type & color & cone angles from the given json object
    void LightComponent::deserialize(const nlohmann::json& data) {

        if (!data.is_object()) return;

        std::string lightTypeStr = data.value("lightType", "directional");

        std::cout<<"light deserializer in light comp"<<std::endl;

        if (lightTypeStr == "directional") {
            type = LightType::DIRECTIONAL;
        }
        else if(lightTypeStr == "point"){
            type = LightType::POINT;
        }
        else if (lightTypeStr == "spot"){
            type = LightType::SPOT;
        }
        diffuse = data.value("diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        specular = data.value("specular", glm::vec3(1.0f, 1.0f, 1.0f));
        cone_angles = data.value("cone-angels", glm::vec2(glm::radians(15.0f), glm::radians(30.0f)));
        attenuation = data.value("attenuation", glm::vec3(0.0f, 0.0f, 1.0f));
    }
}