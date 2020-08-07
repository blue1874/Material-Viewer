#include "ball.h"

BallModel::BallModel(size_t _divide_num) : divide_num(_divide_num)
{
	assert(divide_num % 2 == 0);
	std::vector<glm::vec3> pos;
	std::vector<glm::vec2> tex;
	std::vector<glm::vec3> normal;
	std::vector<size_t> index;
	size_t vertex_num = (divide_num / 2 - 1) * divide_num + 2;
	pos.resize(vertex_num);
	tex.resize(vertex_num);
	normal.resize(vertex_num);
	index_num = (divide_num / 2 - 1) * divide_num * 2 * 3;
	index.resize(index_num);

	pos.front() = glm::vec3(0.0f, radius, 0.0f);
	pos.back() = glm::vec3(0.0f, -radius, 0.0f);
	normal.front() = glm::normalize(pos.front());
	normal.back() = glm::normalize(pos.back());
	tex.front() = glm::vec2(0, 0.5f);
	tex.back() = glm::vec2(1, 0.5f);
	const auto theta = glm::two_pi<float>() / float(divide_num);
	size_t index_cnt = 0;
	for (size_t i = 1; i < divide_num / 2; i++)
	{
		for (size_t j = 0; j < divide_num; j++)
		{
			size_t k = (i - 1) * divide_num + j + 1;
			auto vertex = radius * glm::vec3(glm::sin(i * theta) * glm::cos(j * theta), 
				glm::cos(i * theta), glm::sin(i * theta) * glm::sin(j * theta));
			pos[k] = vertex;
			normal[k] = glm::normalize(vertex);
			tex[k] = glm::vec2(float(i) * 2 / float(divide_num), float(j) / float(divide_num));
			size_t first = k, second, third;
			if (j == divide_num - 1) second = k + 1 - divide_num;
			else second = k + 1;

			if (second <= divide_num) third = 0;
			else third = second - divide_num;

			index[index_cnt * 6] = k;
			index[index_cnt * 6 + 1] = third;
			index[index_cnt * 6 + 2] = second;
			index[index_cnt * 6 + 3] = k;
			index[index_cnt * 6 + 4] = second;
			index[index_cnt * 6 + 5] = k + divide_num > vertex_num - 1 ? vertex_num - 1 : k + divide_num;
			index_cnt++;
		}
	}

	GLuint VBO, IBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// allocate enough space to contain data
	auto pos_size = pos.size() * sizeof(glm::vec3);
	auto tex_size = tex.size() * sizeof(glm::vec2);
	auto nor_size = normal.size() * sizeof(glm::vec3);
	auto index_size = index.size() * sizeof(size_t);
	glBufferData(GL_ARRAY_BUFFER, pos_size + tex_size + nor_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, pos_size, &pos[0]);
	glBufferSubData(GL_ARRAY_BUFFER, pos_size, tex_size, &tex[0]);
	glBufferSubData(GL_ARRAY_BUFFER, pos_size + tex_size, nor_size, &normal[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, &index[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	//tex attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(pos_size));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(pos_size + tex_size));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

BallModel::~BallModel()
{
}

void BallModel::draw(std::shared_ptr<Shader> shader, Camera& camera, glm::mat4& modelMat)
{
	shader->use();
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, material->albedoMap);
	shader->updateUniform("material.albedoMap", 0);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, material->metalicMap);
	shader->updateUniform("material.metalicMap", 1);	
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, material->roughnessMap);
	shader->updateUniform("material.roughnessMap", 2);	
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, material->AOMap);
	shader->updateUniform("material.AOMap", 3);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, material->normalMap);
	shader->updateUniform("material.normalMap", 4);

	shader->updateUniform("material.albedo", material->albedo);
	shader->updateUniform("material.metalic", material->metalic);
	shader->updateUniform("material.roughness", material->roughness);
	shader->updateUniform("material.AO", material->AO);

	shader->updateUniform("projection", camera.GetProjectionMatrix());
	shader->updateUniform("view", camera.GetViewMatrix());
	shader->updateUniform("model", modelMat);
	shader->setAllUniforms();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_num, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
