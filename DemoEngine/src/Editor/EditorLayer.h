#pragma once
#include "DemoEngine.h"
#include "Editor/Panels/SceneHierarchyPanel.h"
#include <filesystem>

namespace DemoEngine 
{
	class EditorLayer : public Layer 
	{
	public:
		EditorLayer();

		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		bool OnMouseButtonPressed(MouseButtonPressedEvent e);
		bool OnKeyPressed(KeyPressedEvent e);

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();
		void SerialiseScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();
		void OnDeleteEntity();

	private:
		EditorCamera m_EditorCamera;
		Ref<Framebuffer> m_Framebuffer;
		Ref<ShaderLibary> m_ShaderLibary;

		//Audio
		Ref<AudioPlayer> audioPlayer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene, m_RuntimeScene;
		std::filesystem::path m_EditorSceneFilePath;

		int m_GizmType = -1;

		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec2 m_ViewportBounds[2];

		Entity m_HoveredEntity;

		SceneHierarchyPanel m_SceneHierarchyPanel;

		enum class SceneState 
		{
			Edit = 0,
			Play = 1,
			Pause = 2,
			Simulate = 3
		};

		SceneState m_SceneState = SceneState::Edit;

		//Raycast
		RaycastResult m_LastRaycastResult;
		glm::vec2 m_RaycastOrigin;
		glm::vec2 m_RaycastEnd;
		bool m_ShowRaycast = false;
		std::pair<glm::vec2, glm::vec2> ScreenToWorldRay();

		//Stats and profiling 
		enum class StatGraphType
		{
			CPU,
			Memory,
			FPS
		};

		std::vector<float> m_CPUHistory;
		std::vector<float> m_MemoryHistory;
		std::vector<float> m_FPSHistory;
		int m_GraphIndex = 0;

		StatGraphType m_CurrentStatGraph = StatGraphType::CPU;
		float m_StatGraphTime = 0.0f;

		//Wanna see em all
		bool m_ShowCPU = true;
		bool m_ShowMemory = true;
		bool m_ShowFPS = true;

	};

}