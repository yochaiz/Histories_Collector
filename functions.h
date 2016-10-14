#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm> //for std::find()using std::vector;using std::string;
using std::stringstream;using std::cout;using std::endl;namespace generalError {
class OPEN_FILE_ERROR: public std::exception {
	const char* what() const throw () {
		return "Failed to create destination file";
	}
};
}

namespace iconvError {
class ICONV_FAILURE: public std::exception {
	const char* what() const throw () {
		return "Failed to use iconv";
	}
};
class ICONV_CONVERSION_ERROR: public std::exception {
	const char* what() const throw () {
		return "Failed to convert some character using iconv";
	}
};
}

/*Removes UTF incompatible characters and returns bool whether made changes or not */
void removeUTF(string& str);

string ifstreamToString(const char* sourceFileName);

string intToString(int num);

void overwriteFileData(const char* fileName, const string& fileStr);

void replaceSpecialChars(string& str);

void restoreSpecialChars(string& str);

void removeAccentChars(string& resStr);

void doIconv(const char* toEncoding, const char* fromEncoding,
		string& outputFileStr);

void normalizeIconv(const char* toEncoding, const char* fromEncoding,
		string& outputFileStr);

void toLower(string& str);

void fileToLower(string& fileName);

template<typename T>
T stringToNumber(const string &Text)//Text not by const reference so that the function can be used with a
{                               //character array as argument
	stringstream ss(Text);
	T result;
	return ss >> result ? result : 0;
}

#endif /* FUNCTIONS_H_ */
