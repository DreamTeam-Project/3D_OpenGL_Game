#include "System.h"

void CheckError(uint line, const char* func) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		string error;
		switch (errorCode) {
			case GL_INVALID_ENUM:                  
				error = "INVALID_ENUM"; 
				break;
			case GL_INVALID_VALUE:                 
				error = "INVALID_VALUE"; 
				break;
			case GL_INVALID_OPERATION:             
				error = "INVALID_OPERATION"; 
				break;
			case GL_STACK_OVERFLOW:                
				error = "STACK_OVERFLOW"; 
				break;
			case GL_STACK_UNDERFLOW:               
				error = "STACK_UNDERFLOW"; 
				break;
			case GL_OUT_OF_MEMORY:                 
				error = "OUT_OF_MEMORY"; 
				break;
			default:
				error = "DEFAULT_ERROR";
		}
		throw GameException(line, func, error);
	}
	print("no error gl");
}

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
	if (TERMINAL) {
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
	if (TERMINAL) {
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

void ReadFromFile(ifstream& fin, const string& what, string& ret) {
	getStringFromFile(fin, ret);
	if (ret != what || ret == "end_of_file") {
		throw GameException(__LINE__, __func__, "Error: error type - string");
	}
	getStringFromFile(fin, ret);
}

void ReadFromFile(ifstream& fin, const string& what, int& ret) {
	string buf;
	getStringFromFile(fin, buf);
	if (buf != what || buf == "end_of_file") {
		throw GameException(__LINE__, __func__, "Error: error type - int");
	}
	getStringFromFile(fin, ret);
}

void ReadFromFile(ifstream& fin, const string& what, double& ret) {
	string buf;
	getStringFromFile(fin, buf);
	if (buf != what || buf == "end_of_file") {
		throw GameException(__LINE__, __func__, "Error: error type - double");
	}
	getStringFromFile(fin, ret);
}

void ReadFromFile(ifstream& fin, const string& what, vec3& ret) {
	string buf;
	getStringFromFile(fin, buf);
	if (buf != what || buf == "end_of_file") {
		throw GameException(__LINE__, __func__, "Error: error type - vec3");
	}
	getStringFromFile(fin, ret);
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
		throw GameException(__LINE__, __func__, "Error: error format \">\" ", "string");
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
		throw GameException(__LINE__, __func__, "error format >", "string");
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
		throw GameException(__LINE__, __func__, "error format \">\" ", "int");
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
		throw GameException(__LINE__, __func__, "error format \">\" ", "int");
	}
}

void getStringFromFile(ifstream& fin, vec3& ret) {
	string buf = "";
	fin >> buf;
	if (buf != "<" || fin.eof()) {
		throw GameException(__LINE__, __func__, "error format \">\" ", "vec3");
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
		throw GameException(__LINE__, __func__, "error format \">\" ", "vec3");
	}
}

void getStringFromFile(ifstream& fin, double& ret) {
	string buf = "";
	fin >> buf;
	if (buf != "<" || fin.eof()) {
		throw GameException(__LINE__, __func__, "error format \">\" ", "double");
	}
	fin >> buf;
	if (buf != ">" && !fin.eof()) {
		try {
			ret = std::stod(buf);
		}
		catch (exception& e) {
			throw GameException(__LINE__, __func__, "error format \"double\" ", e.what());
		}
	}
	if (buf == ">") {
		ret = 0;
		return;
	}
	fin >> buf;
	if (buf != ">") {
		throw GameException(__LINE__, __func__, "error format \">\" ", "double");
	}
}