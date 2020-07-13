#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>

#include "uniform.h"
#include "shaderTextProcess.h"

class Shader
{
public:
    GLuint ID;
	std::vector<std::string> dependencies;
	Uniforms uniforms;
	//std::vector<uniform<std::any>> uniforms;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
	//Shader(const std::string &vertexSrc, const std::string &fragmentSrc, const std::string &geometrySrc = "");
	Shader();
	Shader(const std::vector<std::string> &includeDirs, const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath = "");
    // activate the shader

	inline void use() { glUseProgram(ID); }
	void updateUniform(std::string name, data value);
	void setAllUniforms();

	bool hasInclude(std::string &includeStr);
	bool loadFromPath(const std::string &_path, std::string &src, const std::vector<std::string> &_include_folders);
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
	void checkErrors(GLuint program);
	void checkErrors(GLuint shader, const std::string &shaderPath);

	/**
	 * overload setUniform function
	 * not use template since no same opengl API can be used 
	 * */
	inline void setUniform(const std::string & name, unsigned int value) const { glUniform1ui(glGetUniformLocation(ID, name.c_str()), (unsigned int)value); }
	inline void setUniform(const std::string & name, bool value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); }
	inline void setUniform(const std::string & name, int value) const { glUniform1i(glGetUniformLocation(ID, name.c_str()), value); }
	inline void setUniform(const std::string & name, double value) const { glUniform1d(glGetUniformLocation(ID, name.c_str()), value); };
	inline void setUniform(const std::string & name, float value) const { glUniform1f(glGetUniformLocation(ID, name.c_str()), value); }
	inline void setUniform(const std::string & name, const glm::vec2 & value) const { glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::ivec2 & value) const { glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::dvec2 & value) const { glUniform2dv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::vec3 & value) const { glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::ivec3 & value) const { glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::dvec3 & value) const { glUniform3dv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::vec4 & value) const { glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::ivec4 & value) const { glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, const glm::dvec4 & value) const { glUniform4dv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); }
	inline void setUniform(const std::string & name, float x, float y) const { glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); }
	inline void setUniform(const std::string & name, float x, float y, float z) const { glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); }
	inline void setUniform(const std::string & name, float x, float y, float z, float w) const { glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); }
	inline void setUniform(const std::string & name, const glm::mat2 & mat) const { glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
	inline void setUniform(const std::string & name, const glm::mat3 & mat) const { glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);}
	inline void setUniform(const std::string & name, const glm::mat4 & mat) const { glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
};