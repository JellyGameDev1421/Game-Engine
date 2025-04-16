#pragma once
#include "Scene.h"
#include "entt.hpp"

namespace DemoEngine 
{
	class Entity 
	{
	public:
		Entity() = default;
		Entity(entt::entity handle_, Scene* scene_);
		Entity(const Entity& other) = default;

		template <typename T, typename... Args>
		T& AddComponent(Args&&... args) 
		{
			CORE_ASSERT(!HasComponent<T>(), "Entity Already has component! ");
			T& componenet = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, componenet);
			return componenet;
		}

		template <typename T>
		T& GetComponent() 
		{
			CORE_ASSERT(HasComponent<T>(), "Entity does not have component! ");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template <typename T>
		T& GetComponent() const 
		{
			CORE_ASSERT(HasComponent<T>(), "Entity Does not have the component! ");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template <typename T>
		bool HasComponent() 
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template <typename T>
		bool HasComponent() const 
		{
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template <typename T>
		void RemoveComponent() 
		{
			CORE_ASSERT(HasComponent<T>(), "Entity does not have a component! ");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }
		operator bool() const { return m_EntityHandle != entt::null; }

		bool operator == (const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator != (const Entity& other) const { return !(*this == other); }

		Scene* GetScene() { return m_Scene; }
		UUID GetUUID() const { return GetComponent<IDComponent>().ID; }
		UUID GetSceneUUID() const;

	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;

		friend class Scene;
		friend class SceneSerialiser;
	};
}