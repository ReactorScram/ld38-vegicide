#include "colorado/read-file.h"

#include <fstream>

using namespace std;

typedef unsigned char uchar;

namespace Colorado {

vector <uchar> ReadFile::readFile (string fn) {
	ifstream file;
	file.open (fn);
	
	if (! file.is_open ()) {
		return vector <uchar> ();
	}
	
	file.seekg (0, ios_base::end);
	
	size_t fileLength = file.tellg ();
	file.seekg (0, ios_base::beg);
	
	vector <uchar> buffer;
	buffer.resize (fileLength);
	
	file.read ((char *)buffer.data (), fileLength);
	
	file.close ();
	
	return buffer;
}

}
