#ifndef SHADER_H
#define SHADER_H


#include <vector>


#include "Neko.h"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Shader
{
public:
	Shader() = delete;
	Shader(const char* vertexShader, const char* fragmentShader);
	~Shader();

	void AddShader(const char* shaderFilePath, u32 shaderType);

	void Link();

	inline void Use() const { glUseProgram(m_Id); }
	inline void Unuse() const { glUseProgram(0); }

	inline void SetFloat(const char* location, float val) const { glUniform1f(glGetUniformLocation(m_Id, location), val); }
	inline void SetInt(const char* location, int val) const { glUniform1i(glGetUniformLocation(m_Id, location), val); }
	inline void SetVec3(const char* location, const glm::vec3& v) const { glUniform3fv(glGetUniformLocation(m_Id, location), 1, &v[0]); }
	inline void SetMat4(const char* location, const glm::mat4& m) const { 
		glUniformMatrix4fv(glGetUniformLocation(m_Id, location), 1, GL_FALSE, &m[0][0]); }
	inline void SetU64(const char* location, u64 val) const { glUniformHandleui64ARB(glGetUniformLocation(m_Id, location), val); }

private:
	u32 m_Id;
	std::vector<u32> m_Shaders{};

	u32 createShader(const char* shaderPath, u32 shaderType) const;
};



#endif // !SHADER_H

