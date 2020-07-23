#include "model.h"

Model::Model()
{
}

Model::Model(std::string &path) : mesh_num(0), vertices_num(0), face_num(0), texture_num(0), _path(path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

Model::Model(std::string &&path) : mesh_num(0), vertices_num(0), face_num(0), texture_num(0), _path(path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::Draw(std::shared_ptr<Shader> shader, Camera &camera, std::string type)
{
	for (auto mesh : meshes) mesh.draw(shader, camera, type);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	mesh_num += node->mNumMeshes;
	// process mesh
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// process model node recursively
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{

	// these attribute can not directly be used by opengl, need to transform
	std::vector<glm::vec2> tex(mesh->mNumVertices * 2, glm::vec2(0));
	std::vector<size_t> indices(mesh->mNumFaces * 3, 0);

	if(mesh->mTextureCoords[0]) 
	{
		for(size_t i = 0; i < mesh->mNumVertices; i++)
		{
			tex[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
	}

	// index data
	for (size_t i = 0; i < mesh->mNumFaces; i++) 
	{
		for (int j = 0; j < 3; j++)
		{
			indices[i * 3 + j] = mesh->mFaces[i].mIndices[j];
		}
	}

	Vertex vertex;
	vertex.position = mesh->mVertices;
	vertex.position_size = mesh->mNumVertices * sizeof(glm::vec3);
	vertex.normal = mesh->mNormals;
	vertex.normal_size = mesh->mNumVertices * sizeof(glm::vec3);
	vertex.texCoord = &tex[0];
	vertex.texCoord_size = tex.size() *sizeof(glm::vec2);
	vertex.index = &indices[0];
	vertex.index_size = indices.size() * sizeof(size_t);
	// get material
	Material mat;
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D Ka;
		material->Get(AI_MATKEY_COLOR_AMBIENT, Ka);
		mat.Ka = glm::vec3(Ka.r, Ka.g, Ka.b);
		aiColor3D Kd;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, Kd);	
		mat.Kd = glm::vec3(Kd.r, Kd.g, Kd.b);
		glm::vec3 kd(Kd.r, Kd.g, Kd.b);
		aiColor3D Ks;
		material->Get(AI_MATKEY_COLOR_SPECULAR, Ks);
		mat.Ks = glm::vec3(Ks.r, Ks.g, Ks.b);

		//material->mProperties;
		mat.map_Ka = loadMaterialTextures(material, aiTextureType_AMBIENT, ambient);
		mat.map_Kd = loadMaterialTextures(material, aiTextureType_DIFFUSE, diffuse);
		mat.map_Ks = loadMaterialTextures(material, aiTextureType_SPECULAR, specular);
		mat.map_Kn = loadMaterialTextures(material, aiTextureType_HEIGHT, normal);
	}

	vertices_num += mesh->mNumVertices;
	face_num += mesh->mNumFaces;
	texture_num = textures_loaded.size();
	
	Mesh _mesh(vertex, mat);
	//Mesh _mesh(pos, nor, tex, indices, mat);
	return _mesh;
}

Texture &Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType typeName)
{
	aiString str;
	mat->GetTexture(type, 0, &str);
	for (size_t j = 0; j < textures_loaded.size(); j++)
	{
		if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
		{
			return textures_loaded[j];
		}
	}
	
	Texture texture;
	if (!str.length) texture.id = load_texture();
	else texture.id = load_texture((directory + "/" + std::string(str.C_Str())));
	//texture.id = TextureFromFile(str.C_Str(), directory);
	texture.type = typeName;
	texture.path = str;
	textures_loaded.push_back(texture);
	return textures_loaded.back();
}

//Texture & Model::loadCubeMap(std::vector<std::string>& faces)
//{
//}
