#include "DemoEngine_PCH.h"
#include "ShaderLibary.h"
#include "Shader.h"

namespace DemoEngine 
{
	void ShaderLibary::Add(const std::string& name, const Ref<Shader>& shader) 
	{
		CORE_ASSERT(!Exists(name), "Shader Already Exists");
		m_Shaders[name] = shader;
	}

	void ShaderLibary::Add(const Ref<Shader>& shader) 
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibary::Load(const std::string& filepath) 
	{
		Ref<Shader> shader = std::make_shared<Shader>(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibary::Load(const std::string& name, const std::string& filepath) 
	{
		Ref<Shader> shader = std::make_shared<Shader>(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibary::Get(const std::string& name) 
	{
		CORE_ASSERT(Exists(name), "Shader not found");
		return m_Shaders[name];
	}

	bool ShaderLibary::Exists(const std::string& name) const 
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}

}