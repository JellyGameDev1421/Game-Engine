#include "DemoEngine_PCH.h"
#include "Editor/EditorLayer.h"
#include "ImGui/ImGuiLibary.h"
#include "ImGuizmo.h"
#include "Math/Math.h"

#include "Utils/PlatformUtils.h"
#include "Utils/SystemStats.h"
#include "Scene/SceneSerialiser.h"

namespace DemoEngine
{
	EditorLayer::EditorLayer() : Layer("EditorLayer") { }

	void EditorLayer::OnAttach()
	{
		m_EditorCamera = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

		FramebufferSpecification framebufferSpec;
		framebufferSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		framebufferSpec.Width = 1280;
		framebufferSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(framebufferSpec);

		//Audio
		audioPlayer = CreateRef<AudioPlayer>();
		audioPlayer->Init();

		//Initialising for profiling
		m_CPUHistory.resize(100, 0.0f);
		m_MemoryHistory.resize(100, 0.0f);
		m_FPSHistory.resize(100, 0.0f);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_ShaderLibary = CreateRef<ShaderLibary>();

		m_ShaderLibary->Load("FlatColour", "src/Renderer/Shader/FlatColourShader.glsl");

		Entity cam = m_EditorScene->CreateEntity("Main Camera");
		cam.AddComponent<CameraComponent>();

	}

	void EditorLayer::OnDetach() {}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		// Old stuff
		////Resize Framebuffer if necessary
		//if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		//	m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && //zero sized framebuffer is invalid
		//	(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		//{
		//	m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		//	m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		//	m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		//	m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		//}

