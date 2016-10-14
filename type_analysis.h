#ifndef CLASS_ANALYSIS_H_
#define CLASS_ANALYSIS_H_

//Move here functions which involve only variables from this class

#include <string>
#include <fstream>
#include <iostream>
#include <set>
#include "trie.h"
#include "functions.h"
#include "index_suggestion.h"
using std::string;
using std::ifstream;
using std::set;
using std::cin;
using std::cerr;
using std::cout;
using std::endl;

#define SQUAD_SIZE 50
#define CLUB_DAT_PATH "club.dat"

struct trieItem {
	void* leaf;
	string name;
};

namespace analysisError {
class CLUB_DAT_NOT_FOUND: public std::exception {
	const char* what() const throw () {
		return "Failed to open club.dat";
	}
};
class CSV_FILE_NOT_FOUND: public std::exception {
	const char* what() const throw () {
		return "Failed to open CSV file";
	}
};
class QUIT: public std::exception {
};
}

template<class T>
class TypeAnalysis {
private:
	//void replaceName(string currName, string newName);
	void buildSugTrie(Trie<T>& sugTrie, vector<T>* namesVec);
protected:
	Trie<int>& clubsShortNameTrie;
	string fileName;
	string fileStr; //CSV file string
	ifstream csvFile;
	vector<T> csvVec;
	void buildCsvClubVec(vector<string>& csvVec);
	void replaceName(string prefix, string currName, string suffix,
			string newName);
	void removeName(string prefix, string currName, string suffix);
	void addNameToSet(string name, set<string>& namesSet);
	void buildSugSet(Trie<vector<T> >& namesTrie,
			set<IndexSuggestion<T> >& sugSet, set<string>& namesSet);
	template<class G>
	inline trieItem readUserSuggestion(Trie<G>& namesTrie);
	inline string suggestAnother();
	template<class G>
	inline void handleUserSuggestion(trieItem& item, Trie<G>& namesTrie,
			string prefix, string currName, string suffix);
	virtual void askUserToSuggest(T item)=0;
	string askUserToReplace(vector<T>* namesVec, string prefix, string clubName,
			string suffix);

public:
	TypeAnalysis(Trie<int>& clubsShortNameTrie, const string& fileName);
	TypeAnalysis(const TypeAnalysis& x); //copy c'tor
	TypeAnalysis& operator=(const TypeAnalysis& x);
	virtual ~TypeAnalysis();
};

template<class T>
TypeAnalysis<T>::TypeAnalysis(Trie<int>& clubsShortNameTrie,
		const string& fileName) :
		clubsShortNameTrie(clubsShortNameTrie), fileName(fileName) {
}

template<class T>
TypeAnalysis<T>::TypeAnalysis(const TypeAnalysis& x) :
		clubsShortNameTrie(x.clubsShortNameTrie), fileName(x.fileName) {
	cerr << "using TypeAnalysis copy c'tor" << endl;
}

template<class T>
TypeAnalysis<T>& TypeAnalysis<T>::operator=(const TypeAnalysis<T>& x) {
	cerr << "using TypeAnalysis operator=" << endl;
	return *this;
}

template<class T>
TypeAnalysis<T>::~TypeAnalysis() {
}

template<class T>
void TypeAnalysis<T>::buildCsvClubVec(vector<string>& csvVec) {
	csvFile.open(fileName.c_str());
	if (!csvFile.is_open())
		throw analysisError::CSV_FILE_NOT_FOUND();
	string line;
	getline(csvFile, line); //ignore header
	string prevName = "";
	Trie<string> historyTrie;
	while (getline(csvFile, line)) {
		int pos = line.find(",");
		pos++;
		pos = line.find(",", pos);
		pos++;
		int length = line.find(",", pos) - pos;
		string clubName = line.substr(pos, length);
		if ((clubName != prevName) && (!historyTrie.searchWord(clubName))) { //new club name which isn't in Trie
			historyTrie.addWord(clubName);
			prevName = clubName;
			csvVec.push_back(clubName);
		}
	}
	csvFile.close();
}

template<class T>
void TypeAnalysis<T>::replaceName(string prefix, string currName, string suffix,
		string newName) {
	cout << "Replacing " << currName << " with " << newName << endl << endl;
	int pos = 0;
	string temp = prefix + currName + suffix;
	pos = fileStr.find(temp, pos);
	while (pos >= 0) {
		fileStr.replace(pos + prefix.length(), currName.length(), newName);
		pos += newName.length();
		pos = fileStr.find(temp, pos);
	}
}

template<class T>
void TypeAnalysis<T>::removeName(string prefix, string currName,
		string suffix) {
	string temp = prefix + currName + suffix;
	int pos = fileStr.find(temp);
	for (; pos >= 0; pos = fileStr.find(temp, pos)) {
		int startPos = fileStr.rfind("\n", pos);
		if (startPos == string::npos)
			startPos = 0;
		int endPos = fileStr.find("\n", pos);
		fileStr.erase(startPos, endPos - startPos);
	}
	cout << currName << " was removed" << endl << endl;
}

