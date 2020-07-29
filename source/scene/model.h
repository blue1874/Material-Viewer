#pragma once

#include "mesh.h"

class Model
{
	public:
		/*  model info   */
		size_t mesh_num;
		size_t vertices_num;
		size_t face_num;
		size_t texture_num;

		std::filesystem::path _path;
		Model();
		Model(std::string path);
		//Model(std::string &&path);
		void Draw(std::shared_ptr<Shader> shader, Camera &camera, glm::mat4 &modelMat, std::string type);
	private:
		/*  mesh  */
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> textures_loaded;
		/*  load   */
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		Texture &loadMaterialTextures(aiMaterial *mat, aiTextureType type, TextureType typeName);
		//Texture &loadCubeMap(std::vector<std::string> &faces);
};