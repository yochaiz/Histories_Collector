#include "soccerway.h"
#include "functions.h"
#include <iostream> //for cerr#include <cstdlib>#include <cassert>using std::cerr;
using std::endl;
#define EMPTY_DOB ".."
static string* initMonths() {
	string* months = new string[NUM_OF_MONTHS];
	months[1] = "January";
	months[2] = "February";
	months[3] = "March";
	months[4] = "April";
	months[5] = "May";
	months[6] = "June";
	months[7] = "July";
	months[8] = "August";
	months[9] = "September";
	months[10] = "October";
	months[11] = "November";
	months[12] = "December";
	return months;
}
string Soccerway::url = "soccerway.com";
string* Soccerway::months = initMonths();

Soccerway::Soccerway(const char* leaguePagePath, const char* outFileName,
		int* specificSeason) :
		WebSite(leaguePagePath, outFileName, url, specificSeason) {
}

void Soccerway::extractHistory() {
	if (requiredSeason != "")
		readPlayerPageFunc = &WebSite::readPlayerPageSpecificSeason;
	else
		readPlayerPageFunc = &WebSite::readPlayerPageAllSeasons;

	detectPageType(url, "/players/", "/national/");
}

void Soccerway::extractPosition(int val) {
	readPlayerPageFunc = &WebSite::readPlayerPosition;
	detectPageType(url, "/players/", "/national/");
}

Soccerway::~Soccerway() {
}

Soccerway::Soccerway(const Soccerway& x) :
		WebSite(x) {
	cerr << "using Soccerway copy c'tor" << endl;
}

Soccerway& Soccerway::operator=(const Soccerway& x) {
	cerr << "using Soccerway operator=" << endl;
	return *this;
}

/*Return season in Soccerway player page */
string Soccerway::readSeason(int& pos) {
	pos = playerPageStr.find(">", pos);
	pos++;
	pos = playerPageStr.find(">", pos);
	pos++;
	int length = playerPageStr.find("<", pos) - pos;
	string season = playerPageStr.substr(pos, length);
	return (length > 4) ? season.substr(5) : season;
}

/*Changes DOB string from Soccerway format to CSV format */
void Soccerway::formatDOB(string& dob) {
	int pos1 = dob.find(" ");
	string day = dob.substr(0, pos1);
	if (day.length() == 1)
		day.insert(0, "0");
	pos1++;
	int pos2 = dob.find(" ", pos1);
	string month = dob.substr(pos1, pos2 - pos1);
	month = convertMonth(months, month);
	pos2++;
	string year = dob.substr(pos2);
	dob = day + "." + month + "." + year;
}

void Soccerway::readPlayerPosition() {

}

void inline Soccerway::readPlayerNamesDOB(string& fname, string& sname,
		string& DOB, int& pos) {
	fname = readName(playerPageStr, pos, "<dd>", ">", "<");
	sname = readName(playerPageStr, pos, "<dd>", ">", "<");
	pos = playerPageStr.find("Date of birth", pos); //preparation for DOB
	DOB = readName(playerPageStr, pos, "<dd>", ">", "<");
	formatDOB(DOB);
}

void inline Soccerway::readClubStats(string& clubName, string& apps,
		string& goals, int& pos) {
	clubName = readName(playerPageStr, pos, "title=", "\"", "\"");
	apps = readName(playerPageStr, pos, "appearances", ">", "<");
	if (apps == "?")
		apps = "0";
	goals = readName(playerPageStr, pos, "goals", ">", "<");
	if (goals == "?")
		goals = "0";
}

void determineLoanSeason(string& date) {
	string month = date.substr(3, 2);
	string year = date.substr(6, 2);
	int yearInt = atoi(year.c_str());
	if (atoi(month.c_str()) >= 6) { //next year
		yearInt++;
		year = intToString(yearInt);
		if (yearInt < 10)
			year = "0" + year;
	}
	if (yearInt < 50)
		date = "20" + year;
	else
		date = "19" + year;
}

