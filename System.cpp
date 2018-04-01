#include "System.h"

void PrintException(const string& exc) {
	ofstream error_log("error.log", ios_base::out | ios_base::app);
	error_log << &exc << std::endl;
	error_log.close();
}
void Vertex::addvec(vec3 a) {
	Position += a;
	Normal += a;
	Tangent+= a;
	Bitangent+=a;
	TexCoords += glm::vec2(a.x, a.y);
}