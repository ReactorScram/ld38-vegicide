#include "colorado/triangle-shader.h"

#include <iostream>
using std::cerr;
using std::endl;

#include "colorado/gl.h"
#include "colorado/gl-error.h"

using std::map;
using std::string;
using std::vector;

using namespace Colorado;

namespace Colorado {
TriangleShader::TriangleShader () {
	//ready = false;
}

ProgramLinkResult TriangleShader::loadFromBuffers (
	const vector <uchar> & vertSource, const vector <uchar> & fragSource) 
{
	ProgramLinkResult plr = program.loadProgramFromBuffers (
		vertSource, fragSource);
	
	if (plr.linkSuccess) {
		reloadAttributeLocations ();
		reloadUniformLocations ();
	}
	
	return plr;
}

GLint TriangleShader::attributeLocation (string name) const {
	map <string, GLint>::const_iterator i = attributeLocations.find (name);
	
	if (i == attributeLocations.end ()) {
		return -1;
	}
	else {
		return (*i).second;
	}
}

GLint TriangleShader::uniformLocation (string name) const {
	map <string, GLint>::const_iterator i = uniformLocations.find (name);
	
	if (i == uniformLocations.end ()) {
		return -1;
	}
	else {
		return (*i).second;
	}
}

void TriangleShader::reloadAttributeLocations () {
	attributeLocations.clear ();
	
	//checkGlError ("Loading attributes");
	
	GLint numAttribs = 0;
	glGetProgramiv (program.program, GL_ACTIVE_ATTRIBUTES, &numAttribs);
	
	for (int i = 0; i < numAttribs; i++) {
		const GLsizei NAME_BUFFER_SIZE = 1024;
		char nameBuffer [NAME_BUFFER_SIZE];
		
		GLsizei nameLength = 0;
		
		GLint size = 0;
		GLenum type = GL_FLOAT;
		
		glGetActiveAttrib (program.program, i, NAME_BUFFER_SIZE, &nameLength, &size, &type, nameBuffer);
		
		string name (nameBuffer, nameLength);
		attributeLocations [name] = program.attributeLocation (name.c_str ());
	}
	
	//checkGlError ("Automatically found attributes");
	
	vertPosAttribute = attributeLocation ("attribPos");
	vertNormAttribute = attributeLocation ("attribNormal");
	vertMatrixIndexAttribute = attributeLocation ("attribMatrixIndex");
	vertTexCoordAttribute = attributeLocation ("attribTexCoord");
	
	//cerr << "vertPosAttribute: " << vertPosAttribute << endl;
	
	vertColorAttribute = attributeLocation ("attribColor");
	
	//checkGlError ("Manually found attributes");
	
	// NOTE: This is going to be a sticking point once we have multiple 
	// attribute sets
	/*
	glEnableVertexAttribArray (vertPosAttribute);
	glEnableVertexAttribArray (vertNormAttribute);
	glEnableVertexAttribArray (vertMatrixIndexAttribute);
	glEnableVertexAttribArray (vertTexCoordAttribute);
	
	checkGlError ("Enabled attributes");
	*/
	//glEnableVertexAttribArray (vertColorAttribute);
}

void TriangleShader::reloadUniformLocations () {
	uniformLocations.clear ();
	
	GLint numUniforms = 0;
	glGetProgramiv (program.program, GL_ACTIVE_UNIFORMS, &numUniforms);
	
	for (int i = 0; i < numUniforms; i++) {
		const GLsizei NAME_BUFFER_SIZE = 1024;
		char nameBuffer [NAME_BUFFER_SIZE];
		
		GLsizei nameLength = 0;
		
		GLint size = 0;
		GLenum type = GL_FLOAT;
		
		glGetActiveUniform (program.program, i, NAME_BUFFER_SIZE, &nameLength, &size, &type, nameBuffer);
		
		string name (nameBuffer, nameLength);
		uniformLocations [name] = program.uniformLocation (name.c_str ());
	}
	
	//checkGlError ("Automatically found uniforms");
	
	matrixUniform = uniformLocation ("mvpMatrix");
	normalMatrixUniform = uniformLocation ("normalMatrix[0]");
	textureUniform = uniformLocation ("textureDiffuse");
	
	//checkGlError ("Manually found uniforms");
}

bool TriangleShader::bind () const {
	glUseProgram (program.program);
	return true;
}

void TriangleShader::setMatrixUniform (const glm::mat4 & matrix, GLint uniform) {
	Shader::setUniformValueArray (uniform, &matrix, 1);
}

void TriangleShader::setMvpMatrix (const glm::mat4 & matrix) const {
	setMatrixUniform (matrix, matrixUniform);
}

void TriangleShader::setMvpMatrices (const glm::mat4 * mats, int count) {
	Shader::setUniformValueArray (matrixUniform, mats, count);
}

void TriangleShader::setNormalMatrix (const glm::mat3 & matrix) {
	Shader::setUniformValueArray (normalMatrixUniform, &matrix, 1);
}

void TriangleShader::setNormalMatrices (const glm::mat3 * mats, int count) {
	Shader::setUniformValueArray (normalMatrixUniform, mats, count);
}
}
