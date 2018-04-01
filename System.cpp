#include "System.h"

void PrintException(const string& exc) {
	ofstream error_log("error.log", ios_base::out | ios_base::app);
	error_log << &exc << std::endl;
	error_log.close();
}