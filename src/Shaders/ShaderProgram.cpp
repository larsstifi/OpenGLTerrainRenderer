#include <Shaders/ShaderProgram.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Models\OBJ_Loader.h>


std::string readFile(const char* filePath) {
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return NULL;
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

// constructor reads and builds the shader
unsigned int ShaderProgram::loadShaderProgram(std::string vertShaderPath, std::string fragShaderPath, std::string geomShaderPath) {
	for (int i = 0; i < loadedShaders.size(); i++)
	{
		if (loadedShaders[i].vertShaderPath == vertShaderPath && loadedShaders[i].fragShaderPath == fragShaderPath && loadedShaders[i].geomShaderPath == geomShaderPath) {
			return loadedShaders[i].id;
		}
	}
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vs = readFile(vertShaderPath.c_str());
	const char* vsP = vs.c_str();
	glShaderSource(vertexShader, 1, &vsP, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fs = readFile(fragShaderPath.c_str());
	const char* fsP = fs.c_str();
	glShaderSource(fragmentShader, 1, &fsP, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int geometryShader;
	if (geomShaderPath != "") {
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		std::string gs = readFile(geomShaderPath.c_str());
		const char* gsP = fs.c_str();
		glShaderSource(geometryShader, 1, &gsP, NULL);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
	


	unsigned int ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	if (geomShaderPath != "") glAttachShader(ID, geometryShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROG::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geomShaderPath != "") glDeleteShader(geometryShader);
	LoadedShader ls;
	ls.fragShaderPath = fragShaderPath;
	ls.vertShaderPath = vertShaderPath;
	ls.geomShaderPath = geomShaderPath;
	loadedShaders.push_back(ls);
	return ID;
}
// use/activate the shader
void ShaderProgram::use(unsigned int ID) {
	glUseProgram(ID);
}
// utility uniform functions
void ShaderProgram::setBool(unsigned int ID, const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void ShaderProgram::setInt(unsigned int ID, const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void ShaderProgram::setFloat(unsigned int ID, const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void ShaderProgram::setVec3(unsigned int ID, const std::string& name, const glm::vec3& value) {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec3(unsigned int ID, const std::string& name, const objl::Vector3& value)
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value.X);
}
void ShaderProgram::setMat4(unsigned int ID, const std::string& name, const glm::mat4& value) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
