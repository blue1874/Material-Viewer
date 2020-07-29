#include "fbo.h"

FBO::FBO()
{
}

FBO::FBO(size_t _width, size_t _height, bool enable_msaa, size_t _sample_num) : width(_width), height(_height), enableMSAA(enable_msaa), sample_num(_sample_num)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &MSAATex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATex);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample_num, GL_RGBA, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);


	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &MSAARbo);
	glBindRenderbuffer(GL_RENDERBUFFER, MSAARbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample_num, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (enableMSAA) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MSAATex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, MSAARbo);

	}
	else {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	}


	//GL_FRAMEBUFFER_UNDEFINED;
	//GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
	//GL_FRAMEBUFFER_UNSUPPORTED;
	//GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS;
	auto state = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//assert(state != GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
	assert(state == GL_FRAMEBUFFER_COMPLETE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

void FBO::switchMSAA()
{
	enableMSAA = enableMSAA ? false : true;
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	if (enableMSAA)
	{
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

FBO::~FBO()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &id);
	glDeleteBuffers(1, &rbo);
	glDeleteBuffers(1, &MSAARbo);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &MSAATex);
}

void FBO::draw(std::shared_ptr<Shader> shader)
{
	shader->use();
	glActiveTexture(GL_TEXTURE0 + 0);
	shader->updateUniform("fbo", 0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE0 + 1);
	shader->updateUniform("MSAATex", 1);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MSAATex);

	shader->updateUniform("enableMSAA", enableMSAA);
	shader->updateUniform("time", float(glfwGetTime()));
	shader->updateUniform("width", (int)width);
	shader->updateUniform("height", (int)height);
	shader->setAllUniforms();
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
