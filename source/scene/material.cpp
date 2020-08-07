#include "material.h"

Material::Material()
{
	albedoFile.needReload = normalFile.needReload = metalicFile.needReload = roughnessFile.needReload = AOFile.needReload = true;
	iter[std::shared_ptr<GLuint>(&albedoMap)] = std::make_shared<fileGui>(albedoFile);
	//iter[std::make_shared<GLuint>(diffuseMap)] = std::make_shared<fileGui>(diffuseFile);
	//iter[std::make_shared<GLuint>(specularMap)] = std::make_shared<fileGui>(specularFile);
	iter[std::shared_ptr<GLuint>(&normalMap)] = std::make_shared<fileGui>(normalFile);
	iter[std::shared_ptr<GLuint>(&metalicMap)] = std::make_shared<fileGui>(metalicFile);
	iter[std::shared_ptr<GLuint>(&roughnessMap)] = std::make_shared<fileGui>(roughnessFile);
	iter[std::shared_ptr<GLuint>(&AOMap)] = std::make_shared<fileGui>(AOFile);
	

}

void Material::drawGUi()
{
	for (auto &[tex, file] : iter)
	{
		file->toGUi();
		if (file->filePath != "")
		{
			if (file->needReload) {
				*tex = load_texture(GL_SRGB, file->filePath);
				file->needReload = false;
			}
			ImGui::Image((void*)(intptr_t)(*tex), ImVec2(300, 300));
		}
		else {
			glDeleteTextures(1, &(*tex));
			file->needReload = false;
		}

	}

	ImGui::ColorEdit3("albedo", (float*)&albedo);
	ImGui::SliderFloat("metalic", &metalic, 0.0f, 1.0f);
	ImGui::SliderFloat("roughness", &roughness, 0.0f, 1.0f);
	ImGui::SliderFloat("AO", &AO, 0.0f, 1.0f);

}