void Soccerway::readLoanData(vector<HistoryEntry>& historyVec) {
	int pos = playerPageStr.find(">Loan<");
	for (; pos >= 0; pos = playerPageStr.find(">Loan<", pos)) {
		int pos2 = playerPageStr.rfind("<td class=\"date\">", pos);
		assert(pos >= 0);
		string season = readName(playerPageStr, pos2, "<span", ">", "<");
		determineLoanSeason(season);
		pos2 = playerPageStr.find("title=", pos2); //skipping owner club
		pos2++;
		string clubName = readName(playerPageStr, pos2, "title=", "\"", "\"");
		vector<HistoryEntry>::iterator it = find(historyVec.begin(),
				historyVec.end(), HistoryEntry(season, clubName));
		if (it != historyVec.end())
			(*it).setLoan();
		pos++;
	}
}

void inline Soccerway::printEntries(vector<HistoryEntry>& historyVec,
		string& fname, string& sname, string& DOB) {
	vector<HistoryEntry>::iterator it = historyVec.begin();
	vector<HistoryEntry>::iterator endIt = historyVec.end();
	for (; it != endIt; it++) {
		outputFile << fname << " " << sname << "," << DOB << ","
				<< (*it).getClubName() << "," << (*it).getSeason() << ","
				<< (*it).getApps() << "," << (*it).getGoals() << ","
				<< (*it).getLoan() << endl;
	}
}

/*Reads Soccerway player page */
void Soccerway::readPlayerPageAllSeasons() {
	int pos = 0;
	string fname, sname, DOB;
	readPlayerNamesDOB(fname, sname, DOB, pos);
	if (DOB == EMPTY_DOB) //player without DOB
		return;
	cout << fname << " " << sname << endl;
	pos = playerPageStr.find("<td class=\"season\">", pos);
	vector<HistoryEntry> historyVec;
	for (; pos >= 0; pos = playerPageStr.find("<td class=\"season\">", pos)) {
		string season = readSeason(pos);
		string clubName, apps, goals;
		readClubStats(clubName, apps, goals, pos);
		historyVec.push_back(HistoryEntry(season, clubName, apps, goals));
	}
	readLoanData(historyVec);
	printEntries(historyVec, fname, sname, DOB);
}

/*Reads specific season from Soccerway player page */
void Soccerway::readPlayerPageSpecificSeason() {
	int pos = 0;
	string fname, sname, DOB;
	readPlayerNamesDOB(fname, sname, DOB, pos);
	if (DOB == EMPTY_DOB) //player without DOB
		return;
	cout << fname << " " << sname << endl;
	pos = playerPageStr.find("<td class=\"season\">", pos);
	vector<HistoryEntry> historyVec;
	for (; pos >= 0; pos = playerPageStr.find("<td class=\"season\">", pos)) {
		string season = readSeason(pos);
		if (season != requiredSeason)
			continue;
		string clubName, apps, goals;
		readClubStats(clubName, apps, goals, pos);
		historyVec.push_back(HistoryEntry(season, clubName, apps, goals));
	}
	readLoanData(historyVec);
	printEntries(historyVec, fname, sname, DOB);
}

/*Reads Soccerway club page */
void Soccerway::readClubPage() {
	int pos = 0;
	pos = clubPageStr.find(">Squad<", pos);
	pos++;
	pos = clubPageStr.find(">Squad<", pos);
	pos = clubPageStr.find("\"/players/", pos);
	do {
		pos++;
		int length = clubPageStr.find("\"", pos) - pos;
		string page = clubPageStr.substr(pos, length);
		playerPageStr = saveSourcePage(url.c_str(), page.c_str());
		(this->*readPlayerPageFunc)(); //readPlayerPage();
		pos = clubPageStr.find("\"/players/", pos); //every entry appears twice
		pos++;
		pos = clubPageStr.find("\"/players/", pos);
	} while (pos > 0);
}

/*Reads Soccerway league page */
void Soccerway::readLeaguePage() {
	int pos = leaguePageStr.find("text team large");
	int endPos = leaguePageStr.find(">Tables<", pos);
	while (pos < endPos) {
		string path = readName(leaguePageStr, pos, "href=", "\"", "\"");
		clubPageStr = saveSourcePage(url.c_str(), path.c_str());
		readClubPage();
		pos = leaguePageStr.find("text team large", pos);
	}
}
