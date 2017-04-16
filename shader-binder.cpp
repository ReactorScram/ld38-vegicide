#include "shader-binder.h"

#include <iostream>

using namespace std;
using namespace Terf;
using namespace Colorado;

TriangleShader * newShader (
	const Archive & terf, 
	const string & vertFn, 
	const string & fragFn)
{
	auto shader = new TriangleShader ();
	auto result = shader->loadFromBuffers (terf.lookupFile (vertFn), 
	terf.lookupFile (fragFn));
	
	if (! result.linkSuccess) {
		cerr << "Vert: " << result.vertexResult.infoLog << endl;
		cerr << "Frag: " << result.fragmentResult.infoLog << endl;
		cerr << "Shader: " << result.infoLog << endl;
	}
	
	return shader;
}

ShaderBinder::ShaderBinder () :
	m_currentShader (nullptr),
	shaders ()
{
	
}

void ShaderBinder::addShader (const ShaderKey code, Colorado::TriangleShader * shader) {
	shaders [code] = std::unique_ptr <Colorado::TriangleShader> (shader);
}

const Colorado::TriangleShader * ShaderBinder::currentShader () const {
	return m_currentShader;
}

void ShaderBinder::bind (const ShaderKey code) {
	bind (getShader (code));
}

void ShaderBinder::bind (const Colorado::TriangleShader * shader) {
	m_currentShader = shader;
	m_currentShader->bind ();
}

const Colorado::TriangleShader * ShaderBinder::getShader (const ShaderKey code) {
	const Colorado::TriangleShader * result = shaders.at (code).get ();
	if (result) {
		return result;
	}
	else {
		//cerr << "Warning: Failed to get shader - " << code << endl;
		return result;
	}
}
