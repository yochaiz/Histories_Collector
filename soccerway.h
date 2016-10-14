#ifndef SOCCERWAY_H_
#define SOCCERWAY_H_

/*determine page type and allow only public read() */

/*release static months memory */

#include "website.h"
#include "history_entry.h"
#include <fstream>
#include <vector>
using std::vector;
using std::ofstream;
using std::string;
using std::ifstream;

class Soccerway: public WebSite {
private:
	static string url;
	static string* months;
	string readSeason(int& pos);
	void formatDOB(string& dob);
	void readPlayerPosition();
	void inline readPlayerNamesDOB(string& fname, string& sname, string& DOB,
			int& pos);
	void inline readClubStats(string& clubName, string& apps, string& goals,
			int& pos);
	void readLoanData(vector<HistoryEntry>& historyVec);
	void inline printEntries(vector<HistoryEntry>& historyVec, string& fname,
			string& sname, string& DOB);
	void readLeaguePage();
	void readClubPage();
	void readPlayerPageAllSeasons();
	void readPlayerPageSpecificSeason();
	Soccerway(const Soccerway& x); //copy c'tor
	Soccerway& operator=(const Soccerway& x);
public:
	Soccerway(const char* leaguePagePath, const char* outFileName,
			int* specificSeason);
	virtual void extractHistory();
	virtual void extractPosition(int val);
	~Soccerway();
};

#endif /* SOCCERWAY_H_ */
