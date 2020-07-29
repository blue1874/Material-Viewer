#pragma once
#ifndef LOAD_TEXTURE_H
#define LOAD_TEXTURE_H

#include <iostream>
#include <glad/glad.h>
#include <string>
#include <vector>

#include "tool/path.h"
#include "stb_image.h"

unsigned int load_texture(std::string path = std::string(), int warp_type = GL_REPEAT, int min_filter_type = GL_LINEAR, int max_fileter_type = GL_LINEAR);
unsigned int load_cubemap(std::vector<std::string>& faces);
#endif // !LOAD_TEXTURE_H
