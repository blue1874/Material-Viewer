#include "shader.h"



Shader::Shader()
{
}

Shader::Shader(const std::vector<std::string> &includeDirs, const std::string &vertexPath, const std::string &fragmentPath, const std::string &geometryPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	ID = glCreateProgram();

	if (loadFromPath(vertexPath, vertexCode, includeDirs))
	{
		GLuint vert;
		const char *src = vertexCode.c_str();
		vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &src, NULL);
		glCompileShader(vert);
		checkErrors(vert, vertexPath);
		glAttachShader(ID, vert);
		glDeleteShader(vert);

	}
	if (loadFromPath(fragmentPath, fragmentCode, includeDirs))
	{
		GLuint frag;
		const char *src = fragmentCode.c_str();
		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &src, NULL);
		glCompileShader(frag);
		checkErrors(frag, fragmentPath);
		glAttachShader(ID, frag);
		glDeleteShader(frag);

	}
	if (geometryPath.size() && loadFromPath(geometryPath, geometryCode, includeDirs))
	{
		GLuint geom;
		const char *src = geometryCode.c_str();
		geom = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geom, 1, &src, NULL);
		glCompileShader(geom);
		checkErrors(geom, geometryPath);
		glAttachShader(ID, geom);
		glDeleteShader(geom);
	}

	glLinkProgram(ID);
	checkErrors(ID);

}

// activate the shader


bool Shader::hasInclude(std::string & includeStr)
{
	for (auto str : dependencies) {
		if (str == includeStr) return true;
	}
	return false;
}

bool Shader::loadFromPath(const std::string & _path, std::string & src, const std::vector<std::string>& _include_folders)
{
	std::ifstream file;
	
	file.open(_path);
	if (!file.is_open()) {
		//std::cout << "file open failed\n please check your path" << _path << "\n";
		return false;
	}
	while (!file.eof()) {
		std::string line;
		getline(file, line);
		if (line.find("#include ") == 0 || line.find("#pragma include ") == 0) {
			std::string includeSrc;
			unsigned begin = line.find_first_of("\"");
			unsigned end = line.find_last_of("\"");
			if (begin != end) {
				std::string includePath(line.substr(begin + 1, end - begin - 1));
				if (!hasInclude(includePath)) {
					for (auto str : _include_folders) {
						if (loadFromPath(str + includePath, includeSrc, _include_folders)) {
							src += "\n" + includeSrc + "\n";
							break;
						}
					}
					dependencies.push_back(includePath);
				}
			}
			continue;
		}
		else if(line.find("const") == 0) {
			auto split_str = split(line);

			if(split_str[1] != "int" && split_str[1] != "uint") continue;
			else {
				int value = std::stoi(split_str[split_str.size() - 1].substr(0, split_str[split_str.size() - 1].size() - 1));
				uniforms.constInts.addConstInt(split_str[2], value);
			}
			src += line + "\n";
		}
		else if(line.find("uniform") == 0) {
			auto split_str = split(line);
			split_str[2] = split_str[2].substr(0, split_str[2].size() - 1);
			if(auto pos = split_str[2].find('['); pos != std::string::npos) {
				auto rpos = split_str[2].find(']');
				auto name = split_str[2].substr(0, pos);
				auto str = split_str[2].substr(pos + 1, rpos - pos - 1);
				int arraySize = 1;
				try
				{
					arraySize = std::stoi(str);
				}
				catch(const std::invalid_argument & e)
				{
					if(!uniforms.constInts.getConstInt(str, arraySize)) {
						std::cout << "operation failed\n";
					}
				}
				for(std::size_t i = 0; i < arraySize; i++) {
					uniforms.addUniform(split_str[1], name + '[' + std::to_string(i) + ']');
				}
			}
			else {
				uniforms.addUniform(split_str[1], split_str[2]);
			}
			src += line + "\n";
		}
		else if(line.find("struct") == 0) {
			std::string structName = split(line)[1];
			src += line + "\n";
			while(getline(file, line)) {
				src += line + "\n";
				if(line.find('}') != std::string::npos) break;
				else if(line.find("//") != std::string::npos || line.find('{') != std::string::npos) continue;
				else {
					auto split_str = split(line);
					if(split_str.size() > 1)
						uniforms.structs.addAttr(split_str[0], split_str[1].substr(0, split_str[1].size() - 1));
				}
			}
			uniforms.structs.addStruct(structName);
		}
		else { src += line + "\n"; }
	}
	file.close();
	return true;
}
void Shader::updateUniform(std::string name, data value)
{
	uniforms.updateUniform(name, value);
}

void Shader::setAllUniforms()
{
	for(auto i : uniforms.members)
	{
		if(std::holds_alternative<int>(i.second)) { setUniform(i.first, std::get<int>(i.second)); continue; }
		else if(std::holds_alternative<float>(i.second)) { setUniform(i.first, std::get<float>(i.second)); continue; }
		else if(std::holds_alternative<bool>(i.second)) { setUniform(i.first, std::get<bool>(i.second)); continue; }
		else if(std::holds_alternative<unsigned int>(i.second)) { setUniform(i.first, std::get<unsigned int>(i.second)); continue; }
		else if(std::holds_alternative<glm::ivec2>(i.second)) { setUniform(i.first, std::get<glm::ivec2>(i.second)); continue; }
		else if(std::holds_alternative<glm::ivec3>(i.second)) { setUniform(i.first, std::get<glm::ivec3>(i.second)); continue; }
		else if(std::holds_alternative<glm::ivec4>(i.second)) { setUniform(i.first, std::get<glm::ivec4>(i.second)); continue; }
		else if(std::holds_alternative<glm::vec2>(i.second)) { setUniform(i.first, std::get<glm::vec2>(i.second)); continue; }
		else if(std::holds_alternative<glm::vec3>(i.second)) { setUniform(i.first, std::get<glm::vec3>(i.second)); continue; }
		else if(std::holds_alternative<glm::vec4>(i.second)) { setUniform(i.first, std::get<glm::vec4>(i.second)); continue; }
		else if(std::holds_alternative<glm::dvec2>(i.second)) { setUniform(i.first, std::get<glm::dvec2>(i.second)); continue; }
		else if(std::holds_alternative<glm::dvec3>(i.second)) { setUniform(i.first, std::get<glm::dvec3>(i.second)); continue; }
		else if(std::holds_alternative<glm::dvec4>(i.second)) { setUniform(i.first, std::get<glm::dvec4>(i.second)); continue; }
		else if(std::holds_alternative<glm::mat2>(i.second)) { setUniform(i.first, std::get<glm::mat2>(i.second)); continue; }
		else if(std::holds_alternative<glm::mat3>(i.second)) { setUniform(i.first, std::get<glm::mat3>(i.second)); continue; }
		else if(std::holds_alternative<glm::mat4>(i.second)) { setUniform(i.first, std::get<glm::mat4>(i.second)); continue; }
		else {}
	}
}
// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::checkErrors(GLuint program)
{
	GLint success;
	GLchar infoLog[1024];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR:\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
}
void Shader::checkErrors(GLuint shader, const std::string &shaderPath)
{
	GLint success;
	GLchar infoLog[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of file: " << shaderPath << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
	}
}
