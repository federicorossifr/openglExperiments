//
// Created by federico on 24/07/2021.
//
#include "ShaderHandler.h"


int checkShaderCompilationErrors(unsigned int vertexShader) {
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return success;
}

int checkShaderLinkingError(unsigned int shaderProgram) {
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    return success;
}

unsigned int compileShaderFromSource(const char *vertexShaderSource,const char *fragmentShaderSource ) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    checkShaderCompilationErrors(vertexShader);
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    checkShaderCompilationErrors(fragmentShader);

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    checkShaderLinkingError(shaderProgram);

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

ShaderHandler::ShaderHandler(std::string&& vertexFile, std::string&& fragmentFile) {
    std::string vertexCode,fragmentCode;
    std::ifstream vShaderFile,fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexFile);
        fShaderFile.open(fragmentFile);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch(std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    this->shaderProgramId = compileShaderFromSource(vertexCode.c_str(),fragmentCode.c_str());
}

void ShaderHandler::useShader() {
    glUseProgram(this->shaderProgramId);
}

template<typename UniformType>
void ShaderHandler::setScalarUniform(const std::string& name, UniformType value) const {
    if constexpr(std::is_same<UniformType,float>::value)
        glUniform1f(glGetUniformLocation(this->shaderProgramId, name.c_str()), value);
    if constexpr(std::is_same<UniformType,bool>::value || std::is_same<UniformType,int>::value)
        glUniform1i(glGetUniformLocation(this->shaderProgramId, name.c_str()), (int)value);
}

template<typename UniformVecType>
void ShaderHandler::setVec3Uniform(const std::string &name, std::vector<UniformVecType> value) const {
    int loc = glGetUniformLocation(this->shaderProgramId, name.c_str());
    if constexpr(std::is_same<UniformVecType,float>::value)
        glUniform3f(loc, value[0],value[1],value[2]);
    if constexpr(std::is_same<UniformVecType,bool>::value || std::is_same<UniformVecType,int>::value)
        glUniform3i(loc, (int)value[0],(int)value[1],(int)value[2]);
}

void ShaderHandler::applyMat(const std::string &name, glm::mat4 mat) const {
    int loc = glGetUniformLocation(this->shaderProgramId, name.c_str());
    glUniformMatrix4fv(loc,1,false,(float*)&mat);
}

void ShaderHandler::applyVec4(const std::string &name, glm::vec4 vec) const {
    int loc = glGetUniformLocation(this->shaderProgramId, name.c_str());
    glUniform4fv(loc,1,(float*)&vec);
}

template void ShaderHandler::setScalarUniform<float>(const std::string& name, float value) const;
template void ShaderHandler::setScalarUniform<int>(const std::string& name, int value) const;
template void ShaderHandler::setScalarUniform<bool>(const std::string& name, bool value) const;
template void ShaderHandler::setVec3Uniform<float>(const std::string& name, std::vector<float> value) const;
template void ShaderHandler::setVec3Uniform<int>(const std::string& name, std::vector<int> value) const;
template void ShaderHandler::setVec3Uniform<bool>(const std::string& name, std::vector<bool> value) const;