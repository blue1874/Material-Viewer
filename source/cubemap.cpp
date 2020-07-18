#include "cubemap.h"

std::vector<std::string> Cubemap::faces
{
    "./resources/textures/skybox/right.jpg",
    "./resources/textures/skybox/left.jpg",
    "./resources/textures/skybox/top.jpg",
    "./resources/textures/skybox/bottom.jpg",
    "./resources/textures/skybox/front.jpg",
    "./resources/textures/skybox/back.jpg"
};

std::string Cubemap::MODEL_PATH("./resources/objects/cube/cube.fbx");
std::string Cubemap::FRAG_PATH("./shader/cubemap.frag");
std::string Cubemap::VERT_PATH("./shader/cubemap.vert");