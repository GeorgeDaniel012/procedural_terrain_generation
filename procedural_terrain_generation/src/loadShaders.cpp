//  Preluat si adaptat dupa http://www.opengl-tutorial.org/beginners-tutorials 


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


void checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}


GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path,
    const char* tessellation_control_file_path, const char* tessellation_evaluation_file_path)
{
    GLuint ID;

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
	std::string tessellationControlCode;
	std::string tessellationEvaluationCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream tcShaderFile;
    std::ifstream teShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(vertex_file_path);
        fShaderFile.open(fragment_file_path);
        tcShaderFile.open(tessellation_control_file_path);
        teShaderFile.open(tessellation_evaluation_file_path);
        std::stringstream vShaderStream, fShaderStream, tcShaderStream, teShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        tcShaderStream << tcShaderFile.rdbuf();
        teShaderStream << teShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        tcShaderFile.close();
        teShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        tessellationControlCode = tcShaderStream.str();
        tessellationEvaluationCode = teShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char * fShaderCode = fragmentCode.c_str();
    const char* tcShaderCode = tessellationControlCode.c_str();
    const char* teShaderCode = tessellationEvaluationCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment, tessellationControl, tessellationEvaluation;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
	// tessellation control shader
    tessellationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tessellationControl, 1, &tcShaderCode, NULL);
    glCompileShader(tessellationControl);
    checkCompileErrors(tessellationControl, "TESSELLATIONCONTROL");
	// tessellation evaluation shader
    tessellationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tessellationEvaluation, 1, &teShaderCode, NULL);
    glCompileShader(tessellationEvaluation);
    checkCompileErrors(tessellationEvaluation, "TESSELLATIONEVALUATION");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glAttachShader(ID, tessellationControl);
    glAttachShader(ID, tessellationEvaluation);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glDeleteShader(tessellationControl);
    glDeleteShader(tessellationEvaluation);

    return ID;
}