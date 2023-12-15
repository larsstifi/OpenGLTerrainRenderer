#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H




#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Models\OBJ_Loader.h>

struct LoadedShader {
    unsigned int id = 0;
    std::string geomShaderPath = "";
    std::string vertShaderPath = "";
    std::string fragShaderPath = "";

};
static std::vector<LoadedShader> loadedShaders;

static class ShaderProgram
{
private:
    
public:
    // constructor reads and builds the shader
    static unsigned int loadShaderProgram(std::string vertShaderPath, std::string fragShaderPath, std::string geomShaderPath = "");
    // use/activate the shader
    static void use(unsigned int ID);
    // utility uniform functions
    static void setBool(unsigned int ID, const std::string& name, bool value);
    static void setInt(unsigned int ID, const std::string& name, int value);
    static void setFloat(unsigned int ID, const std::string& name, float value);
    static void setVec3(unsigned int ID, const std::string& name, const glm::vec3& value);
    static void setVec3(unsigned int ID, const std::string& name, const objl::Vector3& value);
    static void setMat4(unsigned int ID, const std::string& name, const glm::mat4& value);
};


#endif