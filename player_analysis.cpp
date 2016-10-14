#include "player_analysis.h"
#include "functions.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <algorithm>
using std::find;
using std::cout;
using std::cerr;
using std::endl;

#define INDEX_NUM_OF_STAFF_POSITION 532
#define DOB_AMOUNT 2
#define LEAP_YEAR_CYCLE 4

#define NAME_MAX_LENGTH 50
#define NAME_BLOCK_SIZE 60

#define CLUB_BLOCK_SIZE 581
#define CLUB_BLOCK_SQUAD_POSITION 215

#define STAFF_BLOCK_SIZE 110
#define STAFF_BLOCK_FNAME_POSITION 4
#define STAFF_BLOCK_DOB_DAY_POSITION 16
#define STAFF_BLOCK_CLASS_POSITION 61
#define JOB_PLAYER 11

#define DAY_OF_YEAR 0
#define YEAR 1

#define FNAME 0
#define SNAME 1
#define CNAME 2

#define INDEX_DAT_PATH "index.dat"
#define FNAME_DAT_PATH "first_names.dat"
#define SNAME_DAT_PATH "second_names.dat"
#define CNAME_DAT_PATH "common_names.dat"
#define STAFF_DAT_PATH "staff.dat"

PlayerAnalysis::PlayerAnalysis(Trie<int>& clubsShortNameTrie,
		const string& fileName) :
		TypeAnalysis(clubsShortNameTrie, fileName) {
	notifyClubMismatch = false;
	cout << endl << "Do you want to confirm players club mismatches? (y/n)"
			<< endl;
	string input;
	cin >> input;
	while ((input != "y") && (input != "n"))
		cin >> input;
	if (input == "y")
		notifyClubMismatch = true;
}

PlayerAnalysis::PlayerAnalysis(const PlayerAnalysis& x) :
		TypeAnalysis(x.clubsShortNameTrie, x.fileName), notifyClubMismatch(
				x.notifyClubMismatch), clubCsvVec(x.clubCsvVec) {
	cerr << "using PlayerAnalysis copy c'tor" << endl;
}

PlayerAnalysis& PlayerAnalysis::operator=(const PlayerAnalysis& x) {
	cerr << "using PlayerAnalysis operator=" << endl;
	return *this;
}

PlayerAnalysis::~PlayerAnalysis() {
}

void PlayerAnalysis::buildCsvPlayerSet() {
	string line;
	getline(csvFile, line); //ignore header
	string prevName = "";
	Trie<string> historyTrie; //holds player names we have seen already
	while (getline(csvFile, line)) {
		int pos = line.find(",");
		string name = line.substr(0, pos);
		if ((name != prevName) && (!historyTrie.searchWord(name))) {
			pos++; //ignoring day of birth
			pos = line.find(",", pos);
			pos++;
			int length = line.find(",", pos) - pos;
			string clubName = line.substr(pos, length);
			CsvPlayer pl(name, clubName);
			prevName = name;
			csvVec.push_back(pl);
			historyTrie.addWord(name);
		}
	}
}

void inline PlayerAnalysis::openNameDat() {
	files[FNAME] = fopen(FNAME_DAT_PATH, "rb");
	files[SNAME] = fopen(SNAME_DAT_PATH, "rb");
	files[CNAME] = fopen(CNAME_DAT_PATH, "rb");
	for (int i = FNAME; i <= CNAME; i++) {
		if (!files[i])
			throw playerError::PLAYER_EXCEPTION();
	}
}

dbNames PlayerAnalysis::buildDBName(const unsigned int namesID[]) {
	int pos = (NAME_BLOCK_SIZE * namesID[FNAME]);
	char name[NAME_MAX_LENGTH];
	fseek(files[FNAME], pos, SEEK_SET);
	fread(name, NAME_MAX_LENGTH, 1, files[FNAME]);
	dbNames names;
	names.isCommonName = false;
	names.fullName = name;
	pos = (NAME_BLOCK_SIZE * namesID[SNAME]);
	fseek(files[SNAME], pos, SEEK_SET);
	fread(name, NAME_MAX_LENGTH, 1, files[SNAME]);
	names.fullName += " ";
	names.fullName += name;
	normalizeIconv("ASCII//TRANSLIT", "ISO-8859-1", names.fullName);
	toLower(names.fullName);
	if (namesID[CNAME]) { //there is common name
		assert(namesID[CNAME] > 0);
		pos = (NAME_BLOCK_SIZE * namesID[CNAME]);
		fseek(files[CNAME], pos, SEEK_SET);
		fread(name, NAME_MAX_LENGTH, 1, files[CNAME]);
		names.commonName = name;
		names.isCommonName = true;
		normalizeIconv("ASCII//TRANSLIT", "ISO-8859-1", names.commonName);
		toLower(names.commonName);
	}
	return names;
}

