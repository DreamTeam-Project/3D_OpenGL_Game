#include "System.h"

void SetZero(aiMatrix4x4* matrix) {
	memset(matrix, 0, sizeof(*matrix));
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
		ofstream fout("log.file");
		if (!fout.is_open()) {
			throw;
		}
		else {
			doNothing();
		}
		fout << what << std::endl;
		fout.close();
	}
}

void print(const string& what) {
	if (_DEBUG) {
		printf("%s\n", what.c_str());
	}
	else {
		ofstream fout("log.file");
		if (!fout.is_open()) {
			throw;
		}
		else {
			doNothing();
		}
		fout << what << std::endl;
		fout.close();
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
		throw GameException(__LINE__, __func__, "error format \">\" ");
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
		throw GameException(__LINE__, __func__, "error format >");
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
		throw GameException(__LINE__, __func__, "error format \">\" ");
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		if (IsItNumber(buf)) {
			ret = std::stoi(buf);
		}
		else {
			throw GameException(__LINE__, __func__, "error format \"int\" ");
		}
	}
	if (buf == ">") {
		ret = 0;
		return;
	}
	fin >> buf;
	if (buf != ">") {
		throw GameException(__LINE__, __func__, "error format \">\" ");
	}
}

void getStringFromFile(ifstream& fin, vec3& ret) {
	string buf = "";
	fin >> buf;
	if (buf != "<" || fin.eof()) {
		throw GameException(__LINE__, __func__, "error format \">\" ");
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
		throw GameException(__LINE__, __func__, "error format \">\" ");
	}
}

int ArraySizeInElements(uint* a) {
	return sizeof(a) / sizeof(a[0]);
}

void InitIdentity(aiMatrix4x4& matrix) {
	/*matrix[0][0] = 1.0f;	matrix[0][1] = 0.0f;	matrix[0][2] = 0.0f;	matrix[0][3] = 0.0f;
	matrix[1][0] = 0.0f;	matrix[1][1] = 1.0f;	matrix[1][2] = 0.0f;	matrix[1][3] = 0.0f;
	matrix[2][0] = 0.0f;	matrix[2][1] = 0.0f;	matrix[2][2] = 1.0f;	matrix[2][3] = 0.0f;
	matrix[3][0] = 0.0f;	matrix[3][1] = 0.0f;	matrix[3][2] = 0.0f;	matrix[3][3] = 1.0f;*/
}

void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ, aiMatrix4x4& m) {
	/*m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
	m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
	m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;*/
}

void InitTranslationTransform(float x, float y, float z, aiMatrix4x4& m) {
	/*m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;*/
}