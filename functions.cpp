#include "functions.h"
#include <iconv.h>
using std::ifstream;
using std::ofstream;

#define ICONV_BLOCK_SIZE 1000
#define UPPER_TO_LOWER_ASCII_GAP 32

void removeUTF(string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] == -60) {
			i++;
			if (i >= len)
				continue;
			if (str[i] == -79)
				str.replace(i - 1, 2, "i");
			else if (str[i] == -112)
				str.replace(i - 1, 2, "D");
			else if (str[i] == -111)
				str.replace(i - 1, 2, "d");
		} else if (str[i] == -61) {
			i++;
			if (i >= len)
				continue;
			if (str[i] == -72)
				str.replace(i - 1, 2, "o");
		} else if (str[i] == -55) {
			i++;
			if (i >= len)
				continue;
			if (str[i] == -103) //ə
				str.replace(i - 1, 2, "a");
		}
	}
}

string ifstreamToString(const char* sourceFileName) {
	ifstream source(sourceFileName);
	stringstream buf;
	buf << source.rdbuf();
	return buf.str();
}

void overwriteFileData(const char* fileName, const string& fileStr) {
	ofstream outputFile;
	outputFile.open(fileName);
	outputFile << fileStr;
	outputFile.close();
}

string intToString(int num) {
	stringstream ss;
	ss << num;
	return ss.str();
}

void replaceSpecialChars(string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] == -16) //Ã°
			str[i] = '*';
		else if (str[i] == -34) //Ãž
			str[i] = '@';
		else if (str[i] == -8) //Ã¸
			str[i] = '+';
		else if (str[i] == -2) //Ã¾
			str[i] = '#';
		else if (str[i] == -40) //Ã˜
			str[i] = '=';
		else if (str[i] == -48) //Ã�
			str[i] = 'D';
		else if ((str[i] == -70) && ((i == 0) || ((i > 0) && (str[i - 1] > 0)))) //Âº
			str[i] = '!';
		else if ((str[i] == -31) && ((i + 1) < len) && (str[i + 1] == -72)
				&& ((i + 2) < len) && (str[i + 2] == -115)) {
			//replace ḍ with d
			str[i] = 'd';
			str.erase(i + 1, 2);
		}
		//WINDOWS ONLY ADDITION
		else if (str[i] == '\'') //'
			str[i] = '$';
		else if (str[i] == '"') //"
			str[i] = '%';
		else if (str[i] == '`') //`
			str[i] = '}';
		else if (str[i] == '~') //~
			str[i] = '|';
	}
}

void restoreSpecialChars(string& str) {
	int len = str.length();
	for (int i = 0; i < len; i++) {
		if (str[i] == '*') //Ã°
			str[i] = -16;
		else if (str[i] == '@') //Ãž
			str[i] = -34;
		else if (str[i] == '+') //Ã¸
			str[i] = 'o';
		else if (str[i] == '#') //Ã¾
			str[i] = -2;
		else if (str[i] == '=') //Ã˜
			str[i] = -40;
		else if (str[i] == '!') //Âº
			str[i] = -70;
		//WINDOWS ONLY ADDITION
		else if (str[i] == '$') //'
			str[i] = '\'';
		else if (str[i] == '%') //"
			str[i] = '"';
		else if (str[i] == '}') //`
			str[i] = '`';
		else if (str[i] == '|') //~
			str[i] = '~';
	}
}

void removeAccentChars(string& resStr) {
	int len = resStr.length(), i = 0;
	while (i < len) {
		if (resStr[i] == '^') {
			resStr.erase(i, 1);
			continue;
		} else if (resStr[i] == '`') {
			resStr.erase(i, 1);
			continue;
		} else if (resStr[i] == '~') {
			resStr.erase(i, 1);
			continue;
		} else if (resStr[i] == '\'') {
			resStr.erase(i, 1);
			continue;
		} else if (resStr[i] == '\"') {
			resStr.erase(i, 1);
			continue;
		}
		i++;
	}
}

void doIconv(const char* toEncoding, const char* fromEncoding,
		string& outputFileStr) {
	iconv_t conv = iconv_open(toEncoding, fromEncoding);
	if (conv == (iconv_t) -1)
		throw iconvError::ICONV_FAILURE();
	int outputFileStrLength = outputFileStr.length();
	int pos = 0;
	string resStr = "";
	while (pos < outputFileStrLength) {
		int inSizeInt = ICONV_BLOCK_SIZE;
		char bufIn[ICONV_BLOCK_SIZE + 1] = { 0 };
		char bufOut[ICONV_BLOCK_SIZE * 2] = { 0 };
		int tempPos = pos + inSizeInt - 1;
		while ((tempPos < outputFileStrLength)
				&& (outputFileStr[tempPos] != ',')
				&& (outputFileStr[tempPos] != '\n')) {
			inSizeInt--;
			tempPos--;
		}
		strcpy(bufIn, (outputFileStr.substr(pos, inSizeInt)).c_str());
		pos += inSizeInt;
		char* pIn = bufIn;
		char* pOut = bufOut;
		size_t inSize = inSizeInt, outSize = ICONV_BLOCK_SIZE * 2;
		size_t res = iconv(conv, &pIn, &inSize, &pOut, &outSize);
		if ((int) res == -1) {
			std::cerr << bufIn << endl;
			throw iconvError::ICONV_CONVERSION_ERROR();
		}
		resStr += bufOut;
	}
	iconv_close(conv);
	outputFileStr = resStr;
}

void normalizeIconv(const char* toEncoding, const char* fromEncoding,
		string& outputFileStr) {
	replaceSpecialChars(outputFileStr);
	//WINDOWS ONLY ADDITION
	doIconv(toEncoding, fromEncoding, outputFileStr);
	removeAccentChars(outputFileStr);
	//WINDOWS ONLY ADDITION - END
	restoreSpecialChars(outputFileStr);
}

void toLower(string& str) {
	int length = str.length();
	for (int i = 0; i < length; i++) {
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
			str[i] += UPPER_TO_LOWER_ASCII_GAP;
	}
}

void fileToLower(string& fileName) {
	string fileStr = ifstreamToString(fileName.c_str());
	toLower(fileStr);
	overwriteFileData(fileName.c_str(), fileStr);
}