inline FILE* PlayerAnalysis::openStaffDat() {
	return fopen(STAFF_DAT_PATH, "rb");
}

void inline PlayerAnalysis::readPlayerNamesIDsFromStaffDat(FILE* fileDat,
		int position, int seekType, unsigned int names[]) {
	fseek(fileDat, position, seekType);
	fread(names, sizeof(int), NAMES_AMOUNT, fileDat);
}

inline FILE* PlayerAnalysis::buildStaffClubSquadPreLoop(int clubID, int playersID[],
		FILE* clubDat) {
	clubDat = fopen( CLUB_DAT_PATH, "rb");
	if (!clubDat)
		throw playerError::PLAYER_EXCEPTION();
	fseek(clubDat, (clubID * CLUB_BLOCK_SIZE) + CLUB_BLOCK_SQUAD_POSITION,
	SEEK_SET);
	fread(playersID, sizeof(int), SQUAD_SIZE, clubDat);
	fclose(clubDat);
	FILE* staffDat = openStaffDat();
	if (!staffDat)
		throw playerError::PLAYER_EXCEPTION();
	return staffDat;
}

void inline PlayerAnalysis::buildStaffClubSquadLoop(int index, int playersID[],
		FILE* staffDat, set<string>& namesSet, dbNames& plNames) {
	unsigned int namesID[NAMES_AMOUNT];
	readPlayerNamesIDsFromStaffDat(staffDat,
			(playersID[index] * STAFF_BLOCK_SIZE) + STAFF_BLOCK_FNAME_POSITION,
			SEEK_SET, namesID);
	plNames = buildDBName(namesID);
	addNameToSet(plNames.fullName, namesSet);
	if (plNames.isCommonName)
		addNameToSet(plNames.commonName, namesSet);
}

void inline PlayerAnalysis::buildStaffClubSquadAdd(
		Trie<vector<CsvPlayer> >* squadTrie, string data, string playerName,
		string clubName) {
	Node<vector<CsvPlayer> >* leaf = squadTrie->addWord(data);
	assert(leaf);
	vector<CsvPlayer>* vec = leaf->getData();
	vec->push_back(CsvPlayer(playerName, clubName));
}

void PlayerAnalysis::buildStaffClubSquad(
		Trie<vector<CsvPlayer> >* squadTrieAr[], int trieArSize, int clubID,
		string clubName) {
	FILE* clubDat = NULL;
	int playersID[SQUAD_SIZE];
	FILE* staffDat = buildStaffClubSquadPreLoop(clubID, playersID, clubDat);
	assert(staffDat);
	for (int i = 0; i < SQUAD_SIZE; i++) {
		if (playersID[i] < 0)
			continue;
		set<string> namesSet;
		dbNames plNames;
		buildStaffClubSquadLoop(i, playersID, staffDat, namesSet, plNames);
		set<string>::iterator it = namesSet.begin();
		set<string>::iterator endIt = namesSet.end();
		for (; it != endIt; it++) {
			for (int j = 0; j < trieArSize; j++)
				buildStaffClubSquadAdd(squadTrieAr[j], *it, plNames.fullName,
						clubName);
		}
		for (int j = 0; j < trieArSize; j++)
			buildStaffClubSquadAdd(squadTrieAr[j], plNames.fullName,
					plNames.fullName, clubName);
	}
	clubsSquadIDs.insert(clubID);
	fclose(staffDat);
}

void PlayerAnalysis::askUserToSuggest(CsvPlayer player) {
	trieItem item = readUserSuggestion(clubsShortNameTrie);
	handleUserSuggestion(item, clubsShortNameTrie, "", player.getName(), ",");
	if (!item.leaf) //user doesn't want to suggest another
		return;
	int* clubID = ((Node<int>*) item.leaf)->getData();
	assert(clubID);
	Trie<vector<CsvPlayer> > squadTrie;
	Trie<vector<CsvPlayer> >* squadTrieAr[2];
	squadTrieAr[0] = &squadTrie;
	squadTrieAr[1] = &clubsSqaudTrie;
	if (find(clubsSquadIDs.begin(), clubsSquadIDs.end(), *clubID)
			== clubsSquadIDs.end()) //assuring club squad wasn't loaded yet
		buildStaffClubSquad(squadTrieAr, 2, *clubID, item.name); //Adding new club squad to clubsSquadTrie
	else
//club squad was loaded before, loading only for squadTrie
		buildStaffClubSquad(squadTrieAr, 1, *clubID, item.name);
	searchByClub(squadTrie, player);
}

