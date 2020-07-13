#include "mesh.h"
// Mesh::Mesh(std::vector<glm::vec3> &_pos, std::vector<glm::vec3> &_nor, std::vector<glm::vec2> &_tex, std::vector<unsigned int> &_index, Material &material)
// {
// 	vertexSize = _pos.size();
// 	indexSize = _index.size();
// 	this->material = material;
// 	/**
// 	 * sizeof(glm::vec2) = 8U, why i get 12U in sizeof(decltype(_tex.get()[0]))?
// 	 * it makes me confused.
// 	 * */
// 	unsigned int sizeofPos = sizeof(glm::vec3) * _pos.size();
// 	unsigned int sizeofNor = sizeof(glm::vec3) * _nor.size();
// 	unsigned int sizeofTex = sizeof(glm::vec2) * _tex.size();
// 	unsigned int sizeofInd = sizeof(unsigned int) * _index.size();

// 	glGenVertexArrays(1, &VAO);
// 	glGenBuffers(1, &VBO);
// 	glGenBuffers(1, &IBO);

// 	glBindVertexArray(VAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, VBO);
// 	// allocate enough space to contain data
// 	glBufferData(GL_ARRAY_BUFFER, sizeofPos + sizeofNor + sizeofTex, NULL, GL_STATIC_DRAW);
// 	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeofPos, &_pos[0]);
// 	glBufferSubData(GL_ARRAY_BUFFER, sizeofPos, sizeofNor, &_nor[0]);
// 	glBufferSubData(GL_ARRAY_BUFFER, sizeofPos + sizeofNor, sizeofTex, &_tex[0]);


// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofInd, &_index[0], GL_STATIC_DRAW);

// 	// position attribute
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
// 	glEnableVertexAttribArray(0);

// 	//normal attribute
// 	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(sizeofPos));
// 	glEnableVertexAttribArray(1);

// 	//tex attribute
// 	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(sizeofPos + sizeofNor));
// 	glEnableVertexAttribArray(2);

// 	glBindVertexArray(0);
// }
Mesh::Mesh(std::shared_ptr<std::vector<glm::vec3>> _pos, std::shared_ptr<std::vector<glm::vec3>> _nor, std::shared_ptr<std::vector<glm::vec2>> _tex, std::shared_ptr<std::vector<unsigned int>> _index, Material &_material)
{
	vertexSize = _pos->size();
	indexSize = _index->size();
	this->material = _material;
	/**
	 * sizeof(glm::vec2) = 8U, why i get 12U in sizeof(decltype(_tex.get()[0]))?
	 * it makes me confused.
	 * */
	unsigned int sizeofPos = sizeof(glm::vec3) * _pos->size();
	unsigned int sizeofNor = sizeof(glm::vec3) * _nor->size();
	unsigned int sizeofTex = sizeof(glm::vec2) * _tex->size();
	unsigned int sizeofInd = sizeof(unsigned int) * _index->size();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// allocate enough space to contain data
	glBufferData(GL_ARRAY_BUFFER, sizeofPos + sizeofNor + sizeofTex, NULL, GL_STATIC_DRAW);
	/** why does &(*_pos)[0] point to different location with _pos.get()?
	 * and &(*_pos)[0] point to correct loaction
	 * 	std::cout << &(*_pos)[0] << " " << _pos.get() << "\n";
	 * */
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeofPos, &(*_pos)[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeofPos, sizeofNor, &(*_nor)[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeofPos + sizeofNor, sizeofTex, &(*_tex)[0]);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofInd, &(*_index)[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(sizeofPos));
	glEnableVertexAttribArray(1);

	//tex attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(sizeofPos + sizeofNor));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::draw(Shader &shader)
{
	//int normalN = 0, ambientN = 0, diffuseN = 0, specularN = 0;

	shader.updateUniform("material.ambient", material.Ka);
	shader.updateUniform("material.diffuse", material.Kd);
	shader.updateUniform("material.specular", material.Ks);

	//glActiveTexture(GL_TEXTURE0);
	//shader.setInt("t", t);
	//glBindTexture(GL_TEXTURE_2D, t);
	glActiveTexture(GL_TEXTURE0 + 0);
	shader.updateUniform("material.map_a", 0);
	glBindTexture(GL_TEXTURE_2D, material.map_Ka.id);

	glActiveTexture(GL_TEXTURE0 + 1);
	shader.updateUniform("material.map_d", 1);
	glBindTexture(GL_TEXTURE_2D, material.map_Kd.id);

	glActiveTexture(GL_TEXTURE0 + 2);
	shader.updateUniform("material.map_s", 2);
	glBindTexture(GL_TEXTURE_2D, material.map_Ks.id);

	glActiveTexture(GL_TEXTURE0 + 3);
	shader.updateUniform("material.map_n", 3);
	glBindTexture(GL_TEXTURE_2D, material.map_Kn.id);

	shader.setAllUniforms();
	//glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

}
