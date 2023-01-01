#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: (Req 3) Write this function
        //create translation matrix from position vector
        glm::highp_mat4 Translation = glm::translate(glm::mat4(1.0f), position);
        //create rotation matrix from euler angles 
        glm::highp_mat4 Rotation = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);
        //create scaling matrix from scale vector
        glm::highp_mat4 Scaling = glm::scale(glm::mat4(1.0f), scale);
        //final transformation matrix
        return Translation * Rotation * Scaling;
        //return glm::mat4(1.0f); 
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}