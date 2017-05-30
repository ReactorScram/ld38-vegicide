#include "colorado/gl.h"
#include "colorado/gl-error.h"
#include "colorado/gl-shader.h"

#include <cstring>

using glm::mat3;
using glm::mat4;
using glm::vec3;
using glm::vec4;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <vector>
using std::vector;

using std::string;

namespace Colorado {
ShaderCompileResult::ShaderCompileResult () {
	compileSuccess = false;
}

ProgramLinkResult::ProgramLinkResult () {
	linkSuccess = false;
}

bool ProgramLinkResult::isSuccessful () const {
	return linkSuccess && vertexResult.compileSuccess && fragmentResult.compileSuccess;
}

Shader::Shader () {
	vertex = 0;
	fragment = 0;
	program = 0;
	
	attribPos = -1;
	attribTexCoord = -1;
}

Shader::~Shader () {
	removeAllShaders ();
}

ProgramLinkResult Shader::loadProgramFromBuffers (const std::vector<uint8_t> & vertBuffer, const std::vector<uint8_t> & fragBuffer) {
	return loadProgramFromBuffers ((const char *)vertBuffer.data (), vertBuffer.size (), (const char *)fragBuffer.data (), fragBuffer.size ());
}

ProgramLinkResult Shader::loadProgramFromBuffers (const char * vertSource, size_t vertLen, const char * fragSource, size_t fragLen) {
	ProgramLinkResult result;
	
	vertex = glCreateShader (GL_VERTEX_SHADER);
	fragment = glCreateShader (GL_FRAGMENT_SHADER);
	
	if (vertex == 0 || fragment == 0) {
		result.linkSuccess = false;
		result.infoLog = "glCreateShader failed";
		return result;
	}
	
	result.vertexResult = compileShader (vertex, vertSource, vertLen);
	if (! result.vertexResult.compileSuccess) {
		result.infoLog = result.vertexResult.infoLog;
		result.linkSuccess = false;
		return result;
	}
	
	result.fragmentResult = compileShader (fragment, fragSource, fragLen);
	if (! result.fragmentResult.compileSuccess) {
		result.infoLog = result.fragmentResult.infoLog;
		result.linkSuccess = false;
		return result;
	}
	
	program = glCreateProgram ();
	
	//checkGlError ("Created shader program");
	
	if (program == 0) {
		result.linkSuccess = false;
		result.infoLog = "glCreateProgram failed";
		return result;
	}
	
	glAttachShader (program, vertex);
	glAttachShader (program, fragment);
	
	//checkGlError ("Attached shaders to program");
	
	glLinkProgram (program);
	//checkGlError ("Linked program");
	
	GLint linked = 0;
	glGetProgramiv (program, GL_LINK_STATUS, &linked);
	
	//checkGlError ("Checked program link status");
	
	if (linked == (GLint)GL_FALSE) {
		result.linkSuccess = false;
		
		vector <char> buffer;
		GLsizei maxLength = 0;
		glGetProgramiv (program, GL_INFO_LOG_LENGTH, &maxLength);
		
		buffer.resize (maxLength);
		
		glGetProgramInfoLog (program, buffer.size (), &maxLength, buffer.data ());
		
		result.infoLog = string (buffer.data (), buffer.size ());
	}
	else {
		result.linkSuccess = true;
		result.infoLog = "Successfully linked";
		
		attribPos = attributeLocation ("attribPos");
		attribTexCoord = attributeLocation ("attribTexCoord");
	}
	
	return result;
}

void Shader::removeAllShaders () {
	glDeleteProgram (program);
	glDeleteShader (vertex);
	glDeleteShader (fragment);
	
	vertex = 0;
	fragment = 0;
	program = 0;
}

void Shader::setUniformValue (GLint l, GLint i) {
	glUniform1i (l, i);
}

void Shader::setUniformValue (GLint l, GLfloat f) {
	glUniform1f (l, f);
}

void Shader::setUniformValue (GLint l, glm::vec2 v) {
	glUniform2f (l, v [0], v [1]);
}

void Shader::setUniformValue (GLint l, const glm::vec3 & v) {
	glUniform3f (l, v [0], v [1], v [2]);
}

/*
void Shader::setUniformValueArray (GLint location, const glm::mat3 * mats, GLuint count) {
	GLfloat outData [9 * MAX_MATRIX_COUNT];
	
	int i = 0;
	
	for (int matIndex = 0; matIndex < count && matIndex < MAX_MATRIX_COUNT; ++matIndex) {
		const MatrixType * inData = mats [matIndex].constData ();
		
		for (int j = 0; j < 3 * 3; ++j, ++i) {
			outData [i] = inData [j];
		}
	}
	
	glUniformMatrix3fv (location, count, false, outData);
}

void Shader::setUniformValueArray (GLint location, const glm::mat4 * mats, GLuint count) {
	GLfloat outData [16 * MAX_MATRIX_COUNT];
	
	int i = 0;
	
	for (int matIndex = 0; matIndex < count && matIndex < MAX_MATRIX_COUNT; ++matIndex) {
		const MatrixType * inData = mats [matIndex].constData ();
		
		for (int j = 0; j < 4 * 4; ++j, ++i) {
			outData [i] = inData [j];
		}
	}
	glUniformMatrix4fv (location, count, false, outData);
}
*/
void Shader::setUniformValue (GLint l, const glm::vec4 & v) {
	glUniform4f (l, v [0], v [1], v [2], v [3]);
}

// TODO: This may not work for an actual matrix array, need to test it
void Shader::setUniformValueArray (GLint location, const mat3 * mats, GLuint count) {
	glUniformMatrix3fv (location, count, false, &mats [0][0][0]);
}

void Shader::setUniformValueArray (GLint location, const glm::mat4 * mats, GLuint count) {
	glUniformMatrix4fv (location, count, false, &mats [0][0][0]);
}

GLint Shader::attributeLocation (const char * name) const {
	return glGetAttribLocation (program, name);
}

GLint Shader::uniformLocation (const char * name) const {
	return glGetUniformLocation (program, name);
}

ShaderCompileResult Shader::compileShader (GLuint shader, const vector <uchar> & source) {
	shaderSource (shader, source);
	return compileShader (shader);
}

ShaderCompileResult Shader::compileShader (GLuint shader, const char * source, size_t sourceLen) {
	shaderSource (shader, source, sourceLen);
	return compileShader (shader);
}

ShaderCompileResult Shader::compileShader (GLuint shader) {
	//checkGlError ("Entering compileShader");
	
	ShaderCompileResult result;
	
	glCompileShader (shader);
	
	//checkGlError ("Compiled shader");
	
	GLint compiled = 0;
	glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);
	
