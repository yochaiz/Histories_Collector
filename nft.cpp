#include "nft.h"
#include "functions.h" //ifstreamToString()#include <iostream> //cout,endl#include <cstdlib> //atoiusing std::cerr;using std::cin;usingstd::endl;string NFT::url = "www.national-football-teams.com";NFT::NFT(const char* leaguePagePath, const char* outFileName,int* specificSeason) :WebSite(leaguePagePath, outFileName, url, specificSeason) {}void NFT::extractHistory() {	if (requiredSeason != "")readPlayerPageFunc	= &WebSite::readPlayerPageSpecificSeason;else	readPlayerPageFunc = &WebSite::readPlayerPageAllSeasons;	detectPageType(url, "/player/", "/leagues/");}void NFT::extractPosition(int val) {	readPlayerPageFunc = &WebSite::readPlayerPosition;	detectPageType(url, "/player/", "/leagues/");}NFT::~NFT() {}NFT::NFT(const NFT& x) :
		WebSite(x) {
	cerr << "using NFT copy c'tor" << endl;
}

NFT& NFT::operator=(const NFT& x) {
	cerr << "using NFT operator=" << endl;
	return *this;
}

/*Return season in national-football-teams player page */
string NFT::readSeason(int& pos) {
	pos = playerPageStr.rfind("class=\"season\"", pos);
	pos = playerPageStr.find(">", pos);
	pos++;
	int length = playerPageStr.find("<", pos) - pos;
	string season = playerPageStr.substr(pos, length);
	formatSeasonType1(season);
	return season;
}

void NFT::formatDOB(string& dob) {
	int pos1 = dob.find("-");
	string year = dob.substr(0, pos1);
	pos1++;
	int pos2 = dob.find("-", pos1);
	string month = dob.substr(pos1, pos2 - pos1);
	pos2++;
	string day = dob.substr(pos2);
	dob = day + "." + month + "." + year;
}

void NFT::readPlayerPosition() {

}

static void inline sortSingleQuote(string& name) {
	for (int pos = name.find("&#039;"); pos >= 0;
			pos = name.find("&#039;", pos)) {
		name[pos] = '\'';
		pos++;
		name.erase(pos, 5);
	}

}

/*Reads national-football-teams player page */
void NFT::readPlayerPageAllSeasons() {
	int pos = 0, lastPos = 0;
	string name = readName(playerPageStr, pos, "familyName", "\">", "<");
	try {
		name = readName(playerPageStr, pos, "givenName", "\">", "<") + " "
				+ name;
	} catch (WebSiteError::TAG_NOT_FOUND& e) {
		pos = 0;
	}
	sortSingleQuote(name);
	cout << name << endl;
	string DOB = readName(playerPageStr, pos, "birthDate", ">", "<");
	formatDOB(DOB);
	pos = 0; //DOB is in the end of the page
	string clubName = readName(playerPageStr, pos, "class=\"club\"",
			">\n\t\t\t\t", "\n");
	while (pos > lastPos) {
		sortSingleQuote(clubName);
		lastPos = pos;
		string season = readSeason(pos);
		string apps = readName(playerPageStr, pos, "stats matches", ">\n\t\t\t",
				"\n");
		if (apps == "-")
			apps = "0";
		string goals = readName(playerPageStr, pos, "stats goals", ">\n\t\t\t",
				"\n");
		if (goals == "-")
			goals = "0";
		outputFile << name << "," << DOB << "," << clubName << "," << season
				<< "," << apps << "," << goals << ",0" << endl;
		try {
			clubName = readName(playerPageStr, pos, "class=\"club\"",
					">\n\t\t\t\t", "\n");
		} catch (WebSiteError::TAG_NOT_FOUND& e) {
			pos = -1;
		}
	}
}

void NFT::readPlayerPageSpecificSeason() {
	int pos = 0, lastPos = -1;
	string name = readName(playerPageStr, pos, "familyName", "\">", "<");
	name = " " + name;
	name = readName(playerPageStr, pos, "givenName", "\">", "<") + name;
	sortSingleQuote(name);
	cout << name << endl;
	string DOB = readName(playerPageStr, pos, "birthDate", ">", "<");
	formatDOB(DOB);
	pos = 0; //DOB is in the end of the page
	string clubName;
	while (pos > lastPos) {
		try {
			clubName = readName(playerPageStr, pos, "class=\"club\"",
					">\n\t\t\t\t", "\n");
		} catch (WebSiteError::TAG_NOT_FOUND& e) {
			//pos = -1;
			break;
		}
		sortSingleQuote(clubName);
		lastPos = pos;
		string season = readSeason(pos);
		if (season != requiredSeason) {
			pos = lastPos + 1;
			continue;
		}
		string apps = readName(playerPageStr, pos, "stats matches", ">\n\t\t\t",
				"\n");
		if (apps == "-")
			apps = "0";
		string goals = readName(playerPageStr, pos, "stats goals", ">\n\t\t\t",
				"\n");
		if (goals == "-")
			goals = "0";
		outputFile << name << "," << DOB << "," << clubName << "," << season
				<< "," << apps << "," << goals << ",0" << endl;
	}
}

void NFT::readClubOrLeaguePage(string& pageStr, string& subPageStr,
		string tags[], int pos, void (NFT::*func)()) {
	pos = pageStr.find(tags[0], pos);
	int endPos = pageStr.find(tags[1], pos);
	while (pos < endPos) {
		int length = pageStr.find(tags[2], pos) - pos;
		string page = pageStr.substr(pos, length);
		subPageStr = saveSourcePage(url.c_str(), page.c_str());
		try {
			(this->*func)(); //readPlayerPage() or readClubPage()
		} catch (WebSiteError::TAG_NOT_FOUND& e) {
			break;
		}
		pos++;
		pos = pageStr.find(tags[0], pos);
	}
}

/*Reads national-football-teams club page */
void NFT::readClubPage() {
	string tags[3] = { "/player/", "Latest player pix", "\"" };
	readClubOrLeaguePage(clubPageStr, playerPageStr, tags, 0,
			this->readPlayerPageFunc);
}

/*Prints page leagues and returns the startPos of the selected league */
int NFT::printLeagues(Trie<int>& leaguesTrie) {
	cout << "Please select league:" << endl;
	for (int pos = leaguePageStr.find("<h5>"), i = 1; pos >= 0; pos =
			leaguePageStr.find("<h5>", pos), i++) {
		Node<int>* leaf = leaguesTrie.addWord(intToString(i));
		int* leafData = leaf->getData();
		*leafData = pos;
		cout << i << " - "
				<< readName(leaguePageStr, pos, "<h5>", "\n\t\t\t\t\t\t\t",
						"\t") << endl;
		pos++;
	}
	string input;
	Node<int>* leaf = NULL;
	for (; !leaf; leaf = leaguesTrie.searchWord(input))
		cin >> input;
	int* startPos = leaf->getData();
	return *startPos;
}

/*Reads national-football-teams league page */
void NFT::readLeaguePage() {
	Trie<int> leaguesTrie;
	int startPos = printLeagues(leaguesTrie);
	string tags[3] = { "/club/", "<h5>", "\"" };
	readClubOrLeaguePage(leaguePageStr, clubPageStr, tags, startPos,
			&NFT::readClubPage);
}
