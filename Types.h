#define GLEW_STATIC

#ifndef Types_H
#define Types_H

#include <string>
#include <exception>
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>

using std::string;
using std::exception;

const bool TEST = true;

class Exception_t {
public:
	Exception_t(size_t LINE, string FILE, string INF = "default") : LINE_(LINE), FILE_(FILE), INF_(INF) { }
	Exception_t(size_t LINE, string FILE, string INF1, string INF2) : LINE_(LINE), FILE_(FILE), INF_(INF1 + " " + INF2) { }
	string& what() { return std::to_string(LINE_) + "::" + FILE_ + "::" + INF_; }
	void print() { printf("LINE:%zu FILE:%s INFO:%s\n", LINE_, FILE_.c_str(), INF_.c_str()); }
private:
	size_t LINE_;
	string FILE_;
	string INF_;
};

#endif