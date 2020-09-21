#pragma once
#include <glm/glm.hpp>
#include <string>
#include <variant>
#include <map>
#include <unordered_map>
#include <iostream>
#include <vector>

#define UNIFORM_TYPE(X) X(empty) X(int) X(float) X(bool) X(uint)  \
    X(double) X(ivec2) X(ivec3) X(ivec4) X(vec2) X(vec3) X(vec4) \
    X(dvec2) X(dvec3) X(dvec4) X(mat2) X(mat3) X(mat4)
#define INT_TYPE(X) X(sampler1D) X(sampler2D) X(sampler2DMS) X(samplerCube)

//create uniform enum
#define X(a) _##a,
enum uniformType { UNIFORM_TYPE(X) };
#undef X

//create map for string to enum
#define X(a) std::pair<std::string, size_t>(#a, _##a),
#define XX(a) std::pair<std::string, size_t>(#a, _int),
inline std::unordered_map<std::string, size_t> typeIndex = { UNIFORM_TYPE(X) INT_TYPE(XX)};
#undef X
    
/** 
 * unifromType variant
 * */
typedef std::variant<std::monostate, int, float, bool, size_t, double, 
    glm::ivec2, glm::ivec3, glm::ivec4, glm::vec2, glm::vec3, 
    glm::vec4, glm::dvec2, glm::dvec3, glm::dvec4, glm::mat2, 
    glm::mat3, glm::mat4> data; 

    
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