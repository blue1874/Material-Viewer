#include "mesh.h"
Mesh::Mesh(Vertex vertex, Material &_material)
{
	auto [pos, pos_size, nor, nor_size, tex, tex_size, index, index_size] = vertex;

	vertexSize = pos_size / sizeof(glm::vec3);
	indexSize = index_size / sizeof(size_t);
	material = _material;
	/**
	 * sizeof(glm::vec2) = 8U, why i get 12U in sizeof(decltype(_tex.get()[0]))?
	 * it makes me confused.
	 * */

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// allocate enough space to contain data
	glBufferData(GL_ARRAY_BUFFER, pos_size + nor_size + tex_size, NULL, GL_STATIC_DRAW);
	/** why does &(*_pos)[0] point to different location with _pos.get()?
	 * and &(*_pos)[0] point to correct loaction
	 * 	std::cout << &(*_pos)[0] << " " << _pos.get() << "\n";
	 * */
	glBufferSubData(GL_ARRAY_BUFFER, 0, pos_size, pos);
	glBufferSubData(GL_ARRAY_BUFFER, pos_size, nor_size, nor);
	glBufferSubData(GL_ARRAY_BUFFER, pos_size + nor_size, tex_size, tex);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(pos_size));
	glEnableVertexAttribArray(1);

	//tex attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(pos_size + nor_size));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);	
}

void Mesh::draw(std::shared_ptr<Shader> shader, Camera &camera, std::string type)
{
	shader->updateUniform("projection", camera.GetProjectionMatrix());
	shader->updateUniform("view", camera.GetViewMatrix());
	if(type == "cubemap") {
		shader->updateUniform("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
		// glActiveTexture(GL_TEXTURE0);
		// shader->updateUniform("skybox", 0);
		// glBindTexture(GL_TEXTURE_CUBE_MAP, Cubemap::cubemap);
		shader->setAllUniforms();
	}
	else {
		//int normalN = 0, ambientN = 0, diffuseN = 0, specularN = 0;
		shader->updateUniform("model", glm::mat4(1.0f));
		shader->updateUniform("material.ambient", material.Ka);
		shader->updateUniform("material.diffuse", material.Kd);
		shader->updateUniform("material.specular", material.Ks);

		glActiveTexture(GL_TEXTURE0 + 0);
		shader->updateUniform("material.map_a", 0);
		glBindTexture(GL_TEXTURE_2D, material.map_Ka.id);

		glActiveTexture(GL_TEXTURE0 + 1);
		shader->updateUniform("material.map_d", 1);
		glBindTexture(GL_TEXTURE_2D, material.map_Kd.id);

		glActiveTexture(GL_TEXTURE0 + 2);
		shader->updateUniform("material.map_s", 2);
		glBindTexture(GL_TEXTURE_2D, material.map_Ks.id);

		glActiveTexture(GL_TEXTURE0 + 3);
		shader->updateUniform("material.map_n", 3);
		glBindTexture(GL_TEXTURE_2D, material.map_Kn.id);

		shader->setAllUniforms();
	}

	//glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

}