#include "graphics/Shader.h"


Shader::Shader(const char* vertexShader, const char* fragmentShader)
	: m_Id(0)
{
	m_Shaders.push_back(createShader(vertexShader, GL_VERTEX_SHADER));
	m_Shaders.push_back(createShader(fragmentShader, GL_FRAGMENT_SHADER));
}

Shader::~Shader()
{
	for (const auto& shader : m_Shaders) glDeleteShader(shader);
	glDeleteProgram(m_Id);
}

void Shader::AddShader(const char* shaderFilePath, u32 shaderType)
{
	u32 shader = createShader(shaderFilePath, shaderType);

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
		return;
	}

	for (const auto& shader : m_Shaders) glDetachShader(m_Id, shader);
}

u32 Shader::createShader(const char* shaderPath, u32 shaderType) const
{
	u32 shader = glCreateShader(shaderType);

	std::string shaderSourceStr = ReadFile(shaderPath);
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
		return 0;
	}

	return shader;
}
