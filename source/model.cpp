#include "model.h"

Model::Model()
{
}

Model::Model(std::string &path)
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

Model::Model(std::string &&path)
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
	// �����ڵ����е���������еĻ���
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// �������������ӽڵ��ظ���һ����
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	
	//��������
	auto pos = std::make_shared<std::vector<glm::vec3>>();
	auto nor = std::make_shared<std::vector<glm::vec3>>();
	auto tex = std::make_shared<std::vector<glm::vec2>>();

	// std::vector<glm::vec3> pos;
	// std::vector<glm::vec3> nor;
	// std::vector<glm::vec2> tex;
	// std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		float tx = 0, ty = 0;
		if (mesh->mTextureCoords[0]) 
		{
			tx = mesh->mTextureCoords[0][i].x;
			ty = mesh->mTextureCoords[0][i].y;
		}
		pos->push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		nor->push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		tex->push_back(glm::vec2(tx, ty));
		// pos.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		// nor.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
		// tex.push_back(glm::vec2(tx, ty));
	}

	auto indices = std::make_shared<std::vector<unsigned int>>();
	// ��������
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
	{
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
		{
			indices->push_back(mesh->mFaces[i].mIndices[j]);
			//indices.push_back(mesh->mFaces[i].mIndices[j]);

		}
	}

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

	Mesh _mesh(pos, nor, tex, indices, mat);
	return _mesh;
}

Texture &Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, TextureType typeName)
{
	Texture texture;
	aiString str;
	mat->GetTexture(type, 0, &str);
	bool skip = false;
	for (unsigned int j = 0; j < textures_loaded.size(); j++)
	{
		if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
		{
			texture = textures_loaded[j];
			texture.type = typeName;
			skip = true;
			break;
		}
	}
	if (!skip)
	{   // ���������û�б����أ��������
		if (!str.length) texture.id = load_texture();
		else texture.id = load_texture((directory + "/" + std::string(str.C_Str())));
		//texture.id = TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.path = str;
		textures_loaded.push_back(texture); // ���ӵ��Ѽ��ص�������
	}

	return texture;
}

//Texture & Model::loadCubeMap(std::vector<std::string>& faces)
//{
//}
