#include "history_entry.h"

HistoryEntry::HistoryEntry(string& season, string& clubName, string& apps,
		string& goals) :
		season(season), clubName(clubName), apps(apps), goals(goals), loan("0") {
}

HistoryEntry::HistoryEntry(string& season, string& clubName) :
		season(season), clubName(clubName) {
}

HistoryEntry::HistoryEntry(const HistoryEntry& x) :
		season(x.season), clubName(x.clubName), apps(x.apps), goals(x.goals), loan(
				x.loan) {
}

void HistoryEntry::setLoan() {
	loan = "1";
}

bool operator==(const HistoryEntry& item1, const HistoryEntry& item2) {
	return ((item1.season == item2.season) && (item1.clubName == item2.clubName));
}

string HistoryEntry::getSeason() const {
	return season;
}

string HistoryEntry::getClubName() const {
	return clubName;
}

string HistoryEntry::getApps() const {
	return apps;
}

string HistoryEntry::getGoals() const {
	return goals;
}

string HistoryEntry::getLoan() const {
	return loan;
}
