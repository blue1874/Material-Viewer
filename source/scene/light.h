#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include "mgl/shader.h"
constexpr auto maxLightNum = 8;

// enum LightType {
// 	empty = -1, point, direct, flash
// };

class Light
{
public:
	glm::vec3 color;
	float intensity;

	Light() : color(glm::vec3(1.0)), intensity(1.0f) {}
	
	virtual void setLightUniform(std::shared_ptr<Shader> shader, size_t index) {};

};

class PointLight : public Light
{
public:
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

	PointLight() : Light(), position(glm::vec3(0.0f, 5.0f, 0.0f)), constant(1.0f), linear(0.09f), quadratic(0.032f) {}
	void setLightUniform(std::shared_ptr<Shader> shader, size_t index)
	{
		std::string uniformPrefix = "light[" + std::to_string(index) + "].";
		shader->updateUniform(uniformPrefix + "color", color);
		shader->updateUniform(uniformPrefix + "intensity", intensity);
		shader->updateUniform(uniformPrefix + "position", position);
		shader->updateUniform(uniformPrefix + "constant", constant);
		shader->updateUniform(uniformPrefix + "linear", linear);
		shader->updateUniform(uniformPrefix + "quadratic", quadratic);
	}
};

class DirectLight : public Light
{
public:
	glm::vec3 direction;

	DirectLight() : Light(), direction(glm::vec3(0.0f, 0.0f, 1.0f)) {}

	void setLightUniform(std::shared_ptr<Shader> shader, size_t index)
	{
		std::string uniformPrefix = "light[" + std::to_string(index) + "].";
		shader->updateUniform(uniformPrefix + "color", color);
		shader->updateUniform(uniformPrefix + "intensity", intensity);
		shader->updateUniform(uniformPrefix + "direction", direction);
	}
};

class spotLight : public Light
{

};
