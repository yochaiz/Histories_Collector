#ifndef HISTORY_ENTRY_H_
#define HISTORY_ENTRY_H_

#include <string>
using std::string;

class HistoryEntry {
private:
	string season;
	string clubName;
	string apps;
	string goals;
	string loan;
public:
	HistoryEntry(string& season, string& clubName, string& apps, string& goals);
	HistoryEntry(string& season, string& clubName);
	HistoryEntry(const HistoryEntry& x);
	string getSeason() const;
	string getClubName() const;
	string getApps() const;
	string getGoals() const;
	string getLoan() const;
	void setLoan();
	friend bool operator==(const HistoryEntry& item1,
			const HistoryEntry& item2);
};

#endif /* HISTORY_ENTRY_H_ */
