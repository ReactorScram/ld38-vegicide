#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

using namespace std;

int main () {
	vector <float> inputs {
		0, 45, 90, 135, 180,
	};
	
	for (float f: inputs) {
		float rads = glm::radians (f);
		float s = glm::sin (rads);
		float a = glm::abs (s);
		cout << f << ", " << rads << ", " << s << ", " << a << endl;
	}
}
