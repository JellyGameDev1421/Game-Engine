#include "DemoEngine_PCH.h"
#include "SceneSerialiser.h"
#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Utils/YamlConverter.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace DemoEngine 
{
	SceneSerialiser::SceneSerialiser(const Ref<Scene>& scene) : m_Scene(scene) {}

	static void SerialiseEntity(YAML::Emitter& out, Entity entity) 
	{
		CORE_ASSERT("Entity does not have ID component", entity.HasComponent<IDComponent>());
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>()) 
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tr = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tr.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tr.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tr.Scale;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNear();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFar();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNear();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFar();

			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent <SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Colour" << YAML::Value << spriteRendererComponent.Colour;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>()) 
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRenderer = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRenderer.Color;
			out << YAML::Key << "Radius" << YAML::Value << circleRenderer.Radius;
			out << YAML::Key << "Thickness" << YAML::Value << circleRenderer.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRenderer.Fade;

			out << YAML::EndMap;
		}

		if (entity.HasComponent <RigidBody2DComponent>()) 
		{
			out << YAML::Key << "RigidBody2DComponent";
			out << YAML::BeginMap;

			auto& rb = entity.GetComponent<RigidBody2DComponent>();
			out << YAML::Key << "m_Type" << YAML::Value << (int)rb.m_Type;
			out << YAML::Key << "fixedRotation" << YAML::Value << rb.fixedRotation;
			out << YAML::Key << "mass" << YAML::Value << rb.mass;
			out << YAML::Key << "CentreMass" << YAML::Value << rb.CentreMass;
			out << YAML::Key << "RotationalInertia" << YAML::Value << rb.RotationalInertia;
			out << YAML::Key << "Gravity" << YAML::Value << rb.hasGravity;
			out << YAML::EndMap;
		}

		if (entity.HasComponent <BoxCollider2DComponent>()) 
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& boxColl = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxColl.Offset;
			out << YAML::Key << "HalfExtents" << YAML::Value << boxColl.HalfExtents;
			out << YAML::Key << "Density" << YAML::Value << boxColl.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxColl.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxColl.Restitution;
			out << YAML::EndMap;
		}

		//Add circle collider
		if (entity.HasComponent <CircleCollider2DComponent>()) 
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& circColl = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << circColl.Offset;
			out << YAML::Key << "Radius" << YAML::Value << circColl.Radius;
			out << YAML::Key << "Density" << YAML::Value << circColl.Density;
			out << YAML::Key << "Friction" << YAML::Value << circColl.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circColl.Restitution;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;

	}

	void SceneSerialiser::Serialise(const std::string& filePath) 
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Untitled Scene";

		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		auto allEntities = m_Scene->m_Registry.view<entt::entity>();
		for (auto entityID : allEntities)
		{
			Entity currentEntity = { entityID, m_Scene.get() };
			if (!currentEntity)return;
			SerialiseEntity(out, currentEntity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	bool SceneSerialiser::Deserialise(const std::string& filePath) 
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data;

		try 
		{
			data = YAML::Load(strStream.str());
		}
		catch (YAML::ParserException e) 
		{
			return false;
		}

		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		LOG_TRACE("Deserializing Scene '{0}'", sceneName);

		auto entities = data["Entities"];

		if (entities)
		{
			for (auto entity : entities) 
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;

				auto tagComponent = entity["TagComponent"];
				if (tagComponent) 
				{
					name = tagComponent["Tag"].as<std::string>();
				}

				LOG_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntityWithID(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) 
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				
				if (cameraComponent) 
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
				
					auto& cameraProps = cameraComponent["Camera"];
					cc.camera.SetProjectionType((ProjectionType)cameraProps["ProjectionType"].as<int>());
				
					cc.camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera.SetPerspectiveNear(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.SetPerspectiveFar(cameraProps["PerspectiveFar"].as<float>());
				
					cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.SetOrthographicNear(cameraProps["OrthographicNear"].as<float>());
					cc.camera.SetOrthographicFar(cameraProps["OrthographicFar"].as<float>());
				
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
				
				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) 
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Colour = spriteRendererComponent["Colour"].as<glm::vec4>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent) 
				{
					auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
					src.Color = circleRendererComponent["Color"].as<glm::vec4>();
					src.Radius = circleRendererComponent["Radius"].as<float>();
					src.Thickness = circleRendererComponent["Thickness"].as<float>();
					src.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto rigidbody2D = entity["RigidBody2DComponent"];
				if (rigidbody2D) 
				{
					auto& src = deserializedEntity.AddComponent<RigidBody2DComponent>();
					src.m_Type = (RigidBody2DComponent::BodyType)rigidbody2D["m_Type"].as<int>();
					src.fixedRotation = rigidbody2D["fixedRotation"].as<bool>();
					src.hasGravity = rigidbody2D["Gravity"].as<bool>();
				}

				auto boxColl = entity["BoxCollider2DComponent"];
				if (boxColl) 
				{
					auto& src = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					src.Offset = boxColl["Offset"].as<glm::vec2>();
					src.HalfExtents = boxColl["HalfExtents"].as<glm::vec2>();
					src.Density = boxColl["Density"].as<float>();
					src.Friction = boxColl["Friction"].as<float>();
					src.Restitution = boxColl["Restitution"].as<float>();
				}

				auto circleColl = entity["CircleCollider2DComponent"];
				if (circleColl) 
				{
					auto& src = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					src.Offset = circleColl["Offset"].as<glm::vec2>();
					src.Radius = circleColl["Radius"].as<float>();
					src.Density = circleColl["Density"].as<float>();
					src.Friction = circleColl["Friction"].as<float>();
					src.Restitution = circleColl["Restitution"].as<float>();
				}
			}
			return true;
		}
	}
}