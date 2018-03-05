#include "Types.h"

void PrintException(string& exc) {
	if (_DEBUG) {
		std::cout << &exc << std::endl;
	}
	else {
		ofstream error_log("error.log", ios_base::out | ios_base::app);
		error_log << &exc << std::endl;
		error_log.close();
	}
}