#include "DemoEngine_PCH.h"
#include "Entity.h"

namespace DemoEngine 
{
	Entity::Entity(entt::entity handle_, Scene* scene_) : m_EntityHandle(handle_), m_Scene(scene_) {}

	UUID Entity::GetSceneUUID() const 
	{
		return m_Scene->m_SceneID;
	}
}