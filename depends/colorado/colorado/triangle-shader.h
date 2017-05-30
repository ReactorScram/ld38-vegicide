#ifndef TRIANGLESHADER_H
#define TRIANGLESHADER_H

#include <map>
#include <string>

#include <glm/glm.hpp>

#include "colorado/dllspec.h"
#include "colorado/gl-shader.h"

namespace Colorado {
class COLORADO_DLLSPEC TriangleShader {
public:
	Shader program;
	
	GLint vertPosAttribute;
	GLint vertNormAttribute;
	GLint vertMatrixIndexAttribute;
	GLint vertTexCoordAttribute;
	
	GLint vertColorAttribute;
	
	// MVP
	GLint matrixUniform;
	GLint normalMatrixUniform;
	GLint textureUniform;
	
	TriangleShader ();
	
	ProgramLinkResult loadFromBuffers (const std::vector <uchar> &, const std::vector <uchar> &);
	
	GLint attributeLocation (std::string) const;
	GLint uniformLocation (std::string) const;
	
	void reloadAttributeLocations ();
	void reloadUniformLocations ();
	
	void removeAllShaders ();
	
	bool bind () const;
	
	static void setMatrixUniform (const glm::mat4 &, GLint);
	
	void setMvpMatrix (const glm::mat4 &) const;
	void setMvpMatrices (const glm::mat4 *, int);
	void setNormalMatrix (const glm::mat3 &);
	void setNormalMatrices (const glm::mat3 *, int);
	
protected:
	std::map <std::string, GLint> attributeLocations;
	std::map <std::string, GLint> uniformLocations;
};
}

#endif // TRIANGLESHADER_H
