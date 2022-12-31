#pragma once

#include "../ecs/component.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace our {

    // enum defining light type (DIRECTIONAL, POINT or SPOT)
    enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };
    // This component adds lighting ability to the owing entity.
    class LightComponent : public Component {
    public:
        LightType type;             // The current type of light, Possible values are Directional, Point and Spot.
        glm::vec3 diffuse;            // The light diffuse
        glm::vec3 specular;           // The light specular
        glm::vec2 cone_angles;      //define the angle of a spot light’s cone (Spot light only).
        glm::vec3 attenuation;   
        // The ID of this component is "Light"
       static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}