#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/mesh-renderer.hpp"
#include "../asset-loader.hpp"

#include "../components/light.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>
#include <iostream>

namespace our
{

    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand
    {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh *mesh;
        Material *material;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer
    {
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        std::vector<LightComponent *> lights;
        // Objects used for rendering a skybox
        Mesh *skySphere;
        TexturedMaterial *skyMaterial;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial *postprocessMaterial;

    public:
        void render(World *world, glm::ivec2 viewportStart, glm::ivec2 viewportSize)
        {

            CameraComponent *camera = nullptr;
            opaqueCommands.clear();
            transparentCommands.clear();
            for (auto entity : world->getEntities())
            {
                // get all light sources
                if (auto light = entity->getComponent<LightComponent>())
                {
                    lights.push_back(light);
                }

                if (!camera)
                    camera = entity->getComponent<CameraComponent>();

                if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
                {

                    RenderCommand command;
                    command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                    command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                    command.mesh = meshRenderer->mesh;
                    command.material = meshRenderer->material;

                    if (command.material->transparent)
                    {
                        transparentCommands.push_back(command);
                    }
                    else
                    {
                        opaqueCommands.push_back(command);
                    }
                }
            }
            // If there is no camera, we return (we cannot render without a camera)
            if (camera == nullptr)
                return;

            glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
            std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                      { return first.center.z * cameraForward.z > second.center.z * cameraForward.z; });

            glm::mat4 VP = camera->getProjectionMatrix(viewportSize) * camera->getViewMatrix();

            glViewport(viewportStart.x, viewportStart.y, viewportSize.x, viewportSize.y);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glClearDepth(1.0f);

            glColorMask(true, true, true, true);
            glDepthMask(true);
            // TODO: Clear the color and depth buffers
            //  clear the buffers in order to draw the new frame
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // TODO: Draw all the opaque commands followed by all the transparent commands
            //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
            //  we draw opaque commands first as we don't care about order
            //  worst that can happen is re-drawing certain pixels
            for (auto command : opaqueCommands)
            {
                command.material->setup();
                command.material->shader->set("transform", VP * command.localToWorld);

                command.material->shader->set("transform", command.localToWorld);
                // -- for lighting support
                command.material->shader->set("transform_IT", glm::transpose(glm::inverse(command.localToWorld)));
                command.material->shader->set("VP", VP);
                command.material->shader->set("eye", camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1));
                // remove this it should be handeld in light component --
                command.material->shader->set("light_count", 2);
                command.material->shader->set("sky_light.sky", glm::vec3(0.2, 0.6, 0.8));
                command.material->shader->set("sky_light.horizon", glm::vec3(0.5, 0.5, 0.5));
                command.material->shader->set("sky_light.ground", glm::vec3(0.2, 0.7, 0.4));
                command.material->shader->set("lights[0].type", 2);
                command.material->shader->set("lights[0].position", glm::vec3(0, 3, 0));
                command.material->shader->set("lights[0].direction", glm::vec3(0, -1, 0));
                command.material->shader->set("lights[0].color", glm::vec3(1, 0.9, 0.7));
                command.material->shader->set("lights[0].attenuation", glm::vec3(0, 0, 1));
                command.material->shader->set("lights[0].cone_angles", glm::vec2(glm::radians(15.0f), glm::radians(30.0f)));
                command.material->shader->set("lights[1].type", 0);
                command.material->shader->set("lights[1].position", glm::vec3(0, 3, 0));
                command.material->shader->set("lights[1].direction", glm::vec3(-1, 0, 0));
                command.material->shader->set("lights[1].color", glm::vec3(1, 0.0, 0.0));
                command.material->shader->set("lights[1].attenuation", glm::vec3(0, 0, 1));
                command.material->shader->set("lights[1].cone_angles", glm::vec2(glm::radians(15.0f), glm::radians(30.0f)));
                // --
                command.mesh->draw();
            }
            // we draw the transparent commands after opaque commands
            // this way, we can draw the transparent objects in the correct order
            // as order matters in transparent objects
            // color , alpha = alpha * v + (1 - alpha) * v
            for (auto command : transparentCommands)
            {
                // setup material samplers => albedo ..etc
                command.material->setup();
                command.material->shader->set("transform", command.localToWorld); // remove VP for light

                command.material->shader->set("transform_IT", glm::transpose(glm::inverse(command.localToWorld)));
                // -- for lighting support -- //
                command.material->shader->set("VP", VP);
                glm::vec4 eye = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                command.material->shader->set("eye", glm::vec3(eye));
                command.material->shader->set("light_count", (int)lights.size());
                // command.material->shader->set("sky_light.sky", glm::vec3(0.2, 0.6, 0.8));
                // command.material->shader->set("sky_light.horizon", glm::vec3(0.5, 0.5, 0.5));
                // command.material->shader->set("sky_light.ground", glm::vec3(0.2, 0.7, 0.4));
                int light_index = 0;
                const int MAX_LIGHT_COUNT = 8;
                for (const auto &light : lights)
                {
                    std::string prefix = "lights[" + std::to_string(light_index) + "].";
                    command.material->shader->set(prefix + "type", static_cast<int>(light->type));

                    command.material->shader->set(prefix + "color", glm::normalize(light->color));

                    switch (light->type)
                    {
                    case LightType::DIRECTIONAL:
                        command.material->shader->set(prefix + "direction", glm::normalize(light->direction));
                        break;
                    case LightType::POINT:
                        glm::vec4 light4 = light->getOwner()->getLocalToWorldMatrix() * glm::vec4(light->getOwner()->localTransform.position, 1);
                        command.material->shader->set(prefix + "position", glm::vec3(light4));
                        command.material->shader->set(prefix + "attenuation", light->attenuation);
                        break;
                    case LightType::SPOT:
                        glm::vec4 Slight = light->getOwner()->getLocalToWorldMatrix() * glm::vec4(light->getOwner()->localTransform.position, 1);
                        command.material->shader->set(prefix + "position", glm::vec3(Slight));
                        command.material->shader->set(prefix + "direction", glm::normalize(light->direction));
                        command.material->shader->set(prefix + "attenuation", light->attenuation);
                        command.material->shader->set(prefix + "cone_angles", glm::vec2(glm::radians(light->cone_angles[0]), glm::radians(light->cone_angles[1])));
                        break;
                    }
                    light_index++;
                    if (light_index >= MAX_LIGHT_COUNT)
                        break;
                }
                command.mesh->draw();
                // command.material->shader->set("transform_IT", glm::transpose(glm::inverse(command.localToWorld)));
                // command.material->shader->set("VP", VP);
                // command.material->shader->set("eye", camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1));
                // //remove this it should be handeld in light component --
                // command.material->shader->set("light_count", 2);
                // command.material->shader->set("sky_light.sky", glm::vec3(0.2, 0.6, 0.8));
                // command.material->shader->set("sky_light.horizon", glm::vec3(0.5, 0.5, 0.5));
                // command.material->shader->set("sky_light.ground", glm::vec3(0.2, 0.7, 0.4));
                // std::cout << "SKY Light DONE" << std::endl;
                // command.material->shader->set("lights[0].type", 2);
                // command.material->shader->set("lights[0].position", glm::vec3(0, 3, 0));
                // command.material->shader->set("lights[0].direction", glm::vec3(0, -1, 0));
                // command.material->shader->set("lights[0].color", glm::vec3(1, 0.9, 0.7));
                // command.material->shader->set("lights[0].attenuation", glm::vec3(0, 0, 1));
                // command.material->shader->set("lights[0].cone_angles", glm::vec2(glm::radians(15.0f), glm::radians(30.0f)));
                // command.material->shader->set("lights[1].type", 0);
                // command.material->shader->set("lights[1].position", glm::vec3(0, 3, 0));
                // command.material->shader->set("lights[1].direction", glm::vec3(-1, 0, 0));
                // command.material->shader->set("lights[1].color", glm::vec3(1, 0.0, 0.0));
                // command.material->shader->set("lights[1].attenuation", glm::vec3(0, 0, 1));
                // command.material->shader->set("lights[1].cone_angles", glm::vec2(glm::radians(15.0f), glm::radians(30.0f)));
                // // -- //
            }

            for (auto command : transparentCommands)
            {
                command.material->setup();
                command.material->shader->set("transform", VP * command.localToWorld);

                command.mesh->draw();
            }
        }
    };
};