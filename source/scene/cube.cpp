#include "cube.h"

CubeModel::CubeModel(glm::vec3 offset)
{
	std::vector<glm::vec3> pos{
        glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),

		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f),
	};
	for (auto i : pos) i += offset;
    std::vector<size_t> index{
		0, 1, 2,
		0, 2, 3,
		0, 1, 5,
		0, 5, 4,
		0, 7, 4,
		0, 3, 7,
		6, 3, 2,
		6, 7, 3,
		6, 2, 1,
		6, 1, 5,
		6, 4, 7,
		6, 5, 4
	};
	index_num = index.size();

	GLuint VBO, IBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// allocate enough space to contain data
	auto pos_size = pos.size() * sizeof(glm::vec3);
	auto index_size = index.size() * sizeof(size_t);
	glBufferData(GL_ARRAY_BUFFER, pos_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, pos_size, &pos[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, &index[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

CubeModel::~CubeModel()
{
}

void CubeModel::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_num, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CubeModel::draw(std::shared_ptr<Shader> shader, Camera& camera)
{
	static bool first = true;
	if (first) {
		texture = load_cubemap(Cubemap::faces);
		first = false;
	}
	shader->use();
	shader->updateUniform("skybox", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	shader->updateUniform("projection", camera.GetProjectionMatrix());
	shader->updateUniform("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
	shader->setAllUniforms();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_num, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
