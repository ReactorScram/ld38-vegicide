#ifndef ATTRIBUTE_ENABLER_H
#define ATTRIBUTE_ENABLER_H

#include <unordered_set>

#include <colorado/gl.h>

namespace Colorado {

class COLORADO_DLLSPEC AttributeEnabler {
public:
	AttributeEnabler ();
	~AttributeEnabler ();
	
	void enableAttributes (const std::unordered_set <GLint> & attribList);
	
protected:
	std::unordered_set <GLint> currentAttribs;
};

}

#endif
