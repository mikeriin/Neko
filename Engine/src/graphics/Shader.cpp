#include "graphics/Shader.h"

Shader::Shader()
	: m_Id(0)
{ }

Shader::~Shader()
{
	for (const auto& shader : m_Shaders) glDeleteShader(shader);
	glDeleteProgram(m_Id);
}

void Shader::AddShader(const char* shaderFilePath, u32 shaderType)
{
	u32 shader = glCreateShader(shaderType);
	std::string shaderSourceStr = ReadFile(shaderFilePath);
	const char* shaderSource = shaderSourceStr.c_str();
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	int isCompiled{ 0 };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (!isCompiled)
	{
		int maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = new char[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

		std::cerr << "SHADER::ERROR::glCompileShader\n" << infoLog << "\n";
		delete[] infoLog;

		glDeleteShader(shader);
		return;
	}

	m_Shaders.push_back(shader);
}

void Shader::Link()
{
	m_Id = glCreateProgram();
	
	for (const auto& shader : m_Shaders) glAttachShader(m_Id, shader);

	glLinkProgram(m_Id);

	int isLinked{ 0 };
	glGetProgramiv(m_Id, GL_LINK_STATUS, &isLinked);
	if (!isLinked)
	{
		int maxLength = 0;
		glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &maxLength);
		char* infoLog = new char[maxLength];
		glGetProgramInfoLog(m_Id, maxLength, &maxLength, infoLog);

		std::cerr << "SHADER::ERROR::glLinkProgram\n" << infoLog << "\n";
		delete[] infoLog;

		glDeleteProgram(m_Id);
		for (const auto& shader : m_Shaders) glDeleteShader(shader);
		return;
	}

	for (const auto& shader : m_Shaders) glDetachShader(m_Id, shader);
}
