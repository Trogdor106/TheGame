#pragma once

#include "IGraphicsResource.h"
#include "EnumToString.h"
#include "glad/glad.h"
#include "Texture2D.h"
#include <unordered_map>
#include "ITexture.h"
#include "EnumToString.h"
#include "glad/glad.h"
#include "Logging.h"
#include <glm/glm.hpp>
#include "Vertex.h"
#include "TextureEnums.h"

ENUM(ShaderStageType, uint32_t,
	VertexShader = 0,
	FragmentShader = 1,
	TessControl = 2,
	TessEval = 3,
	Geometry = 4
);

constexpr uint32_t ToGlEnum(ShaderStageType type);

class Shader : public IGraphicsResource
{
public:
	GraphicsClass(Shader);

	Shader();
	virtual ~Shader();

	/*
	 * Compiles a given shader part and attaches it to this shader
	 * @param type The shader type of stage to load (vertex, fragment)
	 * @param source The source code of the shader stage
	 */
	void CompilePart(ShaderStageType type, const std::string& source);
	/*
	 * Compiles a given shader part and attaches it to this shader
	 * @param type The shader type of stage to load (vertex, fragment)
	 * @param filePath The path to a file containing the source code of the shader
	 */
	void LoadPart(ShaderStageType type, const std::string& filePath);
	/*
	 * Loads a spir-v shader stage into this program
	 * @param type The shader type of stage to load (vertex, fragment)
	 * @param binary The compiled SPIR-V binary to load
	 * @param length The total length of the SPIR-V binary blob
	 * @param entryPoint The name of the function to use as the entry point for this stage
	 */
	void LoadSpirv(ShaderStageType type, const char* binary, size_t length, const std::string& entryPoint);
	/*
	 * Links the stages of the shader, and performs final introspection. You must link the shader before usage
	 */
	void Link();

	/*
	 * Checks whether this shader has a uniform by the given name
	 * @param name The name of the uniform to look for
	 * @returns True if there is a uniform by that name
	 */
	bool HasUniform(const std::string& name) const;
	/*
	 * Gets the data type of the given uniform
	 * @param name The name of the uniform to look for
	 * @returns The type of uniform, or ShaderDataType::None if the uniform does not exist
	 */
	ShaderDataType GetUniformType(const std::string& name) const;
	/*
	 * Gets the array size of the given uniform
	 * @param name The name of the uniform to look for
	 * @returns The array size of the uniform, or 0 if the uniform does not exist
	 */
	int GetUniformArraySize(const std::string& name) const;
	/*
	 * Gets the uniform location of the given uniform
	 * @param name The name of the uniform to look for
	 * @returns The location of the uniform, or -1 if the uniform does not exist
	 */
	int GetUniformLocation(const std::string& name) const;

	/*
	 * Sets a uniform value in the shader
	 * @param <T> The type of the uniform to set
	 * @param name The name of the uniform to set
	 * @param value The value to write to the uniform
	 */
	template <typename T>
	void SetUniform(const std::string& name, const T& value) {
		auto it = myUniforms.find(name);
		if (it != myUniforms.end()) {
			SetUniform(it->second.Location, value);
		}
	}

	/*
	 * Sets an array of uniform values in the shader
	 * @param <T> The type of the uniform to set
	 * @param name The name of the array to set
	 * @param count The number of elements to set in the array
	 * @param values The values to write to the uniform
	 */
	template <typename T>
	void SetUniforms(const std::string& name, int count, const T* values) {
		auto it = myUniforms.find(name);
		if (it != myUniforms.end()) {
			LOG_ASSERT(it->second.ArraySize > 1, "Uniform is not an array!");
			LOG_ASSERT(count <= it->second.ArraySize, "Attempting to send too many elements to an array!");
			SetUniforms(it->second.Location, count, values);
		}
	}

	void SetUniform(int loc, const glm::mat4& value);
	void SetUniform(int loc, const glm::mat3& value);
	void SetUniform(int loc, const glm::vec2& value);
	void SetUniform(int loc, const glm::ivec2& value); // NEW
	void SetUniform(int loc, const glm::vec3& value);
	void SetUniform(int loc, const glm::vec4& value);
	void SetUniform(int loc, const float& value);
	void SetUniform(int loc, const int& value);
	void SetUniform(int loc, const uint64_t& value);

	void SetUniforms(int loc, int count, const glm::mat4* value);
	void SetUniforms(int loc, int count, const glm::mat3* value);
	void SetUniforms(int loc, int count, const glm::vec2* value);
	void SetUniforms(int loc, int count, const glm::vec3* values);
	void SetUniforms(int loc, int count, const glm::vec4* values);
	void SetUniforms(int loc, int count, const float* values);
	void SetUniforms(int loc, int count, const int* values);
	void SetUniforms(int loc, int count, const uint64_t* values);



	/*
	 * Binds this shader program for use in rendering
	 */
	void Use() const;

protected:

	uint32_t myStages[5];
	bool     isLinked;

	struct UniformInfo {
		std::string    Name;
		int            Location;
		ShaderDataType Type;
		int            ArraySize;
	};
	struct UniformBlockInfo
	{
		std::string Name;
		int         Binding;
		int         SizeInBytes;
		int         NumVariables;
		int         BlockIndex;

		std::vector<UniformInfo> SubUniforms;
	};

	std::unordered_map<std::string, UniformInfo>      myUniforms;
	std::unordered_map<std::string, UniformBlockInfo> myUniformBlocks;

	// Allows the shader to perform introspection (ie, looking at it's own state)
	void __Introspect();
	// Determines what uniforms are available in this shader
	void __IntrospectUniforms();
	// Determines what uniform blocks are available in this shader
	void __IntrospectUniformBlocks();

	/*
	 * Checks the compile status of the given shader stage
	 * @param shaderHandle The handle of the shader to check the compile status of
	 * @returns True if the shader has compiled successfully, false if otherwise
	 */
	static bool __CheckCompileStatus(uint32_t shaderHandle);

public:


		typedef std::shared_ptr<Shader> Sptr;
		

		void Compile(const char* vs_source, const char* fs_source);

		// Loads a shader program from 2 files. vsFile is the path to the vertex shader, and fsFile is
		// the path to the fragment shader
		void Load(const char* vsFile, const char* fsFile);

		void SetUniform(const char* name, const glm::mat3& value);
		void SetUniform(const char* name, const glm::ivec2& value);
		void SetUniform(const char* name, const glm::vec3& value);
		void SetUniform(const char* name, const float& value);
		void SetUniform(const char* name, const int& value);

		void SetUniform(const char* name, const glm::mat4& value);

		void Bind();

		GLuint getVS();
		GLuint getFS();

private:
	GLuint __CompileShaderPart(const char* source, GLenum type);

	GLuint myShaderHandle;
	GLuint vs;
	GLuint fs;
};

