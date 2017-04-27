#ifndef READFILE_H
#define READFILE_H

#include "colorado/dllspec.h"
#include <string>
#include <vector>

namespace Colorado {

class COLORADO_DLLSPEC ReadFile
{
public:
	static std::vector <unsigned char> readFile (std::string fn);
};

}

#endif // READFILE_H
