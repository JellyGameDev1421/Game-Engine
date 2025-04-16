#include "DemoEngine_PCH.h"
#include "SceneHierarchyPanel.h"
#include "ImGui/ImGUILibary.h"
#include <ImGui/imgui.h>
#include "Scene/Components.h"

namespace DemoEngine
{
	SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene>& context)
	{
		SetContext(context);
		m_InspectorPanel = CreateRef<InspectorPanel>();
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRenderer()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			auto view = m_Context->m_Registry.view < entt::entity>();
			for (auto entityID : view)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			}

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectionContext = {};
			}
			RightClickMenu();
		}
		ImGui::End();
		ImGui::Begin("Inspector");
		if (m_SelectionContext)
		{
			//Draw componenet and add component button inspector
			m_InspectorPanel->DrawComponents(m_SelectionContext);
			m_InspectorPanel->DrawAddComponent(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Duplicate Entity"))
			{
				m_Context->DuplicateEntity(entity);
			}
			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
			{
				m_SelectionContext = {};
			}
		}
	}

	void SceneHierarchyPanel::RightClickMenu()
	{
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			ImGuiLibary::DrawMenuItem("Create Entity", [](Ref<Scene> m_Context) { m_Context->CreateEntity("Empty entity"); }, m_Context);

			ImGuiLibary::DrawMenuItem("Camera", [](Ref<Scene> m_Context)
				{
					auto cams = m_Context->GetAllEntitiesWith<CameraComponent>();
					std::string name = "Main Camera";
					bool primary = true;
					if (cams.size() > 0)
					{
						name = "Camera";
						primary = false;
					}

					Entity cam = m_Context->CreateEntity(name);
					CameraComponent& c = cam.AddComponent<CameraComponent>();
					c.Primary = primary;
				}, m_Context);
			ImGui::EndPopup();
		}
	}
}