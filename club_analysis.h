#ifndef CLUB_ANALYSIS_H_
#define CLUB_ANALYSIS_H_

#include "type_analysis.h"

class ClubAnalysis: public TypeAnalysis<string> {
private:
	Trie<vector<string> > clubDatLongNameTrie;
	Trie<vector<string> > dbTrie;
	ifstream clubDat;
	void inline insertClubDatLongTrie(string& longNameStr,
			string& shortNameStr);
	void inline insertClubDatShortTrie(string& shortNameStr, int clubID);
	void inline dbTrieInsert(string& longNameStr, string& shortNameStr);
	void findOptimalMatch(string clubName);
	void matchClubNames();
	void addToDBSet(string longName, string shortName);
	void addNamesSetToDBSet(string name, set<string>& namesSet);
	void findOptimalByIndex(string clubName, string orgClubName);
	virtual void askUserToSuggest(string clubName);
public:
	ClubAnalysis(Trie<int>& clubsShortNameTrie, string& fileName);
	ClubAnalysis(const ClubAnalysis& x); //copy c'tor
	ClubAnalysis& operator=(const ClubAnalysis& x);
	void sortCsvClubNames();
	void buildClubDatTries();
	~ClubAnalysis();
};

#endif /* CLUB_ANALYSIS_H_ */
