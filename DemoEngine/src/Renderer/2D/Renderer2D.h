#pragma once
#include "Renderer/Camera/Camera.h"
#include "Renderer/Camera/EditorCamera.h"

#include "Scene/Components.h"

namespace DemoEngine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const glm::vec4& color);

		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void Flush();

		//Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4 color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4 color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4 color, int entityID = -1);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, const float zRotation, const glm::vec4 color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, const float zRotation, const glm::vec4 color);
		//Circle
		static void DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness = 1.0f, float fade = 0.0005f, int entityID = -1);

		//DrawSprite
		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);

		//Draw collider
		static void DrawColliderBox(const glm::mat4& transform, const glm::vec2& offset, const glm::vec2& halfExtents, const glm::vec4& color = { 0, 1, 0, 1 }, int entityID = -1, float rotation = 0.0f);
		static void DrawColliderCircle(const glm::mat4& transform, const glm::vec2& offset, float radius, const glm::vec4& color = { 0, 0, 1, 1 }, int entityID = -1);

		//Draw Raycast
		static void DrawRaycast(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color = { 1, 0, 0, 1 }, int entityID = -1);

		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			uint32_t GetTotalVertexCount() { return QuadCount * 4; };
			uint32_t GetTotalIndexCount() { return QuadCount * 6; };
		};

		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}