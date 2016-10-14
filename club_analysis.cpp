#include "club_analysis.h"
#include "functions.h"
#include <cassert>
using std::ofstream;
using std::ios;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

#define NAME_MAX_LENGTH 50

ClubAnalysis::ClubAnalysis(Trie<int>& clubsShortNameTrie, string& fileName) :
		TypeAnalysis(clubsShortNameTrie, fileName) {
}

ClubAnalysis::ClubAnalysis(const ClubAnalysis& x) :
		TypeAnalysis(x.clubsShortNameTrie, x.fileName) {
	cerr << "using ClubAnalysis copy c'tor" << endl;
}

ClubAnalysis& ClubAnalysis::operator=(const ClubAnalysis& x) {
	cerr << "using ClubAnalysis operator=" << endl;
	return *this;
}

ClubAnalysis::~ClubAnalysis() {
}

void inline ClubAnalysis::insertClubDatLongTrie(string& longNameStr,
		string& shortNameStr) {
	Node<vector<string> >* stringLeaf = clubDatLongNameTrie.addWord(
			longNameStr);
	assert(stringLeaf);
	vector<string>* vec = stringLeaf->getData();
	assert(vec);
	vec->push_back(shortNameStr);
}

void inline ClubAnalysis::insertClubDatShortTrie(string& shortNameStr,
		int clubID) {
	Node<int>* intLeaf = clubsShortNameTrie.addWord(shortNameStr);
	assert(intLeaf);
	int* leafData = intLeaf->getData();
	assert(leafData);
	*leafData = clubID;
}

void inline ClubAnalysis::dbTrieInsert(string& longNameStr,
		string& shortNameStr) {
	set<string> namesSet;
	addNameToSet(longNameStr, namesSet);
	addNameToSet(shortNameStr, namesSet);
	set<string>::iterator it = namesSet.begin();
	set<string>::iterator endIt = namesSet.end();
	for (; it != endIt; it++) {
		Node<vector<string> >* stringLeaf = dbTrie.addWord(*it);
		assert(stringLeaf);
		vector<string>* vec = stringLeaf->getData();
		assert(vec);
		vec->push_back(shortNameStr);
	}
}

void ClubAnalysis::buildClubDatTries() {
	clubDat.open(CLUB_DAT_PATH, ios::in | ios::binary);
	if (!clubDat.is_open())
		throw analysisError::CLUB_DAT_NOT_FOUND();
	int clubID = 0, pos = 4;
	clubDat.seekg(pos);
	char longName[51], shortName[51];
	while (clubDat.tellg() >= 0) {
		clubDat.read(longName, NAME_MAX_LENGTH);
		pos = 2;
		clubDat.seekg(pos, ios::cur);
		clubDat.read(shortName, NAME_MAX_LENGTH);
		string longNameStr = longName;
		string shortNameStr = shortName;
		normalizeIconv("ASCII//TRANSLIT", "ISO-8859-1", longNameStr);
		normalizeIconv("ASCII//TRANSLIT", "ISO-8859-1", shortNameStr);
		toLower(longNameStr);
		toLower(shortNameStr);
		insertClubDatLongTrie(longNameStr, shortNameStr);
		insertClubDatShortTrie(shortNameStr, clubID);
		dbTrieInsert(longNameStr, shortNameStr);
		pos = 479;
		clubDat.seekg(pos, ios::cur);
		clubID++;
	}
	clubDat.close();
}

void ClubAnalysis::askUserToSuggest(string clubName) {
	while (1) {
		trieItem item = readUserSuggestion(dbTrie);
		handleUserSuggestion(item, dbTrie, ",", clubName, ",");
		//Node<vector<string> >* leaf = readUserSuggestion(dbTrie);
		//leaf = handleUserSuggestion(leaf, dbTrie);
		if (!item.leaf) //user doesn't want to suggest another
			return;
		vector<string>* namesVec =
				((Node<vector<string> >*) item.leaf)->getData();
		if (namesVec->size() > SQUAD_SIZE) {
			cout << "There are too many options, ";
			string input = suggestAnother();
			if (input == "n") //user doesn't want to suggest another
				return;
			if (input == "r") {
				removeName(",", clubName, ",");
				return;
			}
			continue; //do the whole process again for another name
		}
		cout << "There are " << namesVec->size() << " options" << endl;
		if (askUserToReplace(namesVec, ",", clubName, ",") != "s")
			return; //prevent another round (because of while(1))
	}
}

void ClubAnalysis::findOptimalMatch(string clubName) {
	set<string> namesSet;
	addNameToSet(clubName, namesSet);
	set<IndexSuggestion<string> > sugSet;
	buildSugSet(dbTrie, sugSet, namesSet);
	cout << endl << "Can't find " << clubName << endl;
	set<IndexSuggestion<string> >::iterator sugIt = sugSet.begin();
	set<IndexSuggestion<string> >::iterator sugEndIt = sugSet.end();
	if (!sugSet.size()) { //all words are too common
		cout << "Can't find something close enough, ";
		string input = suggestAnother();
		if (input == "y") {
			askUserToSuggest(clubName);
			return;
		}
		if (input == "r") {
			removeName(",", clubName, ",");
			return;
		}
	}
	for (; sugIt != sugEndIt; sugIt++) {
		vector<string>* namesVec = (*sugIt).getNamesVec();
		cout << endl << "Comparing to clubs who has " << (*sugIt).getSubName()
				<< " in its name" << endl;
		cout << "There are " << namesVec->size() << " options:" << endl;
		string input = askUserToReplace(namesVec, ",", clubName, ",");
		if (input == "s") {
			askUserToSuggest(clubName);
			return; //we won't get back here anymore
		}
		if (input != "n") //ignoring or option was selected
			return;
	}
}

void ClubAnalysis::matchClubNames() {
	vector<string>::iterator it = csvVec.begin();
	vector<string>::iterator endIt = csvVec.end();
	Node<vector<string> >* nodeVec = NULL;
	for (; it != endIt; it++) {
		string clubName = *it;
		if (clubsShortNameTrie.searchWord(clubName)) {
			cout << "Found " << clubName << endl;
			continue;
		}
		nodeVec = clubDatLongNameTrie.searchWord(clubName);
		if (nodeVec) { //there are clubs which their long name is clubName
			vector<string>* vec = nodeVec->getData();
			assert(vec->size() > 0);
			if (vec->size() == 1) { //there is only one club in DB with that long name
				vector<string>::iterator shortNameIt = vec->begin();
				assert(shortNameIt != vec->end());
				replaceName(",", *it, ",", *shortNameIt);
				continue;
			}
			//there are more than one club in DB with that long name
			cout << endl << clubName
					<< " is the long name of the following clubs:" << endl;
			string input = askUserToReplace(vec, ",", clubName, ",");
			if (input == "s")
				askUserToSuggest(clubName);
			if (input != "n")
				continue;
		}
		//no short name or long name match
		findOptimalMatch(clubName);
	}
}

void ClubAnalysis::sortCsvClubNames() {
	buildCsvClubVec(csvVec);
	fileStr = ifstreamToString(fileName.c_str());
	buildClubDatTries();
	try {
		matchClubNames();
	} catch (analysisError::QUIT& e) {
	}
	overwriteFileData(fileName.c_str(), fileStr);
}
