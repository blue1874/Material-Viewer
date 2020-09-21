#include "fbo.h"

ppFBO::ppFBO(size_t _width, size_t _height, std::shared_ptr<Shader> _shader) : FBO(_width, _height, _shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	if (MSAA.selected_value > 1) {
		glGenTextures(1, &MSAATex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA.selected_value, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glGenRenderbuffers(1, &MSAARbo);
		glBindRenderbuffer(GL_RENDERBUFFER, MSAARbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA.selected_value, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAATex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, MSAARbo);

	}


	//GL_FRAMEBUFFER_UNDEFINED;
	//GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
	//GL_FRAMEBUFFER_UNSUPPORTED;
	//GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;
	auto state = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//assert(state != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	assert(state == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void ppFBO::switchMSAA()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	if (MSAA.selected_value > 1)
	{
		glDeleteTextures(1, &MSAATex);
		glGenTextures(1, &MSAATex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATex);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA.selected_value, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

		glDeleteBuffers(1, &MSAARbo);
		glGenRenderbuffers(1, &MSAARbo);
		glBindRenderbuffer(GL_RENDERBUFFER, MSAARbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA.selected_value, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAATex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, MSAARbo);
	}
	else
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

}

ppFBO::~ppFBO()
{
	glDeleteBuffers(1, &MSAARbo);
	glDeleteTextures(1, &MSAATex);
}

void ppFBO::draw()
{
	shader->use();
	glActiveTexture(GL_TEXTURE0 + 0);
	shader->updateUniform("fbo", 0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE0 + 1);
	shader->updateUniform("MSAATex", 1);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATex);

	shader->updateUniform("sampleNum", MSAA.selected_value);
	shader->updateUniform("time", float(glfwGetTime()));
	shader->updateUniform("width", (int)width);
	shader->updateUniform("height", (int)height);
	shader->updateUniform("gamma", gammaCorrection.selected_value);
	shader->setAllUniforms();
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void ppFBO::drawGui()
{
	MSAA.toGui();
	if (MSAA.selected_index != last_index) {
		last_index = MSAA.selected_index;
		switchMSAA();
	}
	gammaCorrection.toGui();
}

inline FBO::FBO(size_t _width, size_t _height, std::shared_ptr<Shader> _shader) : width(_width), height(_height), shader(_shader)
{
	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	float quadVertices[24] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//GL_FRAMEBUFFER_UNDEFINED;
	//GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
	//GL_FRAMEBUFFER_UNSUPPORTED;
	//GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;
	auto state = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//assert(state != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	assert(state == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
}

inline FBO::~FBO()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &id);
	glDeleteBuffers(1, &rbo);
	glDeleteTextures(1, &texture);
}

void FBO::draw()
{
}

bool cubeMapFBO::first = true;
cubeMapFBO::cubeMapFBO(size_t _width, size_t _height, std::shared_ptr<Shader> _shader) : FBO(_width, _height, _shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glGenTextures(1, &IBLmap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IBLmap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//cubemap = load_cubemap(Cubemap::faces);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	auto state = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//assert(state != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	assert(state == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

cubeMapFBO::~cubeMapFBO()
{
	first = true;
	//glDeleteTextures(1, &cubemap);
	glDeleteTextures(1, &IBLmap);
	glDeleteVertexArrays(1, &cubeVAO);
}

void cubeMapFBO::draw()
{
	if (first) first = false;
	else return;
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.45f, 0.55f, 0.30f, 1.00f);
	shader->use();
	shader->updateUniform("environmentMap", 0);
	shader->updateUniform("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.

	for (unsigned int i = 0; i < 6; ++i)
	{
		shader->updateUniform("view", captureViews[i]);
		shader->setAllUniforms();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, IBLmap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				// bottom face
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				// top face
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			GLuint cubeVBO;
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		unsigned char* data = (unsigned char*)malloc(32 * 32 * 3);
		glReadPixels(0, 0, 32, 32, GL_RGB, GL_UNSIGNED_BYTE, data);
		save_texture2png(DefaultWorkFlow::CAL_IBL_PATH + std::to_string(i) + ".png", 32, 32, 3, data);
		free(data);
	}
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
