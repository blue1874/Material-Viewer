#include "textureIO.h"
//using namespace cimg_library;
size_t load_texture(size_t internalFormat, const std::string &path, int warp_type, int min_filter_type, int max_fileter_type)
{
	auto fileAbsPath = pathLoader::getAbsPath(path).string();
	if (loadedTextureMap.find(fileAbsPath) != loadedTextureMap.end()) {
		return loadedTextureMap[fileAbsPath];
	}
	else {
		size_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// 为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp_type);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter_type);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, max_fileter_type);

		if (!path.size())
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			// 加载并生成纹理
			//image.display();
			//CImg<unsigned char> image(pathLoader::getAbsPath(path).string().c_str());
			//auto nChannels = image.spectrum();
			//auto width = image.width();
			//auto height = image.height();
			//image.permute_axes("cxyz");
			//unsigned char* data = image.data();
			int width, height, nChannels;
			unsigned char* data = stbi_load(pathLoader::getAbsPath(path).string().c_str(), &width, &height, &nChannels, 0);


			if (data)
			{
				if (nChannels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				else if (nChannels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
				else if (nChannels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		loadedTextureMap[fileAbsPath] = texture;
		return texture;
	}

}

size_t load_cubemap(const std::vector<std::string>& faces)
{
	size_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	int width, height, nrChannels;
	for (size_t i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(pathLoader::getAbsPath(faces[i]).string().c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}

void save_texture2png(const std::string& path, int width, int height, int nchannels, const void* data)
{
	stbi_write_png((pathLoader::cwd.string() + path).c_str(), width, height, nchannels, data, 0);
}

