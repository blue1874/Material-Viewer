#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include "shader.h"
constexpr auto maxLightNum = 8;;

enum LightType {
	empty = -1, point, direct, flash
};

struct Light
{
	LightType type;
	//point light arg
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

	//direct light arg
	glm::vec3 direction;

	//common
	glm::vec3 color;
	float intensity;

	Light(LightType type = LightType::empty)
	{
		switch (type)
		{
		case LightType::empty:
			break;
		case point:
			position = glm::vec3(0.0, 0.0, 0.0);
			constant = 1.0f;
			linear = 0.09f;
			quadratic = 0.032f;
			break;
		case direct:
			direction = glm::vec3(0.0, 0.0, 1.0);
			break;
		case flash:
			break;
		default:
			break;
		}
		this->type = type;
		color = glm::vec3(1.0, 1.0, 1.0);
		intensity = 1.5f;
	}

	void setLightUniform(Shader shader, int index)
	{
		char str[10];
		sprintf_s(str, 10, "%d", index);
		std::string uniformName = "lights[" + std::string(str) + "].";
		switch (type)
		{
		case LightType::empty:
			break;
		case point:
			shader.updateUniform(uniformName + "position", position);
			shader.updateUniform(uniformName + "constant", constant);
			shader.updateUniform(uniformName + "linear", linear);
			shader.updateUniform(uniformName + "quadratic", quadratic);
			break;
		case direct:
			shader.updateUniform(uniformName + "direction", direction);
			break;
		case flash:
			break;
		default:
			break;
		}
		shader.updateUniform(uniformName + "type", type);
		shader.updateUniform(uniformName + "color", color);
		shader.updateUniform(uniformName + "intensity", intensity);
	};
};