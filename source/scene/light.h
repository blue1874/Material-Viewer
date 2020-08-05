#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <typeinfo>
#include "mgl/shader.h"
#include "scene/console.h"
#include "cube.h"

constexpr auto maxLightNum = 8;

enum LightType {
	empty = -1, point, direct, flash
};

class Light
{
public:
	size_t index;
	glm::vec3 color;
	float intensity;
	LightType type;

	Light(LightType _type, size_t _index = 0) : type(_type), color(glm::vec3(1.0)), intensity(1.0f), index(_index) {}
	
	virtual void setLightUniform(std::shared_ptr<Shader> shader) {};
	virtual void drawGui() 
	{
		ImGui::ColorEdit3("color", (float*)&color);
		ImGui::SliderFloat("intensity", &intensity, 0.0f, 1.0f);
		ImGui::Text(typeid(type).name());
	};
	virtual void drawModel(std::shared_ptr<Shader> shader, glm::mat4 viewMat, glm::mat4 projectionMat) {};
};

class PointLight : public Light
{
public:
	glm::vec3 originPos;
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;

	std::shared_ptr<CubeModel> cube;
	glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	PointLight(size_t _index = 0) : 
		Light(LightType::point, _index), position(glm::vec3(5.0f, 5.0f, 5.0f)), constant(1.0f), linear(0.09f), quadratic(0.032f) 
	{
		cube = std::make_shared<CubeModel>(CubeModel(position));
		originPos = position;
	}
	void setLightUniform(std::shared_ptr<Shader> shader)
	{
		std::string uniformPrefix = "lights[" + std::to_string(index) + "].";
		shader->updateUniform(uniformPrefix + "type", type);
		shader->updateUniform(uniformPrefix + "color", color);
		shader->updateUniform(uniformPrefix + "intensity", intensity);
		shader->updateUniform(uniformPrefix + "position", position);
		shader->updateUniform(uniformPrefix + "constant", constant);
		shader->updateUniform(uniformPrefix + "linear", linear);
		shader->updateUniform(uniformPrefix + "quadratic", quadratic);
	}
	void drawGui()
	{
		if (ImGui::TreeNode(("lights[" + std::to_string(index) + "]").c_str()))
		{
			Light::drawGui();
			ImGui::SliderFloat3("pos", (float*)&position, -30.0f, 30.0f);
			ImGui::TreePop();
		}
		modelMat = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)), position - originPos);
	}
	void drawModel(std::shared_ptr<Shader> shader, glm::mat4 viewMat, glm::mat4 projectionMat)
	{
		shader->use();
		shader->updateUniform("model", modelMat);
		shader->updateUniform("view", viewMat);
		shader->updateUniform("projection", projectionMat);
		shader->updateUniform("lightColor", this->color);
		shader->setAllUniforms();
		cube->draw();
	}
};

class DirectLight : public Light
{
public:
	glm::vec3 direction;

	DirectLight(size_t _index = 0) : Light(LightType::direct, _index), direction(glm::vec3(0.0f, 0.0f, 1.0f)) {}

	void setLightUniform(std::shared_ptr<Shader> shader)
	{
		std::string uniformPrefix = "lights[" + std::to_string(index) + "].";
		shader->updateUniform(uniformPrefix + "type", type);
		shader->updateUniform(uniformPrefix + "color", color);
		shader->updateUniform(uniformPrefix + "intensity", intensity);
		shader->updateUniform(uniformPrefix + "direction", direction);
	}
	void drawGui()
	{
		if (ImGui::TreeNode(("lights[" + std::to_string(index) + "]").c_str()))
		{
			Light::drawGui();
			ImGui::SliderFloat3("direction", (float*)&direction, 0.0f, 1.0f);
			ImGui::TreePop();

		}
	}
	void drawModel(std::shared_ptr<Shader> shader, glm::mat4 viewMat, glm::mat4 projectionMat) {};

};

class spotLight : public Light
{

};
