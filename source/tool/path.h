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

	inline auto getAbsPath([](std::string path) {
		return std::filesystem::canonical(cwd / path);
	});
};

namespace Cubemap {
	inline std::vector<std::string> faces
	{
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/front.jpg",
		"resources/textures/skybox/back.jpg"
	};

	inline std::string MODEL_PATH("resources/objects/cube/cube.fbx");
	inline std::string FRAG_PATH("shader/cubemap.frag");
	inline std::string VERT_PATH("shader/cubemap.vert");
};

namespace DefaultWorkFlow {
	inline std::string MODEL_PATH("resources/objects/head/head.obj");
	inline std::string FRAG_PATH("shader/shader.frag");
	inline std::string VERT_PATH("shader/shader.vert");
}