#pragma once
#ifndef DEBUG_H
#define DEBUG_H


#include <iostream>
#include <string>
#include <glad/glad.h>

#define glCheckError() glCheckError_(__FILE__, __LINE__) 
GLenum glCheckError_(const char *file, int line);
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *message,
	void *userParam);
#endif // !DEBUG_H