#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

namespace pathLoader{
	inline const std::string PROJECT_NAME("mgl");
	inline auto cwd = []() {
		auto tmp = std::filesystem::current_path().string();
		return std::filesystem::path(tmp.substr(0, tmp.find(PROJECT_NAME) + PROJECT_NAME.size() + 1));
	} ();

	inline auto getAbsPath([](const std::string &path) {
		return std::filesystem::canonical(cwd / path);
	});
};

namespace Cubemap {
	inline const std::vector<std::string> faces
	{
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/front.jpg",
		"resources/textures/skybox/back.jpg"
	};

	inline const std::string MODEL_PATH("resources/objects/cube/cube.fbx");
	inline const std::string FRAG_PATH("shader/cubemap.frag");
	inline const std::string VERT_PATH("shader/cubemap.vert");
};

namespace DefaultWorkFlow {
	inline const std::string ALBEDO("resources/textures/pbr/rusted_iron/albedo.png");
	inline const std::string AO("resources/textures/pbr/rusted_iron/ao.png");
	inline const std::string METALLIC("resources/textures/pbr/rusted_iron/metallic.png");
	inline const std::string ROUGHNESS("resources/textures/pbr/rusted_iron/roughness.png");
	inline const std::string NORMAL("resources/textures/pbr/rusted_iron/normal.png");


	inline const std::string FRAG_PATH("shader/shader.frag");
	inline const std::string VERT_PATH("shader/shader.vert");
}