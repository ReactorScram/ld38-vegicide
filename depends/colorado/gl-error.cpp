#include "colorado/gl-error.h"

#include <iostream>
using std::cerr;
using std::endl;
using std::string;

#include "colorado/gl.h"

namespace Colorado {
void checkGlError (const string & text) {
	auto err = glGetError ();
	
	string errString ("Unknown");
	switch (err) {
		case 0:
			errString = "0";
			break;
		case GL_INVALID_VALUE:
			errString = "Invalid value";
			break;
		case GL_INVALID_ENUM:
			errString = "Invalid enum";
			break;
		case GL_INVALID_OPERATION:
			errString = "Invalid operation";
			break;
		default:
			// >_>
			break;
	}
	
	cerr << "glError " << errString << " " << text << endl;
}
}
