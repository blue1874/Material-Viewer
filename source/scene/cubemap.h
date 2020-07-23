#pragma once
#include <vector>
#include <string>

namespace Cubemap {
    inline std::vector<std::string> faces
    {
        "./resources/textures/skybox/right.jpg",
        "./resources/textures/skybox/left.jpg",
        "./resources/textures/skybox/top.jpg",
        "./resources/textures/skybox/bottom.jpg",
        "./resources/textures/skybox/front.jpg",
        "./resources/textures/skybox/back.jpg"
    };

    inline std::string MODEL_PATH("./resources/objects/cube/cube.fbx");
    inline std::string FRAG_PATH("./shader/cubemap.frag");
    inline std::string VERT_PATH("./shader/cubemap.vert");
}
