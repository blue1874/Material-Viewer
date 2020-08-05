#pragma once

#include <string>
#include <io/load_texture.h>

#define MAX_TEXTURE_NUM 16
enum TextureType { normal, albedo, diffuse, specular, roughness, metalic, AO };

class Texture
{
public:
	int id;
	TextureType type;
	//texture path, used to avoid duplicate texture load
	std::string path;

	Texture() : id(), type(), path("") {};
};