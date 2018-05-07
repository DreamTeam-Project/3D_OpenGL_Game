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

void getStringFromFile(ifstream& fin, double& ret) {
	string buf = "";
	fin >> buf;
	if (buf != "<" || fin.eof()) {
		throw GameException(__LINE__, __func__, "error format \">\" ");
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		try {
			ret = std::stod(buf);
		}
		catch (exception& e) {
			throw GameException(__LINE__, __func__, "error format \"int\" ", e.what());
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

void InitIdentity(aiMatrix4x4& matrix) {
	matrix.a1 = 1.0f;	matrix.a2 = 0.0f;	matrix.a3 = 0.0f;	matrix.a4 = 0.0f;
	matrix.b1 = 0.0f;	matrix.b2 = 1.0f;	matrix.b3 = 0.0f;	matrix.b4 = 0.0f;
	matrix.c1 = 0.0f;	matrix.c2 = 0.0f;	matrix.c3 = 1.0f;	matrix.c4 = 0.0f;
	matrix.d1 = 0.0f;	matrix.d2 = 0.0f;	matrix.d3 = 0.0f;	matrix.d4 = 1.0f;
}

void InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ, aiMatrix4x4& matrix) {
	matrix.a1 = ScaleX;	matrix.a2 = 0.0f;   matrix.a3 = 0.0f;	matrix.a4 = 0.0f;
	matrix.b1 = 0.0f;	matrix.b2 = ScaleY; matrix.b3 = 0.0f;	matrix.b4 = 0.0f;
	matrix.c1 = 0.0f;	matrix.c2 = 0.0f;   matrix.c3 = ScaleZ;	matrix.c4 = 0.0f;
	matrix.d1 = 0.0f;	matrix.d2 = 0.0f;   matrix.d3 = 0.0f;	matrix.d4 = 1.0f;
}

void InitTranslationTransform(float x, float y, float z, aiMatrix4x4& matrix) {
	matrix.a1 = 1.0f;	matrix.a2 = 0.0f;	matrix.a3 = 0.0f;	matrix.a4 = x;
	matrix.b1 = 0.0f;	matrix.b2 = 1.0f;	matrix.b3 = 0.0f;	matrix.b4 = y;
	matrix.c1 = 0.0f;	matrix.c2 = 0.0f;	matrix.c3 = 1.0f;	matrix.c4 = z;
	matrix.d1 = 0.0f;	matrix.d2 = 0.0f;	matrix.d3 = 0.0f;	matrix.d4 = 1.0f;
}