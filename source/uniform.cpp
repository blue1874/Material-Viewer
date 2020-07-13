#include "uniform.h"

std::map<std::string, unsigned int> typeIndex = {
    std::pair<std::string, unsigned int>("int", _int),
    std::pair<std::string, unsigned int>("float", _float),
    std::pair<std::string, unsigned int>("bool", _bool),
    std::pair<std::string, unsigned int>("uint", _uint),
    std::pair<std::string, unsigned int>("double", _double),
    std::pair<std::string, unsigned int>("ivec2", ivec2),
    std::pair<std::string, unsigned int>("ivec3", ivec3),
    std::pair<std::string, unsigned int>("ivec4", ivec4),
    std::pair<std::string, unsigned int>("vec2", vec2),
    std::pair<std::string, unsigned int>("vec3", vec3),
    std::pair<std::string, unsigned int>("vec4", vec4),
    std::pair<std::string, unsigned int>("dvec2", dvec2),
    std::pair<std::string, unsigned int>("dvec3", dvec3),
    std::pair<std::string, unsigned int>("dvec4", dvec4),
    std::pair<std::string, unsigned int>("mat2", mat2),
    std::pair<std::string, unsigned int>("mat3", mat3),
    std::pair<std::string, unsigned int>("mat4", mat4),
    std::pair<std::string, unsigned int>("sampler1D", _uint),
    std::pair<std::string, unsigned int>("sampler2D", _uint),
    std::pair<std::string, unsigned int>("samplerCube", _uint),
    };

data Uniforms::getType(std::string type)
{
    data res;
    if(typeIndex.find(type) != typeIndex.end()) {
        res = variant_from_index<data>(typeIndex.at(type));
    }
    else {
        std::cout << "not supported uniform type: " << type << "\n";
    }
    return res;
}

void Uniforms::updateUniform(std::string name, data value)
{
    if(members.find(name) != members.end()) members[name] = value;
	else {
		// std::cout << "ERROR: operation tring to set uniform " << name << " failed\n";
		// std::cout << "uniform with the name given does not exist.\n";
	}
}

void Uniforms::addUniform(std::string type, std::string name)
{
    if(auto res = structs.members.find(type); res != structs.members.end()) {
        for(auto i : res->second) {
            addUniform(i.second, name + "." + i.first);
        }
    }
    else {
        if(members.find(name) != members.end()) {
            std::cout << "WARNING: " << name << " uniform definition duplicated\n";
            std::cout << "use post value\n";
        }
        data value = getType(type);
        members[name] = value;
    }

}

void Uniforms::addUniform(data d, const std::string &name)
{
    if(members.find(name) != members.end()) {
        std::cout << "WARNING: " << name << " uniform definition duplicated\n";
        std::cout << "use post value\n";
	}
	members[name] = d;
}


void Structs::addStruct(std::string &name)
{
    if(members.find(name) != members.end()) {
        std::cout << "duplicated struct name: " << name << "\n";
        std::cout << "use original definition.";
    }
    members[name] = tempAttrList;
    tempAttrList.clear();
}

void Structs::addAttr(std::string &type, std::string name)
{
    // process struct type
    if(auto res = members.find(type); res != members.end()) {
        for(auto i : res->second) {
            std::string tmp = name + "." + i.first;
            addAttr(i.second, tmp);
        }
    }
    // process normal type
    else {
        if(tempAttrList.find(name) != tempAttrList.end()) {
            std::cout << "duplicated attribute name: " << name << "\n";
            std::cout << "use original definition.";
        }
        tempAttrList[name] = Uniforms::getType(type);
    }
}

void Structs::addAttr(data d, std::string &name)
{
    if(tempAttrList.find(name) != tempAttrList.end()) {
        std::cout << "duplicated attribute name: " << name << "\n";
        std::cout << "use original definition.";
    }
    tempAttrList[name] = d;
}