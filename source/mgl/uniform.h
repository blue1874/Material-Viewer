#pragma once
#include <glm/glm.hpp>
#include <string>
#include <variant>
#include <map>
#include <iostream>
#include <vector>

enum uniformType {_empty, _int, _float, _bool, _uint, _double, 
    ivec2, ivec3, ivec4, vec2, vec3, 
    vec4, dvec2, dvec3, dvec4, 
    mat2, mat3, mat4};
/** 
 * unifromType variant
 * */
typedef std::variant<std::monostate, int, float, bool, size_t, double, 
    glm::ivec2, glm::ivec3, glm::ivec4, glm::vec2, glm::vec3, 
    glm::vec4, glm::dvec2, glm::dvec3, glm::dvec4, glm::mat2, 
    glm::mat3, glm::mat4> data; 

// string to type reflection
inline std::map<std::string, size_t> typeIndex = {
    std::pair<std::string, size_t>("int", _int),
    std::pair<std::string, size_t>("float", _float),
    std::pair<std::string, size_t>("bool", _bool),
    std::pair<std::string, size_t>("uint", _uint),
    std::pair<std::string, size_t>("double", _double),
    std::pair<std::string, size_t>("ivec2", ivec2),
    std::pair<std::string, size_t>("ivec3", ivec3),
    std::pair<std::string, size_t>("ivec4", ivec4),
    std::pair<std::string, size_t>("vec2", vec2),
    std::pair<std::string, size_t>("vec3", vec3),
    std::pair<std::string, size_t>("vec4", vec4),
    std::pair<std::string, size_t>("dvec2", dvec2),
    std::pair<std::string, size_t>("dvec3", dvec3),
    std::pair<std::string, size_t>("dvec4", dvec4),
    std::pair<std::string, size_t>("mat2", mat2),
    std::pair<std::string, size_t>("mat3", mat3),
    std::pair<std::string, size_t>("mat4", mat4),
    std::pair<std::string, size_t>("sampler1D", _int),
    std::pair<std::string, size_t>("sampler2D", _int),
    std::pair<std::string, size_t>("samplerCube", _int),
    };
    
template <class Variant, std::size_t I = 0>
Variant variant_from_index(std::size_t index) {
    if constexpr(I >= std::variant_size_v<Variant>)
        throw std::runtime_error{"Variant index " + std::to_string(I + index) + " out of bounds"};
    else
        return index == 0
            ? Variant{std::in_place_index<I>}
            : variant_from_index<Variant, I + 1>(index - 1);
}

class ConstInt
{
private:
    /* data */
    std::map<std::string, int> members;
public:

    bool getConstInt(std::string &name, int &value) {
        if(members.find(name) != members.end()) {
            value = members[name];
            return true;
        }
        else {
            std::cout << "const int value named " << name << " not found.\n";
            return false;
        }
    }
    void addConstInt(std::string name, int value)
    {
        if(members.find(name) != members.end()) {
            std::cout << "WARNING: duplicated const int value named " << name << "\n";
        }
        members[name] = value;      
    }
};



class Structs
{
private:
    void addAttr(data d, std::string &name);
public:
    std::map<std::string, data> tempAttrList;
    // map<name(struct), map<name, value>>
    std::map<std::string, std::map<std::string, data>> members;

    // add struct to list, clear attibute cache
    void addStruct(std::string &name);

    // add attribute to cache
    void addAttr(std::string &type, std::string name);
};

class Uniforms
{
private:
    void addUniform(data d, const std::string &name);
public:
	std::map<std::string, data> members;
    Structs structs;
    ConstInt constInts;
    // data value;
    Uniforms() {};
    // Uniforms(data &_value) : value(_value) {};
    ~Uniforms() {};
    /** 
     * unifromType Reflection,
     * from string to type
     * */
    static data getType(std::string type);

	void updateUniform(std::string name, data value);
	void addUniform(std::string type, std::string name);
    // data getValue() {return value;};
    // void setValue(data &_value) {value = _value;};
};


// class Uniform
// {
// public:
// 	virtual bool setUniformValue(void *ptr, std::string _type) { return true; };
// 	virtual std::any getValue();
// };

// template<class T>
// class allTypeUniform : public Uniform
// {
// 	T value;
// public:
// 	/**
// 	 * WARNING: MUST ENSURE VALUE HAS BEEN INITIALIZED
// 	 * */
// 	allTypeUniform(T _value) : value(_value) {};

// 	/**
// 	 * use typeid(T).name() to get type name
// 	 * */
// 	bool setUniformValue(void *ptr, std::string _type)
// 	{
// 		if (_type != typeid(value).name()) {
// 			std::cout << "error at setting " << typeid(value).name() << " uniform with " << _type << " type, using original value instead\n";
// 			return false;
// 		}
// 		T *_ptr = (T *)ptr;
// 		value = *_ptr;
// 		return true;
// 	}
// 	// set ptr point to value, _type as data type of value
// 	std::any getValue()
// 	{
// 		return value;
// 	}
// };