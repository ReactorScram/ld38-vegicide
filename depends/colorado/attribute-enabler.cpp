#include "colorado/attribute-enabler.h"

#include <iostream>
using std::cerr;
using std::endl;

#include "colorado/gl.h"

namespace Colorado {

AttributeEnabler::AttributeEnabler () {
	
}

AttributeEnabler::~AttributeEnabler () {
	for (auto it = currentAttribs.begin (); it != currentAttribs.end (); 
	it++) 
	{
		glDisableVertexAttribArray (*it);
	}
}

void AttributeEnabler::enableAttributes (const std::unordered_set 
<GLint> & 
attribList)
{
	for (auto it = currentAttribs.begin (); it != currentAttribs.end (); 
	it++) 
	{
		if (attribList.count (*it) >= 1) {
			// Leave it enabled
		}
		else if (*it >= 0) {
			glDisableVertexAttribArray (*it);
			//cerr << "Disabled attrib " << *it << endl;
		}
	}
	
	for (auto it = attribList.begin (); it != attribList.end (); it++) {
		if (currentAttribs.count (*it) >= 1) {
			// Already enabled
		}
		else if (*it >= 0) {
			glEnableVertexAttribArray (*it);
			//cerr << "Enabled attrib " << *it << endl;
		}
	}
	
	currentAttribs = attribList;
}

}
