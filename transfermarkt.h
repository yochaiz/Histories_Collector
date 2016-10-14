#ifndef TRANSFERMARKT_H_
#define TRANSFERMARKT_H_

#include "website.h"
#include "history_entry.h"
#include "functions.h"
#include <map>
#include <vector>
using std::map;
using std::vector;

#define NUM_OF_POSITIONS 9
#define NUM_OF_SIDES 3

class Position {
private:
	vector<int> posVec;
	vector<int> sideVec;
public:
	Position() {
	}

	Position(int a) {
		posVec.push_back(a);
	}

	Position(int a, int b) {
		posVec.push_back(a);
		sideVec.push_back(b);
	}

	Position(int a1, int a2, int b) {
		posVec.push_back(a1);
		posVec.push_back(a2);
		sideVec.push_back(b);
	}

	void updatePositions(int* positions, int value) {
		vector<int>::iterator it = posVec.begin();
		vector<int>::iterator endIt = posVec.end();
		for (; it != endIt; it++)
			if (positions[*it] < value)
				positions[*it] = value;
	}

	void updateSides(int* sides, int value) {
		vector<int>::iterator it = sideVec.begin();
		vector<int>::iterator endIt = sideVec.end();
		for (; it != endIt; it++)
			if (sides[*it] < value)
				sides[*it] = value;
	}
};

class Transfermarkt: public WebSite {
private:
	static string url;
	static string* months;
	static map<string, int> valuesMap;
	static map<string, Position> positionMap;
	int secondPosValue;
	void (*sortPlayerURL)(string&);
	string readSeason(int& pos);
	string readField(int& pos, string tag1, string tag2, string tag3);
	void clearExtraTags(string& name, string& fname, string& sname,
			string& cname, bool searchFullName);
	void formatDOB(string& dob);
	void formatValue(string& value);
	void convertPosition(int* positions, int* sides, string& sitePosName,
			int value);
	void convertFoot(string& foot, int& right, int& left, int* sides);
	void printPositions(int* positions, int* sides);
	void readPlayerPosition();
	void readLoanData(vector<HistoryEntry>& historyVec);
	void readPlayerPageAllSeasons();
	void readLeaguePage();
	void readClubPage();
	void readPlayerPageSpecificSeason();
	Transfermarkt(const Transfermarkt& x); //copy c'tor
	Transfermarkt& operator=(const Transfermarkt& x);
public:
	Transfermarkt(const char* leaguePagePath, const char* outFileName,
			int* specificSeason);
	virtual void extractHistory();
	virtual void extractPosition(int val);
	~Transfermarkt();
};

#endif /* TRANSFERMARKT_H_ */
