#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include <iostream>
namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/components/movement.hpp"
    class CollisionSystem
    {
        Application* app; // The application in which the state runs
    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }
        // This should be called every frame to update all entities containing a MovementComponent.
        void update(World *world)
        {

            glm::vec3 playerPosition = glm::vec3(0, 0, 0);
            float playerRadius = 0;

            Entity *playerMesh = nullptr;

            for (auto entity : world->getEntities())
            {
                if (entity->name == "monkey")
                {
                    playerMesh = entity;
                    break;
                }
            }

            if (!playerMesh)
                return;
                
                

            // get the current player entity and get its position and radius

            CollisionComponent *collision = playerMesh->getComponent<CollisionComponent>();

            playerPosition = collision->center + glm::vec3(playerMesh->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1));
            playerRadius = collision->radius * glm::length(playerMesh->localTransform.scale);

            for (auto entity : world->getEntities())
            {

                // get all entities that have collision component
                CollisionComponent *collision = entity->getComponent<CollisionComponent>();
                if (collision && entity->name != "monkey")
                {
                    // get the new radius and position of the entity
                    glm::vec3 newPosition = collision->center + glm::vec3(entity->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1));
                    float newRadius = collision->radius * glm::length(entity->localTransform.scale);
                    // compare with player position to check if it collides or not
                    if (glm::length(newPosition - playerPosition) < playerRadius + newRadius)
                    {
                        // collision occurred
                        if (entity->name == "car")
                        {
                            app->changeState("menu");
                        }
                        if (entity->name == "coin")
                        {
                            world->markForRemoval(entity);
                            world->deleteMarkedEntities();
                        }
                    }
                }
            }
        }
    };

}