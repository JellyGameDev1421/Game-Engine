#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/gtx/quaternion.hpp"

#include "Core/UUID.h"
#include "SceneCamera.h"
#include "Box2D/collision.h"
#include "Box2D/box2d.h"

namespace DemoEngine 
{
	struct IDComponent
	{
		UUID ID = 0;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent 
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag_) : Tag(tag_) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
		
		bool isModified = false;
		
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}

		glm::mat4 GetTransform() const 
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool Primary = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent(ProjectionType t) 
		{
			camera.SetProjectionType(t);
		}

		operator SceneCamera& () { return camera; };
		operator const SceneCamera& () const { return camera; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Colour{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& colour) : Colour(colour) {}
	};

	struct CircleRendererComponent 
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Radius = 0.5f;
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	struct RigidBody2DComponent
	{
		static enum class BodyType 
		{
			Static = 0,  Kinematic = 1, Dynamic = 2
		};

		BodyType getBodyType()
		{
			return m_Type;
		}

		BodyType setBodyType(BodyType type) 
		{
			m_Type = type;
			return m_Type;
		}

		BodyType m_Type = BodyType::Dynamic;

		//Adding mass and stuff
		float mass = 1.0f;
		glm::vec2 CentreMass = { 0.0f, 0.0f };
		float RotationalInertia = 1.0f;

		bool fixedRotation = false;    // Prevents rotation if true
		UUID physicsBodyID = 0;        // Reference to the physics engine body
		bool hasGravity = true;		   // For top down movement I turned off gravity
		b2BodyId rbBody;
	};

	struct BoxCollider2DComponent
	{
		glm::vec4 color = { 0, 1, 0, 0.4f };

		glm::vec2 Offset = { 0.0f, 0.0f };  // Offset from the entity's transform
		glm::vec2 HalfExtents = { 0.5f, 0.5f };  // Half-width and half-height

		float Density = 1.0f;      // Mass density
		float Friction = 0.5f;     // Resistance to movement
		float Restitution = 0.0f;  // Bounciness

		//Add Rotation
		float Rotation = 0.0f;

		//UUID physicsShapeID = 0; // Stores the Box2D shape ID (runtime only)
		b2ShapeId physicsShapeID = b2_nullShapeId; // instead of UUID

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
		BoxCollider2DComponent(const glm::vec2& offset, const glm::vec2& extents)
			: Offset(offset), HalfExtents(extents) {}
	};

	struct CircleCollider2DComponent
	{
		glm::vec4 color = { 0, 1, 0, 0.4f };

		glm::vec2 Offset = { 0.0f, 0.0f };  // Offset from the entity's transform
		float Radius = 0.5f;              // Radius of the circle

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;

		//UUID physicsShapeID = 0; // Stores the Box2D shape ID (runtime only)
		b2ShapeId physicsShapeID = b2_nullShapeId; // instead of UUID

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
		CircleCollider2DComponent(const glm::vec2& offset, float radius)
			: Offset(offset), Radius(radius) {}
	};

	struct RaycastResult
	{
		bool hit = false;
		UUID hitEntity;             // Entity hit
		glm::vec2 point = { 0, 0 }; // Point of collision
		glm::vec2 normal = { 0, 0 }; // Surface normal
		float distance = 0.0f;       // Distance from ray origin
	};
}

