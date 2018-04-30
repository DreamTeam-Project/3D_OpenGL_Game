#include "System.h"

void Vertex::addvec(vec3 a) {
	Position += a;
	Normal += a;
	Tangent+= a;
	Bitangent+=a;
	TexCoords += glm::vec2(a.x, a.y);
}

bool IsItNumber(const string& word) {
	size_t i = 0;
	if (word[0] == '-') {
		i++;
	}
	while (word[i] != '\0') {
		if (!isdigit(word[i++]))
			return false;
	}
	return true;
}

string vec3_toString(const vec3& a) {
	return to_string(a.x) + string(" ") + to_string(a.y) + string(" ") + to_string(a.z);
}

int doNothing() {
	return SUCCESS;
}

void print(const char* what) {
	if (_DEBUG) {
		printf("%s\n", what);
	}
	else {
		doNothing();
	}
}

void print(const string& what) {
	if (_DEBUG) {
		printf("%s\n", what.c_str());
	}
	else {
		doNothing();
	}
}

void getStringFromFile(ifstream& fin, string& ret) {
	string buf = "";
	ret = "";
	fin >> buf;
	if (fin.eof()) {
		ret = "end_of_file";
		return;
	}
	if (buf != "<") {
		throw GameException(__LINE__, __FILE__, "error format \">\" ");
	}
	fin >> buf;
	while (buf != ">" || fin.eof()) {
		if (buf == ">") {
			break;
		}
		ret = ret + string(" ") + buf;
		fin >> buf;
	}
	if (buf != ">" && fin.eof()) {
		throw GameException(__LINE__, __FILE__, "error format >");
	}
	if (buf == ">" && fin.eof() && ret == "") {
		ret = "end_of_file";
		return;
	}
	if (buf == ">" && !fin.eof() && ret == "") {
		ret = "null";
		return;
	}
	ret = ret.substr(1);
	return;
}

void getStringFromFile(ifstream& fin, int& ret) {
	string buf = "";
	fin >> buf;
	if (buf != "<" || fin.eof()) {
		throw GameException(__LINE__, __FILE__, "error format \">\" ");
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		if (IsItNumber(buf)) {
			ret = std::stoi(buf);
		}
		else {
			throw GameException(__LINE__, __FILE__, "error format \"int\" ");
		}
	}
	if (buf == ">") {
		ret = 0;
		return;
	}
	fin >> buf;
	if (buf != ">") {
		throw GameException(__LINE__, __FILE__, "error format \">\" ");
	}
}

void getStringFromFile(ifstream& fin, vec3& ret) {
	string buf = "";
	fin >> buf;
	if (buf != "<" || fin.eof()) {
		throw GameException(__LINE__, __FILE__, "error format \">\" ");
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		ret.x = std::stof(buf);
	}
	else {
		ret.x = 0;
		ret.y = 0;
		ret.z = 0;
		return;
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		ret.y = std::stof(buf);
	}
	else {
		ret.y = 0;
		ret.z = 0;
		return;
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		ret.z = std::stof(buf);
	}
	else {
		ret.z = 0;
		return;
	}
	fin >> buf;
	if (buf != ">") {
		throw GameException(__LINE__, __FILE__, "error format \">\" ");
	}
}
