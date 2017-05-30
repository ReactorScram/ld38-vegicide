#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "colorado/dllspec.h"

namespace Colorado {

typedef unsigned char uchar;

typedef unsigned int GLuint;
typedef int GLint;
typedef float GLfloat;

class COLORADO_DLLSPEC ShaderCompileResult {
public:
	bool compileSuccess;
	std::string infoLog;
	
	ShaderCompileResult ();
};

class COLORADO_DLLSPEC ProgramLinkResult {
public:
	bool linkSuccess;
	ShaderCompileResult vertexResult;
	ShaderCompileResult fragmentResult;
	std::string infoLog;
	
	ProgramLinkResult ();
	
	bool isSuccessful () const;
};

class COLORADO_DLLSPEC Shader {
public:
	GLuint vertex;
	GLuint fragment;
	GLuint program;
	
	// For compatibility with UiVboNode
	GLint attribPos;
	GLint attribTexCoord;
	
	static const int MAX_MATRIX_COUNT = 16;
	
	Shader ();
	~Shader ();
	
	ProgramLinkResult loadProgramFromBuffers (const std::vector <uint8_t> &, const std::vector <uint8_t> &);
	ProgramLinkResult loadProgramFromBuffers (const char *, size_t, const char *, size_t);
	void removeAllShaders ();
	
	static void setUniformValue (GLint, GLint);
	static void setUniformValue (GLint, GLfloat);
	static void setUniformValue (GLint, glm::vec2);
	static void setUniformValue (GLint, const glm::vec3 &);
	static void setUniformValue (GLint, const glm::vec4 &);
	
	static void setUniformValueArray (GLint, const glm::mat3 *, GLuint);
	static void setUniformValueArray (GLint, const glm::mat4 *, GLuint);

	GLint attributeLocation (const char *) const;
	GLint uniformLocation (const char *) const;
	
	static ShaderCompileResult compileShader (GLuint);
	static ShaderCompileResult compileShader (GLuint, const std::vector <uchar> &);
	static ShaderCompileResult compileShader (GLuint, const char *, size_t);
	static void shaderSource (GLuint, const std::vector <uchar> &);
	static void shaderSource (GLuint, const char *, size_t);
};
}

#endif
