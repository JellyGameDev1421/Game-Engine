#include "DemoEngine_PCH.h"
#include "Renderer2D.h"

#include "Renderer/Data/VertexArray.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Data/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer2DData.h"

#include <glad/glad.h>

namespace DemoEngine
{

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float2, "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});

		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		//Circles
		s_Data.CircleVertexArray = VertexArray::Create();

		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
		s_Data.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_WorldPosition" },
			{ ShaderDataType::Float3, "a_LocalPosition" },
			{ ShaderDataType::Float4, "a_Color"			},
			{ ShaderDataType::Float, "a_Thickness"		},
			{ ShaderDataType::Float, "a_Fade"			},
			{ ShaderDataType::Int, "a_EntityID"			}
			});

		s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.CircleVertexArray->SetIndexBuffer(quadIB);
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

		//End of Circle
		s_Data.QuadShader = CreateRef<Shader>("src/Renderer/Shader/Renderer2D_Quad.glsl");
		s_Data.CircleShader = CreateRef<Shader>("src/Renderer/Shader/Renderer2D_Circle.glsl");

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = { 0.5f, 0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f, 0.5f, 0.0f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

		glEnable(GL_DEPTH_TEST);
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer2D::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		//Quads
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		//Circles
		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;
	}

	//Render the batch
	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

			s_Data.QuadShader->Bind();

			s_Data.QuadVertexArray->Bind();
			uint32_t count = s_Data.QuadIndexCount ? s_Data.QuadIndexCount : s_Data.QuadVertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount) 
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

			s_Data.CircleShader->Bind();

			s_Data.CircleVertexArray->Bind();
			uint32_t count = s_Data.CircleIndexCount ? s_Data.CircleIndexCount : s_Data.CircleVertexArray->GetIndexBuffer()->GetCount();
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4 color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4 color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4 color, int entityID)
	{
		constexpr size_t quadVertexCount = 4;

		constexpr glm::vec2 textureCoords[] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f,1.0f} };
		const glm::vec2 tilingFactor = { 1.0f ,1.0f };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr->EntityID = entityID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, const float zRotation, const glm::vec4 color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, zRotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, const float zRotation, const glm::vec4 color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(zRotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness, float fade, int entityID) 
	{
		if (s_Data.CircleIndexCount >= Renderer2DData::MaxIndices) 
		{
			NextBatch();
		}

		for (size_t i = 0; i < 4; i++) 
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = color;
			s_Data.CircleVertexBufferPtr->Thickness = thickness;
			s_Data.CircleVertexBufferPtr->Fade = fade;
			s_Data.CircleVertexBufferPtr->EntityID = entityID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		
		DrawQuad(transform, src.Colour, entityID);
	}

	void Renderer2D::DrawColliderBox(const glm::mat4& transform, const glm::vec2& offset, const glm::vec2& halfExtents,
		const glm::vec4& color, int entityID, float rotation)
	{
		glm::mat4 baseTransform = transform
			* glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.001f))
			* glm::rotate(glm::mat4(1.0f), rotation, { 0.0f, 0.0f, 1.0f });

		glm::vec2 size = halfExtents * 2.0f;

		// Use percentage of shortest side for thickness (visually balanced)
		const float thickness = 0.02f * glm::min(size.x, size.y);

		// Top
		DrawQuad(baseTransform *
			glm::translate(glm::mat4(1.0f), { 0.0f, halfExtents.y - thickness * 0.5f, 0.0f }) *
			glm::scale(glm::mat4(1.0f), { size.x, thickness, 1.0f }),
			color, entityID);

		// Bottom
		DrawQuad(baseTransform *
			glm::translate(glm::mat4(1.0f), { 0.0f, -halfExtents.y + thickness * 0.5f, 0.0f }) *
			glm::scale(glm::mat4(1.0f), { size.x, thickness, 1.0f }),
			color, entityID);

		// Left
		DrawQuad(baseTransform *
			glm::translate(glm::mat4(1.0f), { -halfExtents.x + thickness * 0.5f, 0.0f, 0.0f }) *
			glm::scale(glm::mat4(1.0f), { thickness, size.y - thickness * 2.0f, 1.0f }),
			color, entityID);

		// Right
		DrawQuad(baseTransform *
			glm::translate(glm::mat4(1.0f), { halfExtents.x - thickness * 0.5f, 0.0f, 0.0f }) *
			glm::scale(glm::mat4(1.0f), { thickness, size.y - thickness * 2.0f, 1.0f }),
			color, entityID);
	}




	void Renderer2D::DrawColliderCircle(const glm::mat4& transform, const glm::vec2& offset, float radius, const glm::vec4& color, int entityID)
	{
		float uniformScale = radius * 2.0f;

		glm::mat4 colliderTransform = transform
			* glm::translate(glm::mat4(1.0f), glm::vec3(offset, 0.001f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(uniformScale, uniformScale, 1.0f));

		const float thickness = 0.03f;
		const float fade = 0.0005f;

		DrawCircle(colliderTransform, color, thickness, fade, entityID);
	}

	void Renderer2D::DrawRaycast(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, int entityID)
	{
		glm::vec3 from = glm::vec3(start, 0.05f); // Z-layer for visibility
		glm::vec3 to = glm::vec3(end, 0.05f);

		glm::vec3 dir = to - from;
		float length = glm::length(dir);
		if (length == 0) return;

		glm::vec3 normalized = glm::normalize(dir);
		float angle = std::atan2(normalized.y, normalized.x); // angle in radians

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), from)
			* glm::rotate(glm::mat4(1.0f), angle, { 0, 0, 1 })
			* glm::scale(glm::mat4(1.0f), { length, 0.02f, 1.0f });

		DrawQuad(transform, color, entityID);
	}


	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
}