	//checkGlError ("Checked shader status");
	
	result.compileSuccess = bool (compiled);
	
	if (compiled == (GLint)GL_FALSE) {
		vector <char> buffer;
		
		GLsizei length = 0;
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &length);
		buffer.resize (length);
		
		glGetShaderInfoLog (shader, buffer.size (), &length, buffer.data 
		());
		
		result.infoLog = string (buffer.data (), buffer.size ());
	}
	else {
		result.infoLog = "Successfully compiled";
	}
	
	return result;
}

void Shader::shaderSource (GLuint shader, const std::vector<uchar> & source) {
	shaderSource (shader, (const char *)source.data (), source.size ());
}

void Shader::shaderSource (GLuint shader, const char * source, size_t sourceLen) {
	// Need a vector here in case the desktop preamble is added
	vector <const char *> data;
	vector <int> lengths;
	
	#ifdef USE_DESKTOP_GL
	const char * s (
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
	"#line 0\n"
	);
	data.push_back (s);
	lengths.push_back (strlen (s));
	#endif
	
	data.push_back (source);
	lengths.push_back (sourceLen);
	
	if (sourceLen > 0) {
		glShaderSource (shader, data.size (), &data [0], &lengths [0]);
	}
	else {
		cout << "No source to load shader from" << endl;
	}
}
}