void PlayerAnalysis::searchByClub(Trie<vector<CsvPlayer> >& squadTrie,
		CsvPlayer player) {
	Node<vector<CsvPlayer> >* leaf = squadTrie.searchWord(player.getName());
	if (leaf) {
		vector<CsvPlayer>* vec = leaf->getData();
		if (find(vec->begin(), vec->end(), player) != vec->end()) {
			cout << "Found " << player.getName() << " at "
					<< player.getClubName() << endl;
			return;
		} else {
			cout << endl << "Can't find " << player.getName() << " at "
					<< player.getClubName() << endl;
			cout << "But found " << vec->size() << " players named "
					<< player.getName() << ":" << endl;
			vector<CsvPlayer>::iterator vecIt = vec->begin();
			vector<CsvPlayer>::iterator vecEndIt = vec->end();
			for (; vecIt != vecEndIt; vecIt++)
				cout << *vecIt << endl;
			cout << endl;
			if (!notifyClubMismatch)
				return;
			cout << "Does it cover the player you meant to? (y/n)" << endl;
			string input;
			cin >> input;
			while ((input != "n") && (input != "y") && (input != "q"))
				cin >> input;
			if (input == "q")
				throw analysisError::QUIT();
			if (input == "y")
				return;
			cout << endl << "Looking for partial match for " << player.getName()
					<< endl;

		}
	}
	if (!leaf)
		cout << endl << "Can't find exact match for " << player.getName()
				<< endl;
	set<string> namesSet;
	addNameToSet(player.getName(), namesSet);
	set<IndexSuggestion<CsvPlayer> > sugSet;
	buildSugSet(squadTrie, sugSet, namesSet);
	if (!sugSet.size()) { //all words are too common
		cout << "Can't find something close enough, ";
		string input = suggestAnother();
		if (input == "y") {
			askUserToSuggest(player);
			return;
		}
		if (input == "r") {
			removeName("", player.getName(), ",");
			return;
		}

	}
	set<IndexSuggestion<CsvPlayer> >::iterator sugIt = sugSet.begin();
	set<IndexSuggestion<CsvPlayer> >::iterator sugEndIt = sugSet.end();
	for (; sugIt != sugEndIt; sugIt++) {
		vector<CsvPlayer>* namesVec = (*sugIt).getNamesVec();
		cout << endl << "Comparing to players who has " << (*sugIt).getSubName()
				<< " in its name" << endl;
		cout << "There are " << namesVec->size() << " options:" << endl;
		string input = askUserToReplace(namesVec, "", player.getName(), ",");
		if (input == "s") {
			askUserToSuggest(player);
			return; //we won't get back here anymore
		}
		if (input != "n") //ignoring or option was selected
			return;
	}
}

void PlayerAnalysis::buildClubsSqaudTrie() {
	assert(clubCsvVec.size() > 0);
	vector<string>::iterator it = clubCsvVec.begin();
	vector<string>::iterator endIt = clubCsvVec.end();
	for (; it != endIt; it++) {
		Node<int>* leaf = clubsShortNameTrie.searchWord(*it);
		if (!leaf) //club CSV name doesn't match DB short name
			continue;
		int* clubID = leaf->getData();
		assert(clubID);
		Trie<vector<CsvPlayer> >* squadTrieAr[1];
		squadTrieAr[0] = &clubsSqaudTrie;
		buildStaffClubSquad(squadTrieAr, 1, *clubID, *it);
	}
}

void PlayerAnalysis::matchPlayerNames() {
	openNameDat();
	buildClubsSqaudTrie();
	vector<CsvPlayer>::iterator csvVecIt = csvVec.begin();
	vector<CsvPlayer>::iterator csvVecEndIt = csvVec.end();
	for (; csvVecIt != csvVecEndIt; csvVecIt++) {
		searchByClub(clubsSqaudTrie, *csvVecIt);
	}
	for (int i = FNAME; i <= CNAME; i++) {
		fclose(files[i]);
	}
}

void PlayerAnalysis::sortCsvPlayerNames() {
	buildCsvClubVec(clubCsvVec); //in order to apply changes made while analyzing clubs
	csvFile.open(fileName.c_str());
	buildCsvPlayerSet();
	csvFile.close();
	fileStr = ifstreamToString(fileName.c_str());
	try {
		matchPlayerNames();
	} catch (analysisError::QUIT& e) {
	}
	overwriteFileData(fileName.c_str(), fileStr);
}

