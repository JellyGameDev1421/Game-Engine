#include "DemoEngine_PCH.h"
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <Glad/glad.h>
#include <fstream>
#include <filesystem>

namespace DemoEngine 
{
	static GLenum ShaderTypeFromString(const std::string& type) 
	{
		if (type == "vertex") { return GL_VERTEX_SHADER; }
		if (type == "fragment" || type == "pixel") { return GL_FRAGMENT_SHADER; }

		CORE_ASSERT(false, "Unknown Shader Type");
		return 0;
	}

	Shader::Shader(const std::string& filepath) 
	{
		std::string source = Shader::readFile(filepath);

		auto shaderSources = Shader::PreProcess(source);

		Compile(shaderSources);

		const std::filesystem::path pathname = filepath;
		m_Name = pathname.stem().string();
	}

	Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_Name(name) 
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		Compile(sources);
	}

	Shader::~Shader() 
	{
		glDeleteProgram(m_RendererID);
	}

	std::string Shader::readFile(const std::string& filepath) 
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);

		if (in) 
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else 
		{
			LOG_ERROR("Could not open file ", filepath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string& source) 
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos) 
		{
			size_t eol = source.find_first_of("\r\n", pos);
			CORE_ASSERT(eol != std::string::npos, "Systax Error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			CORE_ASSERT(ShaderTypeFromString(type), "Invalid Shader Type Specific");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}
		return shaderSources;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources) 
	{
		GLuint program = glCreateProgram();
		CORE_ASSERT(shaderSources.size() <= 2, "Shader array is not sufficiently for the number of shaders provided");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources) 
		{
			GLenum ShaderType = kv.first;
			const std::string& shaderSource = kv.second;

			GLuint shader = glCreateShader(ShaderType);

			const GLchar* source = shaderSource.c_str();
			glShaderSource(shader, 1, &source, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

			if (isCompiled == GL_FALSE) 
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				CORE_ASSERT(false, "Shader compile error! Type: {0}\n{1}", ShaderType, infoLog.data());
				LOG_ERROR("{0}", infoLog.data());
				break;
			}
			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram(program);

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto id : glShaderIDs)
			{
				glDeleteShader(id);
			}

			CORE_ASSERT(false, "Shader Linking Failiure Error! ");
			LOG_ERROR("{0}", infoLog.data());
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
		}

		m_RendererID = program;
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::UnBind() const
	{
		glUseProgram(0);
	}

	void Shader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void Shader::SetIntArray(const std::string& name, int* value, uint32_t count)
	{
		UploadUniformIntarray(name, value, count);
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		UploadUniformFloat2(name, value);
	}

	void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(u_Location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(u_Location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformInt(const std::string& name, const int value)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(u_Location, value);
	}

	void Shader::UploadUniformIntarray(const std::string& name, const int* values, const uint32_t count)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(u_Location, count, values);
	}

	void Shader::UploadUniformFloat(const std::string& name, float value) 
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(u_Location, value);
	}

	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2f(u_Location, value.x, value.y);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3f(u_Location, value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint u_Location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4f(u_Location, value.x, value.y, value.z, value.w);
	}
}