		// Who doesn't like some frame buffer changes to make stuff quicker
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f)
		{
			constexpr float resizeThreshold = 1.0f;
			if (fabs((float)spec.Width - m_ViewportSize.x) > resizeThreshold ||
				fabs((float)spec.Height - m_ViewportSize.y) > resizeThreshold)
			{
				m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
				m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
				m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
				m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			}
		}

		// Drawing raycast its very buggy though
		if (m_ShowRaycast)
		{
			Renderer2D::BeginScene(m_EditorCamera);
			Renderer2D::DrawRaycast(m_RaycastOrigin, m_RaycastEnd, { 1, 0, 0, 1 });

			if (m_LastRaycastResult.hit)
			{
				glm::mat4 marker = glm::translate(glm::mat4(1.0f), glm::vec3(m_LastRaycastResult.point, 0.1f));
				marker = glm::scale(marker, glm::vec3(0.1f));
				Renderer2D::DrawCircle(marker, { 1, 1, 0, 1 }, 1.0f, 0.001f);
			}

			Renderer2D::EndScene();
		}

		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		Renderer2D::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
		Renderer2D::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
		case SceneState::Edit:
			m_EditorCamera.OnUpdate(ts);
			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			break;

		case SceneState::Play:
			//m_ActiveScene->OnUpdateRuntime(ts);
			m_ActiveScene->OnUpdateRuntime(ts);
			break;
		}

		m_Framebuffer->Unbind();

		//Profiling to show stats
		SystemStats::Update();
		const auto& profileStats = SystemStats::Get();
		float fps = 1.0f / ts;

		// Making the Graph for stats
		m_StatGraphTime += ts;
		if (m_StatGraphTime >= 0.1f) // update every 0.1s
		{
			m_StatGraphTime = 0.0f;

			auto pushBackLimited = [](std::vector<float>& vec, float val)
				{
					vec.erase(vec.begin());
					vec.push_back(val);
				};

			// Lets grab the cpu ram and fps usage
			pushBackLimited(m_CPUHistory, profileStats.cpuUsagePercent);
			// Quick little maths
			pushBackLimited(m_MemoryHistory, profileStats.memoryUsageBytes / (1024.0f * 1024.0f));
			pushBackLimited(m_FPSHistory, fps);
		}

		auto stats = Renderer2D::GetStats();
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		ImGuiLibary::CreateDockspace(dockspaceOpen, "Dockspace Demo");

		// Keyboard shortcuts
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+0"))
				{
					OpenScene();
					audioPlayer->Close();
				}

				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
					SaveScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit", "Ctrl+Q"))
				{
					dockspaceOpen = false;
					Application::Get().Close();

				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// I finally understand how to make a panel lets gooo
		m_SceneHierarchyPanel.OnImGuiRenderer();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		ImGui::Begin("Viewport");

		float toolbarHeight = 30.0f;
		ImGui::BeginChild("Toolbar", ImVec2(ImGui::GetContentRegionAvail().x, toolbarHeight), true);
		ImVec2 buttonSize = { 50, toolbarHeight };

		if (ImGui::Button("Move", buttonSize))
		{
			if (!ImGuizmo::IsUsing())
			{
				m_GizmType = ImGuizmo::OPERATION::TRANSLATE;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Rotate", buttonSize))
		{
			if (!ImGuizmo::IsUsing())
			{
				m_GizmType = ImGuizmo::OPERATION::ROTATE;
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Scale", buttonSize))
		{
			if (!ImGuizmo::IsUsing())
			{
				m_GizmType = ImGuizmo::OPERATION::SCALE;
			}
		}

		ImGui::NextColumn();
		ImGui::SameLine();

		char* buttonName = (m_SceneState == SceneState::Edit) ? "Play" : "Stop";

		if (ImGui::Button(buttonName, buttonSize))
		{
			if (m_SceneState == SceneState::Edit)
			{
				OnScenePlay();
			}

			else if (m_SceneState == SceneState::Play)
			{
				OnSceneStop();
			}
		}

		ImGui::EndChild();

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		auto viewportOffset = ImGui::GetCursorScreenPos();

		m_ViewportBounds[0] = { viewportOffset.x, viewportOffset.y };
		m_ViewportBounds[1] = { viewportOffset.x + viewportPanelSize.x, viewportOffset.y + viewportPanelSize.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint32_t textureID = m_Framebuffer->GetColourAttachmentRendererID();
		ImGui::Image((ImTextureID)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0,1 }, ImVec2{ 1, 0 });

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);


			ImGuizmo::SetRect(
				viewportOffset.x, 
				viewportOffset.y, 
				viewportPanelSize.x, 
				viewportPanelSize.y);

			//Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			//Entity transform
			auto& entityTransform = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = entityTransform.GetTransform();

			bool snap = Input::IsKeyPressed(Key::LeftShift);
			float snapValue = 0.5f;
			if (m_GizmType == ImGuizmo::OPERATION::ROTATE)
			{
				snapValue = 45.0f;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				entityTransform.Translation = translation;
				entityTransform.Scale = scale;
				if (m_GizmType == ImGuizmo::OPERATION::ROTATE)
					entityTransform.Rotation = rotation;

				//glm::vec3 deltaRotation = rotation - entityTransform.Rotation;
				//entityTransform.Rotation += deltaRotation;
			}
		}
		ImGui::End();

		//System Stats
		ImGui::Begin("Engine Stats");

		// Toggle Buttons
		//if (ImGui::Button("CPU##CPUStat")) m_CurrentStatGraph = StatGraphType::CPU;
		//ImGui::SameLine();
		//if (ImGui::Button("Memory##MemStat")) m_CurrentStatGraph = StatGraphType::Memory;
		//ImGui::SameLine();
		//if (ImGui::Button("FPS##FPSStat")) m_CurrentStatGraph = StatGraphType::FPS;

		//Check boxes in case i want to see multiple at a time
		ImGui::Checkbox("Show CPU", &m_ShowCPU);
		ImGui::SameLine();
		ImGui::Checkbox("Show Memory", &m_ShowMemory);
		ImGui::SameLine();
		ImGui::Checkbox("Show FPS", &m_ShowFPS);

		std::vector<float>* data = nullptr;
		std::string label;
		ImVec4 color;
		float currentValue = 0.0f;

		//Old way
		//switch (m_CurrentStatGraph)
		//{
		//case StatGraphType::CPU:
		//	data = &m_CPUHistory;
		//	currentValue = m_CPUHistory.back();
		//	label = "CPU Usage (%%): " + std::to_string(std::round(currentValue * 100.0f) / 100.0f);
		//	color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
		//	break;
		//case StatGraphType::Memory:
		//	data = &m_MemoryHistory;
		//	currentValue = m_MemoryHistory.back();
		//	label = "Memory Usage (MB): " + std::to_string(std::round(currentValue * 100.0f) / 100.0f);
		//	color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
		//	break;
		//case StatGraphType::FPS:
		//	data = &m_FPSHistory;
		//	currentValue = m_FPSHistory.back();
		//	label = "FPS: " + std::to_string(std::round(currentValue * 100.0f) / 100.0f);
		//	color = ImVec4(0.4f, 0.6f, 1.0f, 1.0f);
		//	break;
		//}

		// This used to be so simple now its blowing my mind
		if (m_ShowCPU)
		{
			float current = m_CPUHistory.back();
			std::string label = "CPU Usage (%): " + std::to_string(std::round(current * 100.0f) / 100.0f);
			ImGui::Text("%s", label.c_str());
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			ImGui::PlotLines("##CPU", m_CPUHistory.data(), (int)m_CPUHistory.size(), m_GraphIndex, nullptr, 0.0f, 200.0f, ImGui::GetContentRegionAvail());
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		if (m_ShowMemory)
		{
			float current = m_MemoryHistory.back();
			std::string label = "Memory Usage (MB): " + std::to_string(std::round(current * 100.0f) / 100.0f);
			ImGui::Text("%s", label.c_str());
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
			ImGui::PlotLines("##Memory", m_MemoryHistory.data(), (int)m_MemoryHistory.size(), m_GraphIndex, nullptr, 0.0f, 200.0f, ImGui::GetContentRegionAvail());
			ImGui::PopStyleColor();
			ImGui::Separator();
		}

		if (m_ShowFPS)
		{
			float current = m_FPSHistory.back();
			std::string label = "FPS: " + std::to_string(std::round(current * 100.0f) / 100.0f);
			ImGui::Text("%s", label.c_str());
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
			ImGui::PlotLines("##FPS", m_FPSHistory.data(), (int)m_FPSHistory.size(), m_GraphIndex, nullptr, 0.0f, 200.0f, ImGui::GetContentRegionAvail());
			ImGui::PopStyleColor();
		}

		//ImGui::Text("%s", label.c_str()); // Show the stat label at the top

		//ImGui::Spacing();
		//ImGui::Separator();

		// Drawing the graph
		//ImGui::PushStyleColor(ImGuiCol_PlotLines, color);
		//ImVec2 graphSize = ImGui::GetContentRegionAvail();
		//ImGui::PlotLines("##StatGraph", data->data(), (int)data->size(), m_GraphIndex, nullptr, 0.0f, 200.0f, graphSize);
		//ImGui::PopStyleColor();

		ImGui::End();

		ImGui::PopStyleVar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch <MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch <KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	// Getting the screen to play window from mouse position 
	// Something in here is horribly wrong because its only working in full screen and extremely buggy 
	// DO NOT TOUCH ANYTHING HERE
	std::pair<glm::vec2, glm::vec2> EditorLayer::ScreenToWorldRay()
	{
		glm::vec2 mousePos = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);

		// Convert mouse position to viewport-local space
		glm::vec2 localMouse = mousePos - m_ViewportBounds[0];
		localMouse.y = m_ViewportSize.y - localMouse.y; // Flip Y

		// Out of bounds check
		if (localMouse.x < 0 || localMouse.y < 0 ||
			localMouse.x > m_ViewportSize.x || localMouse.y > m_ViewportSize.y)
		{
			return { glm::vec2(0.0f), glm::vec2(0.0f) };
		}

		// Convert to Normalized Device Coordinates (-1 to 1)
		glm::vec2 ndc = (localMouse / m_ViewportSize) * 2.0f - 1.0f;

		glm::mat4 viewProj = m_EditorCamera.GetProjection() * m_EditorCamera.GetViewMatrix();
		glm::mat4 inverseViewProj = glm::inverse(viewProj);

		glm::vec4 worldPoint = inverseViewProj * glm::vec4(ndc, 0.0f, 1.0f);
		worldPoint /= worldPoint.w;

		glm::vec2 rayOrigin = glm::vec2(m_EditorCamera.GetPosition());
		glm::vec2 rayTarget = glm::vec2(worldPoint);
		glm::vec2 rayDir = glm::normalize(rayTarget - rayOrigin);

		return { rayOrigin, rayDir };
	}

	
	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}

		//When clicking do these things only in play mode and left mouse button
		if (e.GetMouseButton() == Mouse::ButtonLeft && m_SceneState == SceneState::Play && m_ViewportHovered)
		{
			//glm::vec2 mousePos = glm::vec2(Input::GetMousePosition().first, Input::GetMousePosition().second);

			//Better mouse use
			auto [rayOrigin, rayDir] = ScreenToWorldRay();
			if (rayDir == glm::vec2(0)) return false; // Mouse out of viewport

			RaycastResult result = m_ActiveScene->Raycast(rayOrigin, rayDir, 100.0f);

			m_RaycastOrigin = rayOrigin;
			m_RaycastEnd = rayOrigin + rayDir * 100.0f;
			m_LastRaycastResult = result;
			m_ShowRaycast = true;

			//glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			//glm::vec2 viewportSize = glm::vec2(m_Framebuffer->GetSpecification().Width, m_Framebuffer->GetSpecification().Height);
			//glm::vec2 localMouse = mousePos - m_ViewportBounds[0];
			//localMouse.y = viewportSize.y - localMouse.y;

			//glm::vec2 ndc = (localMouse / viewportSize) * 2.0f - 1.0f;

			//glm::mat4 viewProj = m_EditorCamera.GetProjection() * m_EditorCamera.GetViewMatrix();
			//glm::mat4 inverse = glm::inverse(viewProj);
			//glm::vec4 world = inverse * glm::vec4(ndc, 0.0f, 1.0f);
			//world /= world.w;

			//glm::vec2 rayOrigin = glm::vec2(m_EditorCamera.GetPosition());
			//glm::vec2 rayTarget = glm::vec2(world);
			//glm::vec2 rayDir = glm::normalize(rayTarget - rayOrigin);

			// The result hits something do this 
			if (result.hit)
			{
				LOG_INFO("Ray Origin: {}, {}", rayOrigin.x, rayOrigin.y);
				LOG_INFO("Ray Dir: {}, {}", rayDir.x, rayDir.y);

				// Get the UUID
				Entity hitEntity = m_ActiveScene->FindEntityByUUID(result.hitEntity);

				if (hitEntity && hitEntity.HasComponent<TagComponent>())
				{
					std::string name = hitEntity.GetComponent<TagComponent>().Tag;
					LOG_INFO("Raycast hit entity with tag: {}", name); // This better work
					//Yessssssss

					// Destroy any tag starting with "Enemy"
					if (name.rfind("Enemy", 0) == 0)
					{
						m_ActiveScene->DestroyEntity(hitEntity);
						LOG_INFO("Destroyed entity: {}", name);
					}
				}

				// Update end point to actual hit point
				m_RaycastEnd = result.point;

				//LOG_INFO("RAYCAST HIT! Entity: {}", name);

			}
		}
		return false;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent e)
	{
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
		case Key::N:
			if (control)
				NewScene();
			break;
		case Key::O:
		{
			if (control)
				OpenScene();
			break;
		}
		case Key::S:
		{
			if (control && shift)
				SaveSceneAs();
			if (control && !shift)
				SaveScene();
			break;
		}
		case Key::D:
			if (!control && shift)
				OnDuplicateEntity();
			break;
		case Key::Delete:
			OnDeleteEntity();
			break;
		case Key::Q:
			if (m_ViewportHovered)
				m_GizmType = -1;
			break;
		case Key::W:
			if (!ImGuizmo::IsUsing() && m_ViewportHovered)
				m_GizmType = ImGuizmo::OPERATION::TRANSLATE;
			break;
		case Key::E:
			if (!ImGuizmo::IsUsing() && m_ViewportHovered)
				m_GizmType = ImGuizmo::OPERATION::SCALE;
			break;
		case Key::R:
			if (!ImGuizmo::IsUsing() && m_ViewportHovered)
				m_GizmType = ImGuizmo::OPERATION::ROTATE;
			break;
		}
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorSceneFilePath.clear();
	}

	void EditorLayer::OpenScene()
	{
		std::string path = FileDialogs::OpenFile("DemoEngine Scene (*.demoengine)\0*.demoengine\0");
		if (!path.empty())
		{
			OpenScene(path);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
		{
			OnSceneStop();
		}

		if (path.extension().string() != ".demoengine")
		{
			LOG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}
		m_EditorSceneFilePath = path;

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerialiser serialiser(newScene);
		if (serialiser.Deserialise(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorSceneFilePath.empty())
		{
			SerialiseScene(m_EditorScene, m_EditorSceneFilePath);
		}
		else
		{
			SaveSceneAs();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string path = FileDialogs::SaveFile("DemoEngine Scene (*.demoengine)\0*.demoengine\0");
		if (!path.empty())
		{
			SerialiseScene(m_EditorScene, path);
		}
		m_EditorSceneFilePath = path;
	}

	void EditorLayer::SerialiseScene(Ref<Scene>scene, const std::filesystem::path& path)
	{
		SceneSerialiser serialiser(scene);
		serialiser.Serialise(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_RuntimeScene = CreateRef<Scene>();

		Scene::CopyTo(m_EditorScene, m_RuntimeScene);

		m_RuntimeScene->OnRuntimeStart();

		m_ActiveScene = m_RuntimeScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_RuntimeScene->OnRunTimeStop();

		m_RuntimeScene = nullptr;

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			LOG_WARN("Entity Duplication permitted in edit mode only! ");
			return;
		}
		if (m_SceneHierarchyPanel.GetSelectedEntity())
		{
			m_EditorScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity());
		}
	}

	void EditorLayer::OnDeleteEntity()
	{
		if (m_SceneState != SceneState::Edit)
		{
			LOG_WARN("Etntity deletion permitted in edit model only! ");
			return;
		}
		if (m_SceneHierarchyPanel.GetSelectedEntity())
		{
			m_EditorScene->DestroyEntity(m_SceneHierarchyPanel.GetSelectedEntity());
			m_SceneHierarchyPanel.SetSelectedEntity({});
		}
	}
}
