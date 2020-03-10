#pragma once
//#include <GLM/glm.hpp> 
#include <glad/glad.h>
#include <unordered_map>
#include <memory>
#include "Shader.h"
#include "Texture2D.h"

/*    Represents settings for a shader */
class Material {
public:
	typedef std::shared_ptr<Material> Sptr;
	Material(const Shader::Sptr& shader) {
		myShader = shader;
	}
	virtual ~Material() = default;
	const Shader::Sptr& GetShader() const {
		return myShader;
	}
	virtual void Apply();
	void Set(const std::string& name, const glm::mat4& value) {
		myMat4s[name] = value;
	}
	void Set(const std::string& name, const glm::vec4& value) {
		myVec4s[name] = value;
	}
	void Set(const std::string& name, const glm::vec3& value) {
		myVec3s[name] = value;
	}
	void Set(const std::string& name, const float& value) {
		myFloats[name] = value;
	}
	void Set(const std::string& name, const glm::ivec2& value) {
		myIvec2[name] = value;
	}
	void Set2(const std::string& name, const glm::mat4& value) {
		myMat4s[name] = value;
	}
	void Set2(const std::string& name, const glm::vec4& value) {
		myVec4s[name] = value;
	}
	void Set2(const std::string& name, const glm::vec3& value) {
		myVec3s[name] = value;
	}
	void Set2(const std::string& name, const float& value) {
		myFloats[name] = value;
	}
	// New in tutorial 08   
	void Set(const std::string& name, const Texture2D::Sptr& value) { myTextures[name] = value; }

protected:
	Shader::Sptr myShader;
	std::unordered_map<std::string, glm::mat4> myMat4s;
	std::unordered_map<std::string, glm::vec4> myVec4s;
	std::unordered_map<std::string, glm::vec3> myVec3s;
	std::unordered_map<std::string, glm::vec2> myVec2s;
	std::unordered_map<std::string, glm::ivec2> myIvec2;
	std::unordered_map<std::string, float> myFloats;
	// New in tutorial 08    
	std::unordered_map<std::string, Texture2D::Sptr> myTextures;
};