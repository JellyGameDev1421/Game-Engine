#include "DemoEngine_PCH.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"
#include "Renderer/2D/Renderer2D.h"
#include "Editor/EditorLayer.h"
#include "Network/Client.h"
#include "Network/Server.h"

namespace DemoEngine 
{
	//Networking
	Ref<ClientClass> m_Client;
	Ref<ServerClass> m_Server;
	bool m_IsServer = false;

	//Audio

	Scene::Scene(const std::string& name, bool isEditorScene) : m_Name(name), m_IsEditorScene(isEditorScene)
	{
		RegisterComponentHandler<CameraComponent>([](Entity entity, CameraComponent& componenet)
			{
				LOG_INFO("Camera Componenet Added");
				if (auto* scenePtr = entity.GetScene())
				{
					componenet.camera.SetViewportSize(scenePtr->GetViewportWidth(), scenePtr->GetViewportHeight());
				}
			});
	}

	Scene::~Scene() {}
	
	Ref<Scene> Scene::Copy(Ref<Scene>other) 
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		CopyTo(other, newScene);
		return newScene;
	}

	void Scene::CopyTo(Ref<Scene> source, Ref<Scene> destination) 
	{
		if (destination == nullptr) destination = CreateRef<Scene>();

		destination->m_ViewportHeight = source->m_ViewportHeight;
		destination->m_ViewportWidth = source->m_ViewportWidth;

		destination->m_SceneID = source->m_SceneID;

		std::unordered_map<UUID, entt::entity> enttMap;

		auto& srcSceneRegistry = source->m_Registry;
		auto& dstSceneRegistry = destination->m_Registry;

		auto idView = srcSceneRegistry.view<IDComponent>();
		
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& tag = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity en = destination->CreateEntityWithID(uuid, tag);

			enttMap[uuid] = en.m_EntityHandle;
		}

		// More copying stuff
		CopyComponent<TagComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<RigidBody2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
		CopyComponent<CircleCollider2DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

		destination->m_IsEditorScene = false;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithID(UUID(), name);
	}

	Entity Scene::CreateEntityWithID(UUID uuid, const std::string& name)
	{
		auto entity = Entity{ m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	Entity Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity;
		if (entity.HasComponent<TagComponent>()) 
		{
			newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);
		}
		else 
		{
			newEntity = CreateEntity();
		}

		// Copying components add if you add extra components
		CopyComponentIfExists<TransformComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CameraComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<SpriteRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CircleRendererComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<RigidBody2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<BoxCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);
		CopyComponentIfExists<CircleCollider2DComponent>(newEntity.m_EntityHandle, m_Registry, entity);

		return newEntity;
	}

	void Scene::DestroyEntity(Entity entity) 
	{
		// Die entities
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		// Draw Sprites (Quads)
		{
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}
		// Circles
		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}
		// I wanna see em in the editor
		{
			auto view = m_Registry.view < TransformComponent, BoxCollider2DComponent>();
			for (auto entity : view)
			{
				auto [transform, BoxCollider2D] = view.get<TransformComponent, BoxCollider2DComponent>(entity);
				Renderer2D::DrawColliderBox(transform.GetTransform(), BoxCollider2D.Offset, BoxCollider2D.HalfExtents, BoxCollider2D.color, (int)entity, BoxCollider2D.Rotation);
			}
		}

		// I wish to see the circle collider
		{
			auto view = m_Registry.view < TransformComponent, CircleCollider2DComponent>();
			for (auto entity : view)
			{
				auto [transform, CircleCollider2D] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
				Renderer2D::DrawColliderCircle(transform.GetTransform(), CircleCollider2D.Offset, CircleCollider2D.Radius, CircleCollider2D.color, (int)entity);
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		//Making the physics world
		b2World_Step(worldID, (float)ts, 4);

		auto& view = m_Registry.view<TransformComponent, RigidBody2DComponent>();
		for (auto& entity : view)
		{
			auto& [transform, Rigidbody2D] = view.get<TransformComponent, RigidBody2DComponent>(entity);
			if (Rigidbody2D.getBodyType() != RigidBody2DComponent::BodyType::Static)
			{
				// Positions only run when simulation is running 
				b2Vec2 newPosition = b2Body_GetPosition(Rigidbody2D.rbBody);
				b2Rot newRotation = b2Body_GetRotation(Rigidbody2D.rbBody);

				//Rotations and positions

				//transform.Translation = { newPosition.x, newPosition.y, transform.Translation.z };
				
				transform.Translation.x = newPosition.x;
				transform.Translation.y = newPosition.y;
				
				//Now rotiations are actually fixed :()
				if (!Rigidbody2D.fixedRotation)
					transform.Rotation.z = b2Rot_GetAngle(newRotation);

				Rigidbody2D.hasGravity = false;
			}
		}

		// Attempting player movement
		{
			Entity player;

			// Find player by tag
			auto view = m_Registry.view<TagComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto& tag = view.get<TagComponent>(entity);
				if (tag.Tag == "Player")
				{
					player = Entity{ entity, this };
					break;
				}
			}

			if (player)
			{
				auto& rb = player.GetComponent<RigidBody2DComponent>();

				const float speed = 2.5f;
				b2Vec2 desiredVelocity = { 0.0f, 0.0f };

				//WASD movement goes of movement speed
				if (Input::IsKeyPressed(Key::W)) desiredVelocity.y += speed;
				if (Input::IsKeyPressed(Key::S)) desiredVelocity.y -= speed;
				if (Input::IsKeyPressed(Key::A)) desiredVelocity.x -= speed;
				if (Input::IsKeyPressed(Key::D)) desiredVelocity.x += speed;

				// Set the velocity directly on the physics body
				b2Body_SetLinearVelocity(rb.rbBody, desiredVelocity);
			}
		}

		//Attempting to make the Enemy's follow the player now
		{
			glm::vec2 playerPosition;
			bool playerFound = false;

			// Find player
			{
				auto view = m_Registry.view<TagComponent, TransformComponent>();
				for (auto entity : view)
				{
					auto& tag = view.get<TagComponent>(entity);
					if (tag.Tag == "Player")
					{
						playerPosition = view.get<TransformComponent>(entity).Translation;
						playerFound = true;
						break;
					}
				}
			}

			// Move enemies toward player
			if (playerFound)
			{
				auto enemies = m_Registry.view<TagComponent, TransformComponent, RigidBody2DComponent>();
				for (auto entity : enemies)
				{
					auto& tag = enemies.get<TagComponent>(entity);
					
					// This is here because i wanted to make multiple "Enemy" types but add numbers are the end to differentiate them right
					// Found this function that finds strings that start with the string and looks for them instead of doing a bunch of || || || 
					if (tag.Tag.rfind("Enemy", 0) == 0) // tag starts with "Enemy"
					{
						auto& transform = enemies.get<TransformComponent>(entity);
						auto& rb = enemies.get<RigidBody2DComponent>(entity);

						glm::vec2 enemyPos = { transform.Translation.x, transform.Translation.y };
						// Get player pos and - to make them move towards the player similar to unity logic
						glm::vec2 direction = glm::normalize(playerPosition - enemyPos);

						float speed = 2.0f; // tune this
						b2Vec2 velocity = { direction.x * speed, direction.y * speed };
						b2Body_SetLinearVelocity(rb.rbBody, velocity);
					}
				}
			}
		}


		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			//Render Scene
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			//Draw Sprites (Quads)
			{
				auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : group)
				{
					auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
			} 
			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view) 
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
				}
			}
			{
				auto view = m_Registry.view < TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view) 
				{
					auto [transform, BoxCollider2D] = view.get<TransformComponent, BoxCollider2DComponent>(entity);
					Renderer2D::DrawColliderBox(transform.GetTransform(), BoxCollider2D.Offset, BoxCollider2D.HalfExtents, BoxCollider2D.color, (int)entity, BoxCollider2D.Rotation);
				}
			}

			{
				auto view = m_Registry.view < TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [transform, CircleCollider2D] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
					Renderer2D::DrawColliderCircle(transform.GetTransform(), CircleCollider2D.Offset, CircleCollider2D.Radius, CircleCollider2D.color, (int)entity);
				}
			}

			//Visualise raycast way to buggy and can't see it not working at all
			RaycastResult result = Raycast(glm::vec2(0, 0), glm::normalize(glm::vec2(1, 1)), 10.0f);
			if (result.hit)
			{
				glm::vec2 endPoint = result.point;
				
				//Buggy not working the best
				//Renderer2D::DrawRaycast(glm::vec2(0, 0), endPoint, { 1, 0, 0, 1 });

				//Previous thoughts move logic into editorlayer 
				//Entity hitEntity = FindEntityByUUID(result.hitEntity);

				//if (hitEntity && hitEntity.HasComponent<TagComponent>()) 
				//{
				//	std::string tag = hitEntity.GetComponent<TagComponent>().Tag;

				//	if (tag == "Enemy" || tag == "Enemy1") 
				//	{
				//		LOG_INFO("RAYCAST HAS HIT ENEMY! ", tag);
				//		DestroyEntity(hitEntity);
				//	}
				//}
			}
			 
			Renderer2D::EndScene();
		}

		//Networking stuff
		if (m_IsServer && m_Server)
		{
			m_Server->ConnectToClient();
		}
		else if (m_Client)
		{
			m_Client->ReceiveData();
		}

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		//Resize our non-fixed aspect ratio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.fixedAspectRatio == false)
			{
				//Resize the camera
				cameraComponent.camera.SetViewportSize(width, height);
			}
		}
	}

	void Scene::OnRuntimeStart()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = { 0.0f, -10.0f };
		worldID = b2CreateWorld(&worldDef);


		auto view = m_Registry.view<TransformComponent, RigidBody2DComponent>();
		for (auto entity : view)
		{
			auto [transform, Rigidbody2D] = view.get<TransformComponent, RigidBody2DComponent>(entity);

			// Create the body once, regardless of collider type
			// Assigning correct variabled to rbDef
			b2BodyDef rbDef = b2DefaultBodyDef();
			rbDef.position = { transform.Translation.x, transform.Translation.y };
			rbDef.rotation = b2MakeRot(transform.Rotation.z);
			rbDef.type = (b2BodyType)Rigidbody2D.m_Type;
			rbDef.fixedRotation = Rigidbody2D.fixedRotation;
			rbDef.gravityScale = Rigidbody2D.hasGravity;

			b2BodyId rbID = b2CreateBody(worldID, &rbDef);
			Rigidbody2D.rbBody = rbID;

			// Box collider logic
			if (Entity{ entity, this }.HasComponent<BoxCollider2DComponent>())
			{
				auto& box = Entity{ entity, this }.GetComponent<BoxCollider2DComponent>();

				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = box.Density;
				shapeDef.friction = box.Friction;
				shapeDef.restitution = box.Restitution;

				b2Polygon dynamicBox = b2MakeBox(transform.Scale.x * 0.5f, transform.Scale.y * 0.5f);
				box.physicsShapeID = b2CreatePolygonShape(rbID, &shapeDef, &dynamicBox);
			}

			// Circle collider logic
			if (Entity{ entity, this }.HasComponent<CircleCollider2DComponent>())
			{
				auto& circle = Entity{ entity, this }.GetComponent<CircleCollider2DComponent>();

				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = circle.Density;
				shapeDef.friction = circle.Friction;
				shapeDef.restitution = circle.Restitution;

				float worldRadius = circle.Radius * glm::max(transform.Scale.x, transform.Scale.y);

				b2Circle circleShape;
				circleShape.center = { circle.Offset.x, circle.Offset.y };
				circleShape.radius = worldRadius;

				circle.physicsShapeID = b2CreateCircleShape(rbID, &shapeDef, &circleShape);
			}

			//Networking stuff
			m_IsServer = true; // or false for client

			//Create server or connect if its a client
			if (m_IsServer)
			{
				m_Server = CreateRef<ServerClass>();
			}
			else
			{
				m_Client = CreateRef<ClientClass>();
				m_Client->ConnectClient();
			}

		}
	}

	void Scene::OnRunTimeStop()
	{
		//Cleaning up phsycis world
		b2DestroyWorld(worldID);
		worldID = b2_nullWorldId;
	}

	//Raycast stuff
	RaycastResult Scene::Raycast(glm::vec2 origin, glm::vec2 direction, float maxDistance)
	{
		RaycastResult result;

		//Called by Box2D whenever a shape is hit during raycast
		auto callback = [](b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context) -> float
			{
				//LOG_INFO("Hit detected in Box2D callback!");

				auto* ctx = static_cast<std::pair<Scene*, RaycastResult*>*>(context);
				Scene* scene = ctx->first;
				RaycastResult* out = ctx->second;

				// Marking hit and storing the data 
				out->hit = true;
				out->point = { point.x, point.y }; // World point
				out->normal = { normal.x, normal.y }; // Surface normal hit
				out->distance = fraction; // Where the hit occured

				// Find what entity got hit
				b2BodyId body = b2Shape_GetBody(shapeId);
				for (auto entity : scene->m_Registry.view<RigidBody2DComponent>())
				{			
					// If this entity's body matches the hit one, store its UUID
					auto& rb = scene->m_Registry.get<RigidBody2DComponent>(entity);
					if (B2_ID_EQUALS(rb.rbBody, body))
					{
						out->hitEntity = scene->m_Registry.get<IDComponent>(entity).ID;
						break;
					}
				}
				return fraction; // short-circuit after closest hit
			};

		// Perform raycast getting world position to mouse
		// b2World_CastRay(worldID, b2Origin, b2Translation, filter, RayCallback, &context);
		b2Vec2 b2Origin = { origin.x, origin.y };
		b2Vec2 b2Target = b2Origin + b2Vec2{ direction.x, direction.y } *maxDistance;

		std::pair<Scene*, RaycastResult*> context = { this, &result };
		b2QueryFilter filter = b2DefaultQueryFilter();

		// Perform the actual raycast in Box2D
		b2World_CastRay(worldID, b2Origin, b2Target, filter, callback, &context);

		return result;
	}

	// Finding entites names through UUID (Names)
	Entity Scene::FindEntityByUUID(UUID uuid)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entity : view)
		{
			if (m_Registry.get<IDComponent>(entity).ID == uuid)
				return Entity{ entity, this };
		}

		return Entity{};
	}
}