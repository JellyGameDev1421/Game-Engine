#include "DemoEngine_PCH.h"
#include "InspectorPanel.h"

#include "ImGui/ImGuiLibary.h"

#include <imgui/imgui.h>
#include "Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>

namespace DemoEngine
{
    void InspectorPanel::DrawAddComponent(Entity entity)
    {
        ImGui::Separator();
        ImGuiLibary::DrawCentredButton("AddComponent", 0.9f, []()
            {
                ImGui::OpenPopup("AddComponent");
            });

        if (ImGui::BeginPopup("AddComponent"))
        {
            DrawAddComponenetMenuItem<SpriteRendererComponent>(entity, "Sprite Renderer");
            DrawAddComponenetMenuItem<CircleRendererComponent>(entity, "Circle Renderer");
            DrawAddComponenetMenuItem<CameraComponent>(entity, "Camera");
            DrawAddComponenetMenuItem<RigidBody2DComponent>(entity, "RigidBody2D");
            DrawAddComponenetMenuItem<BoxCollider2DComponent>(entity, "BoxCollider2D");
            DrawAddComponenetMenuItem<CircleCollider2DComponent>(entity, "CircleCollider2D");
            ImGui::EndPopup();
        }
    }

    template <typename T>
    void InspectorPanel::DrawAddComponenetMenuItem(Entity entity, const char* label)
    {
        if (!entity.HasComponent<T>())
        {
            if (ImGui::MenuItem(label))
            {
                entity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGuiLibary::DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
            {
                ImGuiLibary::DrawVec3Control("Translation", component.Translation);
                glm::vec3 rotation = glm::degrees(component.Rotation);

                ImGuiLibary::DrawVec3Control("Rotation", rotation);
                component.Rotation = glm::radians(rotation);

                ImGuiLibary::DrawVec3Control("Scale", component.Scale, 1.0f);
            });

        ImGuiLibary::DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
            {
                auto& camera = component.camera;
                const char* projectionStringTypes[] = { "Orthographic", "Perspective" };
                const char* currentProjectionString = projectionStringTypes[(int)camera.GetProjectionType()];

                if (ImGui::BeginCombo("Camera Type", currentProjectionString))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = (currentProjectionString == projectionStringTypes[i]);
                        if (ImGui::Selectable(projectionStringTypes[i], isSelected))
                        {
                            currentProjectionString = projectionStringTypes[i];
                            camera.SetProjectionType((ProjectionType)i);
                        }

                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == ProjectionType::Orthographic)
                {
                    float orthoSize = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Size", &orthoSize, 0.1f))
                    {
                        camera.SetOrthographicSize(orthoSize);
                    }

                    float orthoNear = camera.GetOrthographicNear();
                    if (ImGui::DragFloat("Near", &orthoNear, 0.1f))
                    {
                        camera.SetOrthographicNear(orthoNear);
                    }

                    float orthoFar = camera.GetOrthographicFar();
                    if (ImGui::DragFloat("Far", &orthoFar, 0.1f))
                    {
                        camera.SetOrthographicFar(orthoFar);
                    }

                    ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
                }

                if (camera.GetProjectionType() == ProjectionType::Perspective)
                {
                    float perspectiveFov = glm::degrees(camera.GetPerspectiveFOV());
                    if (ImGui::DragFloat("FOV", &perspectiveFov, 0.1f))
                    {
                        camera.SetPerspectiveFOV(glm::radians(perspectiveFov));
                    }

                    float perspectiveNear = camera.GetPerspectiveNear();
                    if (ImGui::DragFloat("Near", &perspectiveNear, 0.1f))
                    {
                        camera.SetPerspectiveNear(perspectiveNear);
                    }

                    float perspectiveFar = camera.GetPerspectiveFar();
                    if (ImGui::DragFloat("Far", &perspectiveFar, 0.1f))
                    {
                        camera.SetPerspectiveFar(perspectiveFar);
                    }
                }
            });

        ImGuiLibary::DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
            {
                ImGui::ColorEdit4("Colour", glm::value_ptr(component.Colour));
            });

        ImGuiLibary::DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
            {
                ImGui::ColorEdit4("Colour", glm::value_ptr(component.Color));
                ImGui::DragFloat4("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
            });



        ImGuiLibary::DrawComponent<RigidBody2DComponent>("Rigidbody 2D", entity, [](auto& component)
            {
                RigidBody2DComponent rb;
                const char* rbTypeStrings[] = { "Static", "Kinematic", "Dynamic"};
                const char* rbCurrentType = rbTypeStrings[(int)component.m_Type];

                if (ImGui::BeginCombo("RigidBody Types", rbCurrentType)) 
                {
                    for (int i = 0; i < 3; i++) 
                    {
                        bool isSelected = (rbCurrentType == rbTypeStrings[i]);
                        if (ImGui::Selectable(rbTypeStrings[i], isSelected))
                        {
                            rbCurrentType = rbTypeStrings[i];
                            LOG_WARN("Selected type: " + std::to_string(i));
                            component.m_Type = static_cast<RigidBody2DComponent::BodyType>(i);

                            if (isSelected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::DragFloat("mass", &component.mass, 0.1f, 0.0f, 100.0f);
                ImGuiLibary::DrawVec2Control("CentreMass", component.CentreMass);
                ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
                ImGui::Checkbox("Gravity", &component.hasGravity);
            });

        ImGuiLibary::DrawComponent<BoxCollider2DComponent>("Box Collider", entity, [](auto& component) 
            {
                ImGuiLibary::DrawVec2Control("Offset", component.Offset);
                ImGuiLibary::DrawVec2Control("Half Extents", component.HalfExtents);

                ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
                ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
            });

        ImGuiLibary::DrawComponent<CircleCollider2DComponent>("Circle Collider", entity, [](auto& component) {
            ImGuiLibary::DrawVec2Control("Offset", component.Offset);
            ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
            });
    }
}