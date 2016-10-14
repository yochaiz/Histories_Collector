#ifndef PLAYER_ANALYSIS_H_
#define PLAYER_ANALYSIS_H_

#include <vector>
#include "csv_player.h"
#include "type_analysis.h"
using std::vector;

#define NAMES_AMOUNT 3

struct dbNames {
	string fullName;
	string commonName;
	bool isCommonName;
};

namespace playerError {
class PLAYER_EXCEPTION: public std::exception {
	const char* what() const throw () {
		return "Player class error";
	}
};
}

class PlayerAnalysis: public TypeAnalysis<CsvPlayer> {
private:
	bool notifyClubMismatch;
	Trie<vector<CsvPlayer> > clubsSqaudTrie;
	set<int> clubsSquadIDs; //holds the IDs of all the clubs that their squad was loaded to prevent duplicate loading
	vector<string> clubCsvVec;
	FILE* files[NAMES_AMOUNT];
	void matchPlayerNames();
	inline FILE* buildStaffClubSquadPreLoop(int clubID, int playersID[],
			FILE* clubDat);
	void inline buildStaffClubSquadLoop(int index, int playersID[],
			FILE* staffDat, set<string>& namesSet, dbNames& plNames);
	void inline buildStaffClubSquadAdd(Trie<vector<CsvPlayer> >* squadTrie,
			string data, string playerName, string clubName);
	void buildStaffClubSquad(Trie<vector<CsvPlayer> >* squadTrieAr[],
			int trieArSize, int clubID, string clubName);
	inline FILE* openStaffDat();
	dbNames buildDBName(const unsigned int namesID[]);
	void inline openNameDat();
	void buildCsvPlayerSet();
	void inline readPlayerNamesIDsFromStaffDat(FILE* fileDat, int position,
			int seekType, unsigned int names[]);
	void searchByClub(Trie<vector<CsvPlayer> >& squadTrie, CsvPlayer player);
	virtual void askUserToSuggest(CsvPlayer player);
	void buildClubsSqaudTrie();
public:
	PlayerAnalysis(Trie<int>& clubsShortNameTrie, const string& fileName);
	PlayerAnalysis(const PlayerAnalysis& x); //copy c'tor
	PlayerAnalysis& operator=(const PlayerAnalysis& x);
	void sortCsvPlayerNames();
	~PlayerAnalysis();
};

#endif /* PLAYER_ANALYSIS_H_ */