template<class T>
void TypeAnalysis<T>::addNameToSet(string name, set<string>& namesSet) {
	int pos1 = 0, len = name.length();
	while ((pos1 < len) && (name[pos1] == ' ')) //ignoring extra spaces
		pos1++;
	int pos2 = name.find(" ", pos1);
	while (pos2 != (int) string::npos) {
		namesSet.insert(name.substr(pos1, pos2 - pos1));
		pos2++;
		while ((pos2 < len) && (name[pos2] == ' ')) //ignoring extra spaces
			pos2++;
		pos1 = pos2;
		pos2 = name.find(" ", pos2);
	}
	if (pos1 < len) //ignoring extra spaces
		namesSet.insert(name.substr(pos1, name.length() - pos1));
}

template<class T>
void TypeAnalysis<T>::buildSugSet(Trie<vector<T> >& namesTrie,
		set<IndexSuggestion<T> >& sugSet, set<string>& namesSet) {
	set<string>::iterator it = namesSet.begin();
	set<string>::iterator endIt = namesSet.end();
	for (; it != endIt; it++) {
		Node<vector<T> >* leaf = namesTrie.searchWord(*it);
		if (leaf) {
			vector<T>* vec = leaf->getData();
			if ((vec->size() > SQUAD_SIZE) || (!vec->size())) //too many suggestions or no suggestion at all
				continue;

			sugSet.insert(IndexSuggestion<T>(*it, vec));
		}
	}
}

template<class T>
template<class G>
inline trieItem TypeAnalysis<T>::readUserSuggestion(Trie<G>& namesTrie) {
	cout << "Please enter name: (case insensitive)" << endl;
	string str;
	fflush(stdin);
	getline(cin, str);
	toLower(str);
	trieItem item;
	item.leaf = namesTrie.searchWord(str);
	item.name = str;
	return item;
}

template<class T>
inline string TypeAnalysis<T>::suggestAnother() {
	cout << "do you want to suggest another club name? (y/n/r)" << endl;
	string input;
	cin >> input;
	while ((input != "y") && (input != "n") && (input != "r") && (input != "q"))
		cin >> input;
	if (input == "q")
		throw analysisError::QUIT();
	return input;
}

template<class T>
template<class G>
inline void TypeAnalysis<T>::handleUserSuggestion(trieItem& item,
		Trie<G>& namesTrie, string prefix, string currName, string suffix) {
	while (!item.leaf) {
		cout << "Club name wasn't found, ";
		string input = suggestAnother();
		if (input == "n") { //user doesn't want to suggest another
			assert(!item.leaf);
			return;
		}
		if (input == "r") {
			removeName(prefix, currName, suffix);
			return;
		}
		item = readUserSuggestion(namesTrie);
		//leaf = readUserSuggestion(namesTrie);
	}
	//return item;
}

template<class T>
void TypeAnalysis<T>::buildSugTrie(Trie<T>& sugTrie, vector<T>* namesVec) {
	typename vector<T>::iterator namesVecIt = namesVec->begin();
	typename vector<T>::iterator namesVecEndIt = namesVec->end();
	for (int i = 1; namesVecIt != namesVecEndIt; namesVecIt++, i++) {
		string iStr = intToString(i);
		//sugTrie.addWord(iStr, *namesVecIt, false);
		Node<T>* leaf = sugTrie.addWord(iStr);
		assert(leaf);
		T* suggestion = leaf->getData();
		assert(suggestion);
		*suggestion = *namesVecIt; //leaf holds suggestion
		cout << iStr << " - " << *namesVecIt << endl;
	}
	cout << "n - none of the options" << endl;
	sugTrie.addWord("n");
	cout << "i - ignore this mismatch, move to next mismatch" << endl;
	sugTrie.addWord("i");
	cout << "s - suggest another club name" << endl;
	sugTrie.addWord("s");
	cout << "r - remove any line that contains this name" << endl;
	sugTrie.addWord("r");
	sugTrie.addWord("q");
}

template<class T>
string TypeAnalysis<T>::askUserToReplace(vector<T>* namesVec, string prefix,
		string clubName, string suffix) {
	Trie<T> sugTrie;
	buildSugTrie(sugTrie, namesVec);
	string input;
	Node<T>* res = NULL;
	while (!res) { //invalid input
		cin >> input;
		res = sugTrie.searchWord(input);
	}
	if (input == "q")
		throw analysisError::QUIT();
	if ((input == "n") || (input == "i") || (input == "s"))
		return input;
	if (input == "r") {
		removeName(prefix, clubName, suffix);
		return input;
	}
//one of the clubs names options was selected
	assert(res->getData());
	T* newName = res->getData();
	replaceName(prefix, clubName, suffix, *newName);
	return input;
}

#endif /* CLASS_ANALYSIS_H_ */

/*
 template<class T>
 void TypeAnalysis<T>::replaceName(string currName, string newName) {
 replaceName("", currName, "", newName);
 }
 */

