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
