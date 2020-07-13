#pragma once

#include "mesh.h"
#include "load_texture.h"
class Model
{
	public:
		/*  函数   */
		Model();
		Model(std::string &path);
		Model(std::string &&path);
		void Draw(Shader shader);
	private:
		/*  模型数据  */
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> textures_loaded;
		/*  函数   */
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		Texture &loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName);
		//Texture &loadCubeMap(std::vector<std::string> &faces);
};