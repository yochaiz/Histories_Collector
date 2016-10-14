#ifndef NFT_H_
#define NFT_H_

//Remove spaces in player name beginning

#include "website.h"
#include "trie.h"
#include <fstream> //ofstream#include <string>#include <vector>using std::string;usingstd::vector;using
std::ofstream;

class NFT: public WebSite {
private:
	static string url;
	string readSeason(int& pos);
	void formatDOB(string& dob);
	void readPlayerPosition();
	void readPlayerPageAllSeasons();
	void readPlayerPageSpecificSeason();
	void readClubPage();
	void readLeaguePage();
	void readClubOrLeaguePage(string& pageStr, string& subPageStr,
			string tags[], int pos, void (NFT::*func)());
	int printLeagues(Trie<int>& leaguesTrie);
	NFT(const NFT& x); //copy c'tor
	NFT& operator=(const NFT& x);
public:
	NFT(const char* leaguePagePath, const char* outFileName,
			int* specificSeason);
	virtual void extractHistory();
	virtual void extractPosition(int val);
	~NFT();
};

#endif /* NFT_H_ */
