#pragma once
#ifndef LOAD_TEXTURE_H
#define LOAD_TEXTURE_H

#include <iostream>
#include <glad/glad.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "tool/path.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

//#define cimg_use_png
//#include "CImg.h"
inline std::unordered_map<std::string, int> loadedTextureMap;
size_t load_texture(size_t internalFormat, const std::string &path = std::string(), int warp_type = GL_REPEAT, int min_filter_type = GL_LINEAR, int max_fileter_type = GL_LINEAR);
size_t load_cubemap(const std::vector<std::string>& faces);
void save_texture2png(const std::string &path, int width, int height, int nchannels, const void *data);
#endif // !LOAD_TEXTURE_H